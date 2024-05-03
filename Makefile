CC=gcc
CFLAGS=-Wall -g
LIBS=-lcurl  # Add additional libraries needed, for example, -lm for math library if required

# Default target
all: webserver client

# Compile the webserver executable
webserver: webserver.o
	$(CC) $(CFLAGS) -o webserver webserver.o $(LIBS)

# Compile the client executable
client: client.o
	$(CC) $(CFLAGS) -o client client.o $(LIBS)

# Generic rule for compiling any source file to an object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove all compiled files
clean:
	rm -f *.o webserver client
