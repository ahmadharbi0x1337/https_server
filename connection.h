// connection.h
#ifndef CONNECTION_H
#define CONNECTION_H

#include <netinet/in.h>  // For sockaddr_in

// Accept a client connection and fill client address info.
// Returns client socket fd on success, or -1 on failure.
int accept_client(int server_sockfd, struct sockaddr_in *client_addr);

// Close client and/or server sockets gracefully.
void cleanup_connection(int server_sockfd, int client_sockfd);

// Handle a raw TCP connection (simple echo server).
void handle_raw_connection(int client_fd);

// Handle a simple HTTP request and response.
void handle_http_connection(int client_fd);

#endif // CONNECTION_H
