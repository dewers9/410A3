#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

// Function to write data received from the server to a buffer
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((char *)userp)[0] = '\0'; // Initializing the buffer
    strncat(userp, contents, size * nmemb);
    return size * nmemb;
}

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

int main(void) {
    CURL *curl;
    CURLcode res;
    char buffer[128]; // Buffer for the HTTP response

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://yourserver.com/time"); // Your server URL
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)buffer);
        
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("Time received: %s\n", buffer);
            send_to_arduino("/dev/ttyUSB0", buffer); // Replace with your Arduino serial port
        }

        curl_easy_cleanup(curl);
    }
    return 0;
}
