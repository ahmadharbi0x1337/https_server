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
#include "http.h"

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