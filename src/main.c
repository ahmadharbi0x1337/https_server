#include <stdint.h>
#include <stdlib.h>
#include "server.h"
#include "connection.h"   // For handle_http_connection
#include "logger.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        LOG_ERR("Usage: %s <port>", argv[0]);
        return EXIT_FAILURE;
    }

    int port = atoi(argv[1]);
    if (port <= 0 || port > 65535)
    {
        LOG_ERR("Invalid port number. Use a value between 1 and 65535.");
        return EXIT_FAILURE;
    }

    LOG_INFO("Starting server on port %d...", port);
    run_server(port, handle_http_connection);  // Pass handler function here

    return EXIT_SUCCESS;
}



















/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <pthread.h>

#include "mime_types.h"


typedef struct
{
    const char* extension;
    const char* mime_type;
}MimeType;


// How Many Client Can Wait in Line ?
#define BACKLOG 5

// Logging Macros
#define LOG_INFO(fmt, ...) fprintf(stdout, "[INFO] " fmt "\n", ##__VA_ARGS__)
#define LOG_ERR(fmt, ...)  fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)



// Function Declarations
int create_socket();
int bind_socket(int sockfd, struct sockaddr_in *server_address);
int start_listening(int sockfd, int backlog);
int accept_connections(int sockfd, struct sockaddr *client_address, socklen_t *client_addr_length, struct sockaddr_in *client_addr);
void cleanup(int sockfd, int client_fd);
void fatal_error(const char* msg);
void handle_row_connection(int client_fd);
void handle_http_request(int client_fd);
void run_server(int server_port);


// Main Function
int main(int argc, char *argv[])
{
    // Check Command-line Argument
    if (argc != 2)
    {
        fatal_error("Must specify a port number");
        return -1;
    }

    // Validate Port
    int port = atoi(argv[1]);
    if (port < 0 || port > 65535)
    {
        LOG_ERR("Invalid port number. Please use a value between 0 - 65535\n");
        return -2;
    }

    // Run the TCP server
    run_server(port);

    return 0;
}

// Functions Definitions

int create_socket()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        fatal_error("Socket Creation Failed");
    }

    LOG_INFO("Socket created successfully.\nInitializing socket structure...\n");
    return sockfd;
}

int bind_socket(int sockfd, struct sockaddr_in *server_address)
{
    if (bind(sockfd, (struct sockaddr *)server_address, sizeof(*server_address)) < 0)
    {
        close(sockfd);
        fatal_error("Bind Failed");
    }

    LOG_INFO("Socket Binded Successfully.\n");
    return 0;
}

int start_listening(int sockfd, int backlog)
{
    if (listen(sockfd, backlog) == -1)
    {
        
        close(sockfd);
        fatal_error("Listen Failed");
    }

    LOG_INFO("Socket is now listening...\n");
    return 0;
}

int accept_connections(int sockfd, struct sockaddr *client_address, socklen_t *client_addr_length, struct sockaddr_in *client_addr)
{
    int client_fd = accept(sockfd, client_address, client_addr_length);
    if (client_fd == -1)
    {
        perror("Accept Failed");
        return -3;
    }

    LOG_INFO("Connection accepted from %s:%d\n",
    inet_ntoa(client_addr->sin_addr),
    ntohs(client_addr->sin_port));

    return client_fd;
}

void cleanup(int sockfd, int client_fd)
{
    if (client_fd >= 0)
    {
        close(client_fd);
    }
    if (sockfd >= 0)
    {
        close(sockfd);
    }
    LOG_INFO("Cleaned up Sockets, Server Shutting Down ....\n");
}

void fatal_error(const char* msg)
{
    LOG_ERR("%s", msg);
    exit(EXIT_FAILURE);
}

void handle_row_connection(int client_fd)
{
    char buffer[1024];
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer)-1, 0);
        if (bytes_received < 0)
        {
            LOG_ERR("Failed To Receive Data From Client");
            break;
        }
        else if (bytes_received == 0)
        {
            LOG_INFO("Client Disconnected");
            break;
        }
        buffer[bytes_received] = '\0';
        LOG_INFO("Received From CLient %s", buffer);
        if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "exit\n") == 0)
        {
            LOG_INFO("Client Requested To Disconnect");
            break;
        }

        char response[2048];
        snprintf(response, sizeof(response), "\n%s\n", buffer);
        if (send(client_fd, response, strlen(response) ,0) == -1)
        {
            LOG_ERR("Failed To Send Data To Client");
            break;
        }
    }

}

void handle_http_request(int client_fd)
{
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer)-1, 0);
    if (bytes_received <= 0)
    {
        LOG_ERR("Failed To Receive The HTTP Request");
    }

    LOG_INFO("Received HTTPS Request:\n%s", buffer);

    // Simple Parsing 
    if (strncmp(buffer, "GET / ", 6) == 0)
    {
        const char* http_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 14\r\n"
        "Connection: close\r\n"
        "\r\n"
        "Hello, Browser!\n";
        send(client_fd, http_response, strlen(http_response), 0);
    }
    else
    {
        const char* not_found = 
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "Connection: Close\r\n"
        "\r\n"
        "404 Not Found\n";
        send(client_fd, not_found, strlen(not_found), 0);
    }
}
void run_server(int server_port)
{
    int sockfd = create_socket();

    // Initialize socket structure
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    // Configure socket address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server_port);

    // Bind and listen
    int bind_results = bind_socket(sockfd, &server_address);
    int listen_result = start_listening(sockfd, BACKLOG);

    // Accept connection
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept_connections(sockfd, (struct sockaddr *)&client_addr, &client_len, &client_addr);
    if (client_fd >= 0)
    {
        handle_http_request(client_fd);
        close(client_fd);
    }
    // Cleanup and close sockets
    // Modified To Only Close Server Socket Now
    cleanup(sockfd, -1);
}

*/