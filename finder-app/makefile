# --- Compiler & Cross-Compiler Configuration ---
# If CROSS_COMPILE is provided on the command line (e.g., CROSS_COMPILE=aarch64-none-linux-gnu-),
# it will prepend to gcc. Otherwise, it defaults to the host machine's native gcc.
CC = $(CROSS_COMPILE)gcc

# Compilation flags (Compiler warnings and optimization level)
CFLAGS = -Wall -Wextra -O2

# --- Application Name ---
TARGET = writer

# --- Source File Management ---
# Automatically find all .c files in the current directory
SRCS = $(wildcard *.c)
# Derive object file names (.o) from source file names (.c)
OBJS = $(SRCS:.c=.o)

# --- Default Target ---
# This executes automatically when you run just 'make'
all: $(TARGET)

# Rule to link the final executable binary from object files
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- Clean Target ---
# Removes the compiled binary and all generated .o object files
clean:
	rm -f $(TARGET) $(OBJS)

# Declare phony targets to prevent naming conflicts with local files
.PHONY: all clean
