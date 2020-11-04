#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <sys/select.h>
#include <sys/time.h>
  
#define MAXLINE 1024 
  
// Driver code 
int main(int argc, char *argv[]) { 

  int sockfd; 
  char buffer[MAXLINE];
  int valid = 1;
  int port = 6666; 
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
  inet_aton("127.0.0.1", &servaddr.sin_addr); 

  /*
  //test
  char *syn = "SYN:6667\n";
  sendto(sockfd, (const char *)syn, strlen(syn), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));


  int len;
  int n;
  n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &servaddr, &len);
  buffer[n] = '\0';
  printf("serveur : %s\n", buffer);
  */



  //----------------------3 way handshake connection-----------------------------------
  //ENVOIE SYN
  char *portNouveau;
  char *syn = "SYN:6667\n";//inutile cote client
  sendto(sockfd, (const char *)syn, strlen(syn), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
  printf("SYN envoye\n");
  
  //ATTEND LE SYNACK
  int contb=1;
  while(contb){
    int len, n;
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &servaddr, &len);
    buffer[n] = '\0';
    printf("Message : %s\n",buffer);
    char delim[]=":";
    char *ptc = strtok(buffer,delim);
    char *prt = strtok(NULL,delim);
    if (strcmp(ptc,"SYN-ACK") == 0) {
      //envoye de ACK 
      printf("SYN-ACK reçu\n");
      printf("\n");
      portNouveau = prt;
      char ack[1024] = "ACK:";
      //strcat(ack,prt);
      sendto(sockfd, (const char *)ack, strlen(ack), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
      contb = 0; 
    }
  }
  
  //messages over new port test---------------------------------------------------------
  printf("test:%s\n",portNouveau);
  int newPort = atoi(portNouveau);
  servaddr.sin_port = htons(newPort);
  printf("test:%d\n",newPort);
  char test[1024]="test";  

  struct timeval timeout;
  int selector;
  timeout.tv_sec = 2;
  timeout.tv_usec = 0;
  selector = select(NULL,NULL, NULL,NULL,&timeout);
  sendto(sockfd, (const char *)test, strlen(test), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));


  //Receiving new files!!!!!!!!!!!!!!!!-------------------------------------------
  FILE *outFile;
  outFile = fopen("TP3c.pdf", "a");
  int n=0,len,ackSize=12;
  int chunkSize = 1016;
  int segmentSize = 1024;
  char *rcv_chunk = NULL;
  rcv_chunk = malloc(sizeof(char) * (chunkSize + 1));
  char *sgmnt = NULL;
  sgmnt = malloc(sizeof(char) * (segmentSize + 1));
  char header[8];
  int nbseq=0;
  char *ack;
  ack = malloc(sizeof(char) * (12 + 1));
  while(1){

    //receive file
    n = recvfrom(sockfd, (char *)sgmnt, segmentSize, MSG_WAITALL, ( struct sockaddr *) &servaddr, &len);
    sgmnt[n] = '\0';

    //parse file : first 8 is header, rest is data
    memcpy(header,sgmnt,8);
    header[8]='\0';
    rcv_chunk = sgmnt + 8;
    nbseq = atoi(header);
    printf("received : %s\n",header);

    //appaend in file
    fwrite(rcv_chunk,sizeof(char),chunkSize,outFile);//toCPy

    //ACK each file
    memcpy(ack,"ACK_00000000",ackSize);
    memcpy(ack+4,header,8);
    //WAIT
    struct timeval timeot;
    int selectr;
    timeot.tv_sec = 0;
    timeot.tv_usec = 90;
    selectr = select(NULL,NULL, NULL,NULL,&timeot);
    sendto(sockfd, (const char *)ack, ackSize, MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    printf("sending : %s\n",ack);
    

    //to delete
    memset(ack, '\0',ackSize);
    memset(rcv_chunk, '\0', chunkSize);
    memset(sgmnt, '\0', segmentSize);
    memset(header,'\0',8);
  }

 
  
  close(outFile);
  close(sockfd);
  return 0;
}