#ifndef TLS_H
#define TLS_H

#include <openssl/ssl.h>

void tls_init();

void tls_cleanup();

SSL *tls_wrap_socket(int client_fd);
#endif