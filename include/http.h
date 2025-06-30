#ifndef HTTTP_H
#define HTTP_H

// Handles a single HTTP connection by parsing the request and sending the appropriate file
void handle_http_connection(int client_fd);


#endif