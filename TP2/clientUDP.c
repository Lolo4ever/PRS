#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
  
#define MAXLINE 1024 
  
// Driver code 
int main(int argc, char *argv[]) { 

  if(argc != 3) {
    perror("2 arguments wanted\n");
    return -1;
  }

  int sockfd; 
  char buffer[MAXLINE];
  int valid = 1;
  int port = atoi(argv[1]); 
  struct sockaddr_in servaddr; 
  
  // Creating socket file descriptor 
  if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
	  perror("socket creation failed"); 
	  return -1; 
  } 

  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &valid, sizeof(int));
  
    //memset(&servaddr, 0, sizeof(servaddr)); 
      
  // Filling server information 
  servaddr.sin_family = AF_INET; 
  servaddr.sin_port = htons(port); 
  inet_aton(argv[2], &servaddr.sin_addr); 
  
  //envoyer
  /*
  int cont = 1;
  while (cont){ 
	fgets(buffer, MAXLINE, stdin); 
	int sss= write(sockfd,buffer,strlen(buffer));
	printf("the value of sent is:%d\n", sss); 
	if (strcmp(buffer,"stop\n") == 0) {
      cont= 0; 
    }
  }*/
  char *hello = "Hello from client";
  sendto(sockfd, (const char *)hello, strlen(hello), 
        MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr)); 
  close(sockfd); 
  return 0; 
} 
