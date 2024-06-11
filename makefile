CC=gcc
CFLAGS=-Wall -g

# Define the target executable name
TARGET=client

# List all the object files needed for the executable
OBJS=client.o
OBJS+=protocol.o

all: $(TARGET)

# Define how the executable is built from the object files
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile each source file to an object file
client.o: client.c protocol.h
	$(CC) $(CFLAGS) -c client.c
protocol.o: protocol.c protocol.h
	$(CC) $(CFLAGS) -c protocol.c

# Define a clean command to remove compiled files
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
