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
#include <sys/types.h>
#include <sys/wait.h>

// creating a webserver in C
// that connects to a client browser

// initialize a socket endpoint
// for use by a server

#define BUFFER_SIZE 1024
//Africa
//America
//Antarctica
//Arctic
//Asia
//Atlantic
//Australia
//Europe
//Indian
//Pacific
char buffer[80];
char * get_local_time(char* timezone){

  setenv("TZ", timezone, 1);
  tzset();
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);


  return asctime(timeinfo);
}

int main(int argc, char *argv[]) {
  if (argc < 0){
    perror("No Port Specified");
    exit(EXIT_FAILURE);
  }

  char* a = argv[1];
  int port = atoi(a);

  //printf("port: %d\n", port);
  if (port < 5000 || port > 65536){
    perror("Invalid Port Number");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in address;

  // AF_INET is a IPv4 Internet domain
  address.sin_family = AF_INET;
  // INADDR_ANY means the socket will be bound to
  // all of the local system's network interfaces
  address.sin_addr.s_addr = INADDR_ANY;
  // h to network; "converts integer port
  // into bytes that will refer to a network port"
  address.sin_port = htons(port);

  // get fd and make a new socket
  int server_fd, new_socket;

  // creating socket file descriptor
  // sockets let our OS communicate with the network
  // SOCK_STREAM: gets one character after then ext
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  //setsockopt(server_fd, SO_REUSEADDR, 1, (socklen_t*)&addrlen)

  // Bind the socket to localhost and the specified port
  // binding is just connect a port to the socket
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Binding to socket failed");
    exit(EXIT_FAILURE);
  }

  // APUE pg. 608
  // wait for connection on specified port
  // 3 potential backlogs
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  char buffer[BUFFER_SIZE];
  char *response_string = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body><h1>Hello, World!</h1></body></html>";
  char *response = malloc(strlen(response_string) + 1);
  char *request = NULL;
  strcpy(response, response_string);

  int addrlen = sizeof(address);
  int status;
  // Accept incoming connections
  while (1) {
    // APUE pg. 608:
    // We use the accept function to retrieve a connect request and convert it into a connection.
    printf("=====Web Server Waiting for Connection=====\n");
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    // Read the incoming request
    read(new_socket, buffer, BUFFER_SIZE);

    pid_t pid = fork();
    if(pid < 0){
      perror("failed to fork");
      exit(1);
    }
    else if (pid == 0){
      //first check if arguments, get those, then get extension
      char *token;
      printf("HTTP Part: %s\n", buffer);
      fflush(stdout);
      char *args = malloc(4096);
      token = strtok(buffer,"\n");
      token += 5;
      token = strtok(token, " ");
      if(strchr(token,'?') != NULL){
        token = strtok(token, "?");
        args = strtok(NULL, " ");
      }
      char* request_new = realloc(request, strlen(token) + 1);
      if (request_new == NULL){
        printf("Memory allocation failed!\n");
        return 1; // Return an error code
      }

      strcpy(request_new, token);
      request = request_new;

      token = strtok(token, ".");
      token = strtok(NULL, ".");



      char *extension = NULL;
      if(token){
        extension = malloc(strlen(token) + 1);
        strcpy(extension, token);
      }
      // Stop further processing
      printf("Request is %s\n", request);
      fflush(stdout);
      printf("Extension %s\n", extension);
      printf("Arguments %s\n", args);
      fflush(stdout);



      //
      //GOT
      //TOKEN
      //SENDING RESPONSES


      FILE *file;
      if (extension){
        printf("Arrive at Extension\n");
        fflush(stdout);
        if (strcmp(extension, "html") == 0){
          file = fopen(request, "r");
          if (file == NULL) {
            perror("Error opening file");
            char *not_found = "<html><body><h1>404 Not Found</h1></body></html>";
            char *http_response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
            send(new_socket, http_response, strlen(http_response), 0);
            send(new_socket , not_found, strlen(not_found), 0);

          }
          //read contents into buffer
          char html_buffer[1024];
          fread(html_buffer, sizeof(char), sizeof(html_buffer), file);
          //send html headers then the content
          char *http_response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
          send(new_socket, http_response, strlen(http_response), 0);
          send(new_socket , html_buffer, strlen(html_buffer), 0);
          fclose(file);
        }else if(strcmp("gif", extension) == 0 || strcmp("jpeg", extension) == 0 || strcmp("jpg", extension) == 0){
          size_t bytes_read;
          char image_buffer[1024];

          file = fopen(request, "rb");

          if (!file) {
            perror("Error opening image file");
            return 1;
          }
          //headers

          char http_response[1024];

          snprintf(http_response, sizeof(http_response), "HTTP/1.1 200 OK\nContent-Type: image/%s\n\n", extension );

          printf("Arrive at Image %s\n", http_response);
          fflush(stdout);
          send(new_socket, http_response, strlen(http_response), 0);

          while ((bytes_read = fread(image_buffer, 1, sizeof(image_buffer), file)) > 0) {\


            send(new_socket, image_buffer, bytes_read, 0);
          }
          fclose(file);
        }else if(strcmp("cgi", extension) == 0){

          int pipefd[2];
          pid_t pid;

          // Create a pipe
          if (pipe(pipefd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
          }
          pid = fork();
          if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
          } else if (pid == 0) {
            // Child process
            close(pipefd[0]);  // Close reading end of pipe
            // Write data to the pipe
            dup2(pipefd[1], STDOUT_FILENO);
            char *args[] = { request, NULL };
            char cur_dir[100] = "./";
            strcat(cur_dir, request);
            char *arg[] = {cur_dir,args, NULL};
            if (execvp(cur_dir, args) == -1) {
              perror("execvp failed");
              return 1;
            }

            close(pipefd[1]);  // Close writing end of pipe
            exit(EXIT_SUCCESS);
          } else {
            // Parent process
            close(pipefd[1]);  // Close writing end of pipe
            char buffer[1024];
            // Read data from the pipe
            read(pipefd[0], buffer, sizeof(buffer));
            printf("Received message from child: %s\n", buffer);
            char *http_response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
            send(new_socket, http_response, strlen(http_response), 0);
            char html[2048] = {0};
            sprintf(html,"<html>\n<head> <style>body {background-color: powderblue;}h1 {color: blue;}p    {color: red;}</style>\n<title>Script Output</title>\n</head>\n<body>\n<h1>Script Output</h1>\n<p>%s</p>\n</body>\n</html>", buffer);
            send(new_socket, buffer, strlen(buffer), 0);
            close(pipefd[0]);  // Close reading end of pipe
          }


        }

      }else{
        if(strcmp("directory", request) == 0){
          printf("Arrive at Directory\n");
          fflush(stdout);
          DIR *directory;
          struct dirent *entry;
          directory = opendir(".");
          if (directory == NULL) {
            perror("Error opening directory");

          }

          // Read directory entries
          char dir_contents[1024];
          while ((entry = readdir(directory)) != NULL) {
            strcat(dir_contents, entry->d_name);
            strcat(dir_contents, "\n");
          }



          char *http_response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
          send(new_socket, http_response, strlen(http_response), 0);
          // Close the directory
          char html[2048] = {0};

          sprintf(html,"<html>\n<head> <style>body {background-color: powderblue;}h1 {color: blue;}p    {color: red;}</style>\n<title>Directory Listing</title>\n</head>\n<body>\n<h1>Directory Listing</p>\n<h5>%s</p\n</body>\n</html>", dir_contents);
          send(new_socket, html, strlen(html), 0);
          closedir(directory);

        }else if(strcmp("request", request) == 0){
          printf("THESE ARE THE ARGUMENTS %s\n",args);
          fflush(stdout);
          char *func = strtok(args, "=");
          char *f_arg = strtok(NULL, " ");
          
          //arduino request
          if (strcmp("zone", func ) ==  0){

            char * current_time = get_local_time(f_arg);
            printf("The Date in %s is %s", f_arg, current_time);
            fflush(stdout);
            file = fopen("raw.txt", "a");
            if (file == NULL) {
              printf("Error opening file!\n");
              return 1;
            }
            send(new_socket,current_time,strlen(current_time),0);
          }
        }
        else if(strcmp("dynamic", request) == 0){
          char buffer[] = "./my-histogram ";
          char destination[1000];
          // get a dynamic request
          
          char *func = strtok(args, "=");
          char *f_arg = strtok(NULL, " ");
          printf("HISTOGRAM FOR THE %s DIRECTORY \n", f_arg);
          fflush(stdout);
          
          strcpy(destination, buffer);
          strcat(buffer, f_arg);
          printf("buffer: %s", buffer);
          // runs entire my-histogram pipeline
          system(buffer);
          //system("my-histogram.cgi");

          /* Now Display the my-histogram.jpg */
          size_t bytes_read;
          char image_buffer[1024];

          file = fopen("my-histogram.jpg", "rb");

          if (!file) {
            perror("Error opening image file");
            return 1;
          }
          //headers

          char http_response[1024];

          snprintf(http_response, sizeof(http_response), "HTTP/1.1 200 OK\nContent-Type: image/%s\n\n", "jpg");

          printf("Arrive at Image %s\n", http_response);
          fflush(stdout);
          send(new_socket, http_response, strlen(http_response), 0);

          while ((bytes_read = fread(image_buffer, 1, sizeof(image_buffer), file)) > 0) {\


            send(new_socket, image_buffer, bytes_read, 0);
          }
          fclose(file);
        }
      }
      exit(0);
    }
    else{ // Parent Process
      waitpid(pid, &status, 0);
    }

    // Send response
    // write(new_socket, response, strlen(response));
    // close(new_socket);

    // Send the response
    //send(new_socket, response, strlen(response), 0);

    close(new_socket);
  }

  return 0;
}