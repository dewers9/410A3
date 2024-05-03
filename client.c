#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <termios.h>

#define SERVER_IP "127.0.0.1"  // Server IP address
#define SERVER_PORT 5050        // Server port number as per your server configuration
#define BUFFER_SIZE 1024        // Size of the buffer for incoming data

// Function to send data to Arduino via serial port
int send_to_arduino(const char *portname, const char *data) {
    int fd;
    struct termios tty;

    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("Error opening serial port");
        return -1;
    }

    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        perror("Error from tcgetattr");
        close(fd);
        return -1;
    }

    cfsetospeed(&tty, B9600); // Set baud rate
    cfsetispeed(&tty, B9600);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    tty.c_iflag &= ~IGNBRK; // Disable break processing
    tty.c_lflag = 0; // No signaling chars, no echo
    tty.c_oflag = 0; // No remapping, no delays

    tty.c_cc[VMIN]  = 0; // Read doesn't block
    tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD); // Ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD); // Shut off parity
    tty.c_cflag |= 0;
    tty.c_cflag &= ~CSTOPB;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("Error from tcsetattr");
        close(fd);
        return -1;
    }

    write(fd, data, strlen(data)); // Send data

    close(fd);
    return 0;
}

int main() {
    printf("Starting client\n");
    int sockfd, n;
    struct sockaddr_in serv_addr;
    char sendbuffer[BUFFER_SIZE];
    char recvbuffer[BUFFER_SIZE];
    int current_timezone = 0;  // Default timezone offset
    char input;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    else{
        printf("Socket opened");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serv_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        fprintf(stderr, "ERROR invalid server IP address\n");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    printf("Use '+' to request time for next timezone, '-' for previous timezone.\n");

    while(1) {
        printf("Enter command (+/-): ");
        input = getchar();  // Read a single character
        getchar();  // Consume the newline character

        if (input == '+') {
            current_timezone++;
        } else if (input == '-') {
            current_timezone--;
        } else {
            printf("Invalid input. Use '+' or '-'.\n");
            continue;
        }

        // Format the HTTP GET request with the current timezone
        snprintf(sendbuffer, sizeof(sendbuffer), "GET /time?zone=%d HTTP/1.1\r\nHost: %s\r\n\r\n", current_timezone, SERVER_IP);

        // Send the GET request
        if (write(sockfd, sendbuffer, strlen(sendbuffer)) < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }

        // Read the server's response
        memset(recvbuffer, 0, BUFFER_SIZE);
        if (read(sockfd, recvbuffer, BUFFER_SIZE - 1) < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }

        // Print the server's response
        printf("Server response: %s\n", recvbuffer);

    // Optionally, send data to Arduino
    //send_to_arduino("/dev/ttyUSB0", "Data to send to Arduino"); // Update this with actual data and port

    // Close the socket
    close(sockfd);
    return 0;
    }
}