#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include "apue.h"
#include <errno.h>
#include <syslog.h>
#include <netinet/in.h>
#include <sys/types.h>

// creating a webserver in C
// that connects to a client browser

// initialize a socket endpoint
// for use by a server


#define PORT 8080
#define BUFFER_SIZE 1024

// int
// initserver(int type, const struct sockaddr *addr, socklen_t alen,
// int qlen)
// {
// int fd;
// int err = 0;
// if ((fd = socket(addr->sa_family, type, 0)) < 0)
//     return(-1);
// if (bind(fd, addr, alen) < 0)
//     goto errout;
// if (type == SOCK_STREAM || type == SOCK_SEQPACKET) {
//     if (listen(fd, qlen) < 0)
//         goto errout;
// }
// return(fd);

// errout:
// err = errno;
// close(fd);
// errno = err;
// return(-1);
// }

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char *response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body><h1>Hello, World!</h1></body></html>";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to localhost and the specified port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connections
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Read the incoming request
        read(new_socket, buffer, BUFFER_SIZE);
        printf("%s\n", buffer);

        // Send the response
        send(new_socket, response, strlen(response), 0);
        close(new_socket);
    }

    return 0;
}