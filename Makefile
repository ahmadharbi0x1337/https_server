# ===============================
# Dynamic Makefile for HTTPS Server
# ===============================

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -std=c11

# Debug/Release flags
DEBUG_FLAGS := -g -O0 -DDEBUG
RELEASE_FLAGS := -O2 -DNDEBUG

# Project directories
SRC_DIR := src
OBJ_DIR := build
BIN_DIR := bin
INCLUDE_DIR := include

# Final binary
TARGET := $(BIN_DIR)/https-server

# Automatically find all .c files
SRC_FILES := $(shell find $(SRC_DIR) -type f -name '*.c')
OBJ_FILES := $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(SRC_FILES:.c=.o))

# Automatically find all header paths (include/ and src/)
HEADER_DIRS := $(shell find $(SRC_DIR) -type d) $(INCLUDE_DIR)
INCLUDE_FLAGS := $(foreach dir, $(HEADER_DIRS), -I$(dir))

# Libraries (OpenSSL + pthread)
LDFLAGS := -lpthread -lssl -lcrypto

# ===============================
# Build Targets
# ===============================

# Default build (release)
.PHONY: all
all: release

# Debug target
.PHONY: debug
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Release target
.PHONY: release
release: CFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)

# Link target
$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ_FILES) -o $@ $(LDFLAGS)
	@echo "✅ Built: $@"

# Compile each .c file to .o file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

# Clean build artifacts
.PHONY: clean
clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "🧹 Cleaned build artifacts."

# Full rebuild
.PHONY: rebuild
rebuild: clean all

