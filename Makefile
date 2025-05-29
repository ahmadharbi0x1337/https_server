CC = gcc
CFLAGS = -Wall -Wextra -g
SRCS = main.c server.c connection.c mime_types.c
OBJS = $(SRCS:.c=.o)
TARGET = http-server

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
