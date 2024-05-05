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
#define SERVER_PORT 5050     // Server port number as per your server configuration
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

int main(int argc, char *argv[]) {

    int port = 5050;

    if (argc < 1){
        perror("No Port Specified");
        exit(EXIT_FAILURE);
    }

    char* a = argv[1];
    port = atoi(a);
    
    printf("Starting client\n");
    int sockfd, send =0;
    struct sockaddr_in serv_addr;
    char sendbuffer[BUFFER_SIZE];
    char recvbuffer[BUFFER_SIZE];
    char input[1024];  // Define a buffer to hold the input line
    char time_buf[1024];  // Define a buffer to hold the input line

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        // exit(1);
    }
    else{
        printf("Socket opened\n");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        fprintf(stderr, "ERROR invalid server IP address\n");
        exit(1);
    }

    while(1) {
        
        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR connecting");
            exit(1);
        }

        char fileName[256]; // Buffer to store the filename
        int int_of_code;

        printf("\nEnter timezone Code or command: ");
        if (fgets(input, sizeof(input), stdin)) {  // Read line from stdin
            printf("You entered: %s", input);
        } else {
            printf("Error reading input.\n");
        }
        fflush(stdin);

        // Remove the newline character if present
        input[strcspn(input, "\n")] = 0;


        if (input[0] == 'q'){
            break;
        } else if (strncmp(input, "file", strlen("file")) == 0) {
            // Extract the filename
            // Assuming the format "file filename", we skip the first 5 characters and any subsequent spaces
            sscanf(input + strlen("file"), " %255[^\n]", fileName); // Skip the keyword and any spaces after it
            
            // Output the extracted filename and activate specific control flow
            printf("Filename extracted: '%s'\n", fileName);
            // Here you would add your specific control flow for handling the file
            snprintf(sendbuffer, sizeof(sendbuffer), "GET /request?file=%s HTTP/1.1\r\nHost: %s\r\n\r\n", fileName, SERVER_IP);

        } else if (strncmp(input, "run", strlen("run")) == 0) {
            // Extract the filename
            // Assuming the format "file filename", we skip the first 5 characters and any subsequent spaces
            sscanf(input + strlen("run"), " %255[^\n]", fileName); // Skip the keyword and any spaces after it
            
            // Output the extracted filename and activate specific control flow
            printf("Filename extracted: '%s'\n", fileName);
            // Here you would add your specific control flow for handling the file
            snprintf(sendbuffer, sizeof(sendbuffer), "GET /request?file=%s HTTP/1.1\r\nHost: %s\r\n\r\n", fileName, SERVER_IP);
        } else if (strncmp(input, "ls", strlen("ls")) == 0) {
            snprintf(sendbuffer, sizeof(sendbuffer), "GET /request?ls=TRUE HTTP/1.1\r\nHost: %s\r\n\r\n", SERVER_IP);
        } else {
            int_of_code = atoi(input);
            if (int_of_code < 1 || int_of_code > 25){
                perror("ERROR Invalid Timezone code");
                exit(1);
            }
            // Format the HTTP GET request with the current timezone
            snprintf(sendbuffer, sizeof(sendbuffer), "GET /request?zone=Etc/GMT:%d HTTP/1.1\r\nHost: %s\r\n\r\n", int_of_code - 13, SERVER_IP);

            sprintf(time_buf,"%d:00:00",int_of_code);
            // printf("%d:00:00",int_of_code);
            
            send = 1;

        }

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
        printf("\nServer response: %s\n", recvbuffer);

        if(send){
            if (sscanf(recvbuffer, "%*s %*s %*s %*s %*s %*s %8s", time_buf) == 1) {
                printf("Extracted time: %s\n", time_buf);
            } else {
                printf("Failed to extract time.\n");
            }
            send_to_arduino("/dev/cu.usbmodem1101", time_buf); // Update this with actual data and port
            send = 0;

            char html[2048] = {0};
            sprintf(html,"HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html>\n<head> <style>body {background-color: powderblue;}h1 {color: blue;}p    {color: red;}</style>\n<title>Directory Listing</title>\n</head>\n<body>\n<h1>Directory Listing</p>\n<h5>TIME RECIEVED=%s</p\n</body>\n</html>", time_buf);

            if (write(sockfd, html, strlen(html)) < 0) {
                perror("ERROR writing to socket");
                exit(1);
            } 

            printf("Wrote back to Server!\n\n%s\n",html);
        }

        close(sockfd);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
    }
    // Print the server's response
    printf("Server response: %s\n", recvbuffer);

    // Optionally, send data to Arduino
    //send_to_arduino("/dev/ttyUSB0", "Data to send to Arduino"); // Update this with actual data and port

    // Close the socket
    close(sockfd);
    return 0;
}