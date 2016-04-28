

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h> 
#include <signal.h>
#include <termios.h>
#include <time.h>
#include <fcntl.h>
#include "linked_list.h"

int flag = 0; //stop flag
struct_of_ints* head = NULL;
struct_of_ints* tail = NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; //make sure its not modified and read at same time
pthread_mutex_t listlock = PTHREAD_MUTEX_INITIALIZER;

double max, min, avg, total = 0;
int count = 0;

int ard = -1;

time_t start, stop;

int sensor = 1;

int fahrenheit = 0, standby = 0;

double tempF(double celsius) {
  return celsius * 1.8 + 32;
}

int update_stats(double value) {
  if (value > 40) { //irregular value, discard
    return 0;
  }
  count++;
  pthread_mutex_lock(&lock);
  if (count == 1) { //initialize for first value
    max = value;
    min = value;
    avg = value;
    total = value;
  } else {
    if (value > max) {
      max = value;
    } else if (value < min) {
      min = value;
    }
    total = total + value;
    avg = total / count;
  }
  pthread_mutex_unlock(&lock);
  return 0;
}

struct_of_ints* clean_up(struct_of_ints* tail_node) {
  //iterate from end until time is within an hour
  //then delete end of list that's expired
  time(&stop);
  struct_of_ints* curr = tail_node;
  double cutoff = difftime(stop, start) - 3600;
  while (curr->time < cutoff) {
    //while time is not more than an hour ago
    curr = curr->prev;
  }
  tail_node = curr;
  curr = curr->next;

  // temp holds node to be freed
  struct_of_ints* temp;
  // loop while not not at end
  //no lock needed because information at end of list will not conflict with other operations
  while (curr) {
    temp = curr;
      // update curr as the previous one will be deleted
    curr = curr->next;
    total -= curr->value;
    count--;
      // free!
    free(temp);
  }

  //update tail_node's next
  tail_node->next = NULL;
  //recalculate avg
  avg = total / count;

  return tail_node;
}


int start_server(int PORT_NUMBER)
{
      // structs to represent the server and client
  struct sockaddr_in server_addr,client_addr;    

  int sock; // socket descriptor

  // 1. socket: creates a socket descriptor that you later use to make other system calls
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Socket");
    exit(1);
  }
  int temp;
  if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&temp,sizeof(int)) == -1) {
    perror("Setsockopt");
    exit(1);
  }

  // configure the server
  server_addr.sin_port = htons(PORT_NUMBER); // specify port number
  server_addr.sin_family = AF_INET;         
  server_addr.sin_addr.s_addr = INADDR_ANY; 
  bzero(&(server_addr.sin_zero),8); 
  
  // 2. bind: use the socket and associate it with the port number
  if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
    perror("Unable to bind");
    exit(1);
  }

  // 3. listen: indicates that we want to listen to the port to which we bound; second arg is number of allowed connections
  if (listen(sock, 1) == -1) {
    perror("Listen");
    exit(1);
  }

  // once you get here, the server is set up and about to start listening
  printf("\nServer configured to listen on port %d\n", PORT_NUMBER);
  fflush(stdout);


  // 4. accept: wait here until we get a connection on that port
  int sin_size = sizeof(struct sockaddr_in);

  while(1) { //continuous loop
    printf("Getting new request...\n"); // print for debugging 
    pthread_mutex_lock(&lock);
    if (flag == 1) { //stop flagged, break
      printf("Stopping program\n"); // print for debugging 
      pthread_mutex_unlock(&lock);
      break;
    }
    pthread_mutex_unlock(&lock);

    int fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
    if (fd != -1) {
      printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

      // buffer to read data into
      char request[1024];
      
      // 5. recv: read incoming message into buffer
      int bytes_received = recv(fd,request,1024,0);
      // null-terminate the string
      request[bytes_received] = '\0';
      
      printf("Here comes the message:\n");
      printf("%s\n", request);

      char* token = strtok(request, "/ ");
      printf("%s\n", token);

      token = strtok(NULL, ",/ ");
      printf("%s\n", token);

      printf("Done tokenizing\n");
      char* reply = malloc(sizeof(char) * 1000);

      if (standby == 0) {
        if (strcmp(token, "party") == 0) { //turn on party mode
          write(ard, "1\n", 2); //tell ard to party
          strcpy(reply, "{\n\"party\": \"");
          strcat(reply, "Party time!");
          strcat(reply, "\"\n}\n");
        } 
        else if (strcmp(token, "stats") == 0) { //get stats
          double avg1, min1, max1;
          pthread_mutex_lock(&lock); //lock before reading
          if (fahrenheit == 1) { //is it in F mode
            avg1 = tempF(avg);
            min1 = tempF(min);
            max1 = tempF(max);
          } else {
            avg1 = avg;
            min1 = min;
            max1 = max;
          }
          pthread_mutex_unlock(&lock);
          //write stats in reply
          strcpy(reply, "{\n\"avg\": \"");
          char buf[12], buf2[12], buf3[12];
          sprintf(buf, "%f", avg1);
          strcat(reply, buf);
          strcat(reply, "\"\n,");
          strcat(reply, "\n\"max\": \"");
          sprintf(buf2, "%f", max1);
          strcat(reply, buf2);
          strcat(reply, "\"\n,");
          strcat(reply, "\n\"min\": \"");
          sprintf(buf3, "%f", min1);
          strcat(reply, buf3);
          strcat(reply, "\"\n}\n");
        } 
        else if (strcmp(token, "phila") == 0) { //compare philly temp
          printf("Temperature in Philly is: ");
          token = strtok(NULL, ",/ ");
          double phila = atof(token);
          printf("%f \n", phila);

          pthread_mutex_lock(&listlock); //lock before reading
          if (head != NULL) {
            if (phila < head->value) {
              printf("It's cooler outside \n");  
              write(ard, "2\n", 2); //tell ard to flash blue
            } else {
              printf("It's warmer outside \n");  
              write(ard, "3\n", 2); //tell ard to flash red
            }
          }
          pthread_mutex_unlock(&listlock);

          strcpy(reply, "{\n\"phila\": \""); //return philly temp
          if (fahrenheit == 1) {
            char buf[12];
            sprintf(buf, "%f", tempF(phila));
            strcat(reply, buf);
          } else {
            strcat(reply, token);
          }
          strcat(reply, "\"\n}\n");
        }
        else if (strcmp(token, "temp") == 0) { //get current temp
          strcat(reply, "{\n\"temp\": \"");
          pthread_mutex_lock(&listlock);
          if (head != NULL) {
            char buf[12];
            if (fahrenheit == 1) { //if F mode
              sprintf(buf, "%f", tempF(head->value));
            } else {
              sprintf(buf, "%f", head->value);
            }
            strcat(reply, buf);
          } else {
            strcat(reply, "No data!");
          }
          pthread_mutex_unlock(&listlock);
          strcat(reply, "\"\n}\n");
        }
        else if (strcmp(token, "mode") == 0) { //change temp mode     
          write(ard, "4\n", 2); //tell ard to change mode
          if (fahrenheit == 0) {
            fahrenheit = 1;
          } else {
            fahrenheit = 0;
          }
          strcpy(reply, "{\n\"mode\": \""); //tell pebble mode
          if (fahrenheit == 1) {
            strcat(reply, "The mode is Fahrenheit");
          } else {
            strcat(reply, "The mode is Celsius");
          }
          strcat(reply, "\"\n}\n");
        }
      }
      if (strcmp(token, "standby") == 0) { //toggle standby
        write(ard, "5\n", 2); //tell ard to change standby
        if (standby == 0) {
          standby = 1;
        } else {
          standby = 0;
        }
        strcpy(reply, "{\n\"standby\": \""); //tell pebble
        if (standby == 0) {
          strcat(reply, "Resumed!");
        } else {
          strcat(reply, "Standing by...");
        }
        strcat(reply, "\"\n}\n");
      }       

      if (sensor == 0) {
        free(reply);
        char* reply = malloc(sizeof(char) * 1000);
        strcpy(reply, "{\n\"error\": \""); //tell pebble
        strcat(reply, "Arduino is not connected!");
        strcat(reply, "\"\n}\n");
      }

      send(fd, reply, strlen(reply), 0);
      printf("Server sent message: %s\n", reply); // print for debugging 
      free(reply);
    }

    // 7. close: close the connection
    close(fd);
    printf("Server closed connection\n");
    
  }//what to do with this man

  // 8. close: close the socket
  delete_list(head);
  close(sock);
  printf("Server shutting down\n");
  
  return 0;
} 

//check for input and stop if 'q' is entered
void* input(void* p) {
  printf("%s\n", "Please provide input: ");
  char input = ' ';
  while (input != 'q') { //flag stop once q is entered
    scanf("%c", &input);
  }
  pthread_mutex_lock(&lock);
  flag = 1;
  pthread_mutex_unlock(&lock);
  pthread_exit(NULL);
}

//read from arduino
void* serial(void* p) {
  char buffer[100];
  ard = open("/dev/cu.usbmodem1411", O_RDWR);
  int bytes_read;
  if (ard == -1) {
    sensor = 0;
    printf("Sensor is not connected!\n");
    return NULL;
  }
  
  struct termios options; // struct to hold options
  tcgetattr(ard, &options);  // associate with this ard
  cfsetispeed(&options, 9600); // set input baud rate
  cfsetospeed(&options, 9600); // set output baud rate
  tcsetattr(ard, TCSANOW, &options); // set options
  
  int current_read = 0;
  int start_string, iter = 0;
  int leftover = 0;
  int copy;

  while(1) {
    bytes_read = read(ard, &(buffer[leftover]), 100 - leftover);
    if (bytes_read == -1) {
      sensor = 0;
    } else {
      sensor = 1;
    }
    
    bytes_read += leftover;
    start_string = 0;
    leftover = 0;
    for (iter = 0; iter < bytes_read; iter++) {
      if (buffer[iter] == '\n') {
        buffer[iter] = '\0';

        time(&stop); //get time
        double value = atof(&(buffer[start_string]));
        pthread_mutex_lock(&listlock);
        head = add_to_top(head, value, difftime(stop, start)); //save temp & time
        pthread_mutex_unlock(&listlock);
        if (tail == NULL) tail = head; //set tail to first node
        tail = clean_up(tail);
        update_stats(value);

        start_string = iter + 1;
      }
    }
    if (start_string != bytes_read) {
      leftover = bytes_read - start_string;
      copy = 0;
      for (iter = start_string; iter < bytes_read; iter++) {
        buffer[copy] = buffer[iter];
        copy++;
      }
    }
    if (flag == 1) {
      break;
    }
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  time(&start);

  // check the number of arguments
  if (argc != 2) {
    printf("\nUsage: %s [port_number] \n", argv[0]);
    exit(-1);
  }

  int port_number = atoi(argv[1]);
  if (port_number <= 1024) {
    printf("\nPlease specify a port number greater than 1024\n");
    exit(-1);
  }

  pthread_t thread2, thread3;
  pthread_create(&thread2, NULL, &input, NULL); //start thread2
  pthread_create(&thread3, NULL, &serial, NULL); //start thread3

  start_server(port_number);
  pthread_join(thread2, NULL); //stop thread2
  pthread_join(thread3, NULL); //stop thread3
  return 0;
}

