// server.h
#ifndef SERVER_H
#define SERVER_H

// Starts the TCP server on the specified port,
// and handles each client connection using the provided callback function.
// The callback receives the client socket file descriptor.
void run_server(int port, void (*handle_client)(int));

#endif // SERVER_H
