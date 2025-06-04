CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
SRCS = src/main.c src/server.c src/connection.c src/mime_types.c
OBJS = $(SRCS:.c=.o)
TARGET = http-server

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
