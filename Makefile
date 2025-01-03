# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 $(shell sdl2-config --cflags) -D_GNU_SOURCE
LDFLAGS = $(shell sdl2-config --libs) -lm

# Directories
SRC_DIR = src
OBJ_DIR = obj

# Executable
TARGET = chip8

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Make sure the obj directory exists
$(shell mkdir -p $(OBJ_DIR))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
