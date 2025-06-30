#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include "mime_types.h"
#include "http.h"
#include "logger.h"

#define BUFFER_SIZE 4096
#define MAX_HEADER_SIZE 512
#define MAX_PATH_LEN 512

// Helper function: send all bytes (handle partial sends)
static ssize_t send_all(int sockfd, const void *buf, size_t len) {
    size_t total_sent = 0;
    const char *data = (const char *)buf;
    while (total_sent < len) {
        ssize_t sent = send(sockfd, data + total_sent, len - total_sent, 0);
        if (sent <= 0) {
            return -1;  // error
        }
        total_sent += sent;
    }
    return total_sent;
}

void handle_http_connection(int client_fd)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0)
    {
        LOG_ERR("Failed to receive HTTP request");
        close(client_fd);
        return;
    }

    LOG_INFO("Received HTTP request:\n%s", buffer);

    char method[8], path[256];
    if (sscanf(buffer, "%7s %255s", method, path) != 2)
    {
        LOG_ERR("Malformed HTTP request");
        close(client_fd);
        return;
    }
    // Only handle GET requests
    if (strcmp(method, "GET") != 0)
    {
        const char *method_not_allowed_response = 
        "HTTP/1.1 405 Method Not Allowed\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n";
        send_all(client_fd, method_not_allowed_response, strlen(method_not_allowed_response));
        close(client_fd);
        return;
    }


    // Serve index.html by default and avoid path traversal
    char safe_path[MAX_PATH_LEN];
    if (strstr(path, ".."))
    {
        // Reject path traversal attempts
        const char *bad_request =
        "HTTP/1.1 400 Bad Request\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n";
        send_all(client_fd, bad_request, strlen(bad_request));
        close(client_fd);
        return;
    }
    const char *relative_path = (strcmp(path, "/") == 0) ? "index.html" : path + 1;
    snprintf(safe_path, sizeof(safe_path), "static/%s", relative_path);

    // Open requested file
    FILE* file = fopen(safe_path, "rb");
    if (!file)
    {
        // Send 404 Not Found
        const char* not_found_response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "Connection: close\r\n"
            "\r\n"
            "404 Not Found\n";
        send_all(client_fd, not_found_response, strlen(not_found_response));
        close(client_fd);
        return;
    }

    // Determine MIME Type
    const char* mime_type = get_mime_type(safe_path);

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* file_content = malloc(file_size);
    if (!file_content)
    {
        fclose(file);
        LOG_ERR("Memory Allocation Failed");
        close(client_fd);
        return;
    }

    ssize_t bytes_read = fread(file_content, 1, file_size, file);
    if (bytes_read != file_size)
    {
        LOG_ERR("Failed to read full file content from %s\n", safe_path);
        free(file_content);
        fclose(file);
        close(client_fd);
        return;
    }
    fclose(file);

    // Build HTTP response header
    char header[MAX_HEADER_SIZE];
    int header_len = snprintf(header, sizeof(header),
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %ld\r\n"
    "Connection: close\r\n"
    "\r\n",
    mime_type, file_size);

    // Send headers fully
    if (send_all(client_fd, header, header_len) == -1)
    {
        LOG_ERR("Failed to send HTTP headers");
        free(file_content);
        close(client_fd);
        return;
    }
    // Send file content fully
    if (send_all(client_fd, file_content, file_size) == -1)
    {
        LOG_ERR("Failed to send HTTP body");
        free(file_content);
        close(client_fd);
        return;
    }

    free(file_content);
    close(client_fd);
}
