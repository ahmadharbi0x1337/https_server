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

#define BACKLOG 5

// Function Declarations
int create_socket();
int bind_socket(int sockfd, struct sockaddr_in *server_address);
int listen_e(int sockfd, int backlog);
int accept_connections(int sockfd, struct sockaddr *client_address, socklen_t *client_addr_length, struct sockaddr_in *client_addr);
void cleanup(int sockfd, int new_sockfd);
void run_server(int server_port);

// Main Function
int main(int argc, char *argv[])
{
    // Check Command-line Argument
    if (argc != 2)
    {
        perror("Must specify a port number");
        return -1;
    }

    // Validate Port
    int port = atoi(argv[1]);
    if (port < 0 || port > 65535)
    {
        printf("Invalid port number. Please use a value between 0 - 65535\n");
        return -2;
    }

    // Run the TCP server
    run_server(port);

    return 0;
}

// Function Definitions

int create_socket()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Socket created successfully.\nInitializing socket structure...\n");
    return sockfd;
}

int bind_socket(int sockfd, struct sockaddr_in *server_address)
{
    if (bind(sockfd, (struct sockaddr *)server_address, sizeof(*server_address)) < 0)
    {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Socket binded successfully.\n");
    return 0;
}

int listen_e(int sockfd, int backlog)
{
    if (listen(sockfd, backlog) == -1)
    {
        perror("Listen failed");
        close(sockfd);
        return -5;
    }

    printf("Socket is now listening...\n");
    return 0;
}

int accept_connections(int sockfd, struct sockaddr *client_address, socklen_t *client_addr_length, struct sockaddr_in *client_addr)
{
    int new_sockfd = accept(sockfd, client_address, client_addr_length);
    if (new_sockfd == -1)
    {
        perror("Accept failed");
        return -6;
    }

    printf("Connection accepted from %s:%d\n",
           inet_ntoa(client_addr->sin_addr),
           ntohs(client_addr->sin_port));

    return new_sockfd;
}

void cleanup(int sockfd, int new_sockfd)
{
    // Placeholder cleanup logic (currently empty)
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
    bind_socket(sockfd, &server_address);
    listen_e(sockfd, BACKLOG);

    // Accept connection
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int new_sockfd = accept_connections(sockfd, (struct sockaddr *)&client_addr, &client_len, &client_addr);

    // Cleanup and close sockets
    close(new_sockfd);
    close(sockfd);
}

