// connection.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // close()
#include <sys/socket.h>     // accept()
#include <arpa/inet.h>      // inet_ntoa()

#include "connection.h"
#include "logger.h"
#include "mime_types.h"

#define BUFFER_SIZE 4096
#define STATIC_ROOT "../static/"

int accept_client(int server_sockfd, struct sockaddr_in *client_addr)
{
    socklen_t client_len = sizeof(*client_addr);

    int client_fd = accept(server_sockfd, (struct sockaddr *)client_addr, &client_len);
    if (client_fd == -1)
    {
        LOG_ERR("Accept failed");
        return -1;
    }

    LOG_INFO("Connection accepted from %s:%d",
             inet_ntoa(client_addr->sin_addr),
             ntohs(client_addr->sin_port));

    return client_fd;
}

void cleanup_connection(int server_sockfd, int client_sockfd)
{
    if (client_sockfd >= 0)
    {
        close(client_sockfd);
        LOG_INFO("Closed client socket");
    }

    if (server_sockfd >= 0)
    {
        close(server_sockfd);
        LOG_INFO("Closed server socket");
    }
}

void handle_raw_connection(int client_fd)
{
    char buffer[BUFFER_SIZE];

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received < 0)
        {
            LOG_ERR("Failed to receive data from client");
            break;
        }
        else if (bytes_received == 0)
        {
            LOG_INFO("Client disconnected");
            break;
        }
        buffer[bytes_received] = '\0';

        LOG_INFO("Received from client: %s", buffer);

        if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "exit\n") == 0)
        {
            LOG_INFO("Client requested to disconnect");
            break;
        }

        char response[BUFFER_SIZE * 2];
        snprintf(response, sizeof(response), "\n%s\n", buffer);

        if (send(client_fd, response, strlen(response), 0) == -1)
        {
            LOG_ERR("Failed to send data to client");
            break;
        }
    }
}

void handle_http_connection(int client_fd)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0)
    {
        LOG_ERR("Failed to receive HTTP request");
        return;
    }

    LOG_INFO("Received HTTP request:\n%s", buffer);

    char method[8], path[256];
    if (sscanf(buffer, "%7s %255s", method, path) != 2)
    {
        LOG_ERR("Malformed HTTP request");
        return;
    }
    // Currently Only Handle GET requests, meaning other than GET, will be Method Not Allowed
    if (strcmp(method, "GET") != 0)
    {
        const char *method_not_allowed_response = 
        "HTTP/1.1 405 Method Not Allowed/r/n"
        "Content-Length: 0/r/n"
        "Connection: close/r/n"
        "/r/n";
        send(client_fd, method_not_allowed_response, strlen(method_not_allowed_response), 0);
    }


    // Serve index.html by default if root requested.
     const char* filename = (strcmp(path, "/") == 0) ? "/../static/index.html" : path + 1;
    // NOTE : path + 1 , this is a pointer arithmetic, effectively removing the leading slash
       // Open the requested file
    FILE* file = fopen(filename, "rb");
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
        send(client_fd, not_found_response, strlen(not_found_response), 0);
        return;
    }

    // Determine MIME Type FROM FILE NAME 
    const char* mime_type = get_mime_type(filename);

    // Get File Size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* file_content = malloc(file_size);
    if (!file_content)
    {
        fclose(file);
        LOG_ERR("Memory Allocation Failed");
        return;
    }

    fread(file_content, 1, file_size, file);
    fclose(file);


    // Build HTTP Response Header
    char header[512];
    int header_len = snprintf(header, sizeof(header),
    "HTTP/1.1 200 OK/r/n"
    "Content-Type: %s/r/n"
    "Content-Length: %ld/r/n"
    "Connecion: close/r/n"
    "/r/n",
    mime_type, file_size);


    // Send Headers
    if (send(client_fd, header, header_len, 0) == -1)
    {
        LOG_ERR("Failed To Send HTTP Headers");
        return;
    }
    // Send File Content
    if (send(client_fd, file_content, file_size, 0) == -1)
    {
        LOG_ERR("Failed To Send HTTP Body");
        return;
    }

    free(file_content);
}
