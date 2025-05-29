// server.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>             // close()
#include <sys/socket.h>         // socket(), bind(), listen(), accept()
#include <netinet/in.h>         // sockaddr_in
#include <arpa/inet.h>          // inet_ntoa()

#include "logger.h"

// Internal helper: create socket and return fd
static int create_socket()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        LOG_ERR("Failed to create socket");
        exit(EXIT_FAILURE);
    }
    LOG_INFO("Socket created");
    return sockfd;
}

// Internal helper: bind socket to address
static void bind_socket(int sockfd, struct sockaddr_in *addr)
{
    if (bind(sockfd, (struct sockaddr *)addr, sizeof(*addr)) < 0)
    {
        LOG_ERR("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    LOG_INFO("Socket bound to port %d", ntohs(addr->sin_port));
}

// Internal helper: start listening on socket
static void start_listening(int sockfd)
{
    if (listen(sockfd, 5) < 0)
    {
        LOG_ERR("Listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    LOG_INFO("Listening for incoming connections...");
}

// Public function to run the server and handle clients with the provided callback
void run_server(int port, void (*handle_client)(int))
{
    int sockfd = create_socket();

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    bind_socket(sockfd, &server_addr);
    start_listening(sockfd);

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
        {
            LOG_ERR("Accept failed");
            continue;  // Could consider whether to exit here based on your needs
        }

        LOG_INFO("Connection accepted from %s:%d",
                 inet_ntoa(client_addr.sin_addr),
                 ntohs(client_addr.sin_port));

        // Delegate handling of client socket to provided callback
        handle_client(client_fd);

        close(client_fd);
        LOG_INFO("Connection closed");
    }

    close(sockfd);
}
