# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -Ikeywords -I.
LDFLAGS =

# Output binary
TARGET = parser 

# Automatically find all source files and generate object files list
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

# Default target: build the program with optimizations
all: CFLAGS += -O2
all: $(TARGET)

# Debug target: build the program with debugging symbols and logging enabled
debug: CFLAGS += -g -DLOGGING_ENABLED
debug: $(TARGET)

# Clean target: remove build artifacts
clean:
	@rm -f $(TARGET) $(OBJS)

# Rule to build the target
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Rule to compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Phony targets
.PHONY: all debug clean
