#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

// Info log macro (prints to stdout)
#define LOG_INFO(fmt, ...) \
    fprintf(stdout, "[INFO] " fmt "\n", ##__VA_ARGS__)

// Error log macro (prints to stderr)
#define LOG_ERR(fmt, ...) \
    fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)

#endif
