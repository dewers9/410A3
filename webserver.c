#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include "apue.h"
#include <errno.h>
#include <syslog.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <time.h>

// creating a webserver in C
// that connects to a client browser

// initialize a socket endpoint
// for use by a server

#define BUFFER_SIZE 1024

int get_local_time(void){
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf ( "Current local time and date: %s", asctime (timeinfo) );
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 0){
        perror("No Port Specified");
        exit(EXIT_FAILURE);
    }

    printf("Web Server Started\n");
    // get fd and make a new socket
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char *response = "sending response";

    // creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    char* a = argv[1];
    int port = atoi(a);

    printf("port: %d\n", port);
    if (port < 5000 || port > 65536){
        perror("Invalid Port Number");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind the socket to localhost and the specified port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // APUE pg. 608
    // willing to accept connect requests by calling the listen function.
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connections
    while (1) {
        // APUE pg. 608:
        // We use the accept function to retrieve a connect request and convert it into a connection.
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
