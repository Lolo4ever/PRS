#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>

#include "congestionControl.h"

#define RCVSIZE 1024

int main (int argc, char *argv[]) {

  //Pour le UDP
  struct sockaddr_in adresseUDP, clientUDP;
  int portUDP= 6666;
  int validU =1;
  socklen_t alenUDP = sizeof(clientUDP);

  //create socket
  int server_descUDP = socket(AF_INET, SOCK_DGRAM, 0);//UDP

  //handle error
  if (server_descUDP < 0 ) {
    perror("Cannot create socket\n");
    return -1;
  }

  //UDP---
  setsockopt(server_descUDP, SOL_SOCKET, SO_REUSEADDR, &validU, sizeof(int));
  memset(&clientUDP, 0, sizeof(clientUDP));  

  //memset((char*)&adresseUDP, 0, sizeof(adresseUDP));  
  adresseUDP.sin_family= AF_INET;
  adresseUDP.sin_port= htons(portUDP);
  adresseUDP.sin_addr.s_addr= htonl(INADDR_ANY);


  //initialize socket
  if (bind(server_descUDP, (struct sockaddr*) &adresseUDP, sizeof(adresseUDP)) == -1) {
    perror("Bind failed UDP\n");
    close(server_descUDP);
    return -1;
  }

  /*-------------------- HANDLE CONNECTION---------------------------------------------*/
  printf("Socket working fine, waiting for connection\n");
  int err = threeWayHandshake(clientUDP,server_descUDP); //handle error
  

  /*-------------------- CONNECTION ESTABLISHED, SET NEW PORT----------------------------------------*/
  printf("Connection succesfull\n");
  printf("\n");
  printf("\n");
  //change socket for connection
  int newPort = 6667;
  int server_messages = socket(AF_INET, SOCK_DGRAM, 0);//UDP
  if (server_descUDP < 0 ) {
    perror("Cannot create socket\n");
    return -1;
  }
  setsockopt(server_messages, SOL_SOCKET, SO_REUSEADDR, &validU, sizeof(int));
  adresseUDP.sin_port= htons(newPort);
  //initialize socket
  if (bind(server_messages, (struct sockaddr*) &adresseUDP, sizeof(adresseUDP)) == -1) {
    perror("Bind failed UDP\n");
    close(server_messages);
    return -1;
  }

  /*char test[RCVSIZE];
  int n;
  int len = sizeof(clientUDP);
  n = recvfrom(server_messages, (char *)test, RCVSIZE, MSG_WAITALL, ( struct sockaddr *) &clientUDP, &len);
  test[n]='\0';
  printf("test:%s\n",test);*/

  /*--------------------------NEW PORT DONE : WAIT CLIENT------------------------------------------------*/
  char buffs[RCVSIZE];
  int n;
  int len = sizeof(clientUDP);
  n = recvfrom(server_messages, (char *)buffs, RCVSIZE, MSG_WAITALL, ( struct sockaddr *) &clientUDP, &len);
  buffs[n]='\0';
  printf("Client sent : %s\n",buffs);


  /*--------------------------CLIENT ANSWERED : SEND DATA------------------------------------------------*/
  //Fragment and send data
  fragmentFile(clientUDP,server_messages);
  
}    










/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------FUNCTIONS----------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/










/*------------------------------------------------ACKs---------------------------------------------------*/
int ack(struct sockaddr_in clientUDP, int server_descUDP, char *buffer){
  int len = sizeof(clientUDP);
  char header[8];
  memcpy(header,buffer,8);
  int sgmntNbr = atoi(header);
  char sgmntNbrChar[4];
  sprintf(sgmntNbrChar, "%d", sgmntNbr);
  printf("header : %s\n",header);
  printf("Gegment number : %d\n",sgmntNbr);

  char *ack;
  ack = malloc(sizeof(char) * (12 + 1));
  memcpy(ack,"ACK_00000000",12);
  if(sgmntNbr<10){
    memcpy(ack+4,sgmntNbrChar,4);
  }else if(sgmntNbr<100){
    memcpy(ack+4,sgmntNbrChar,4);
  }
  printf("sending : %s\n",ack);
  sendto(server_descUDP, (const char *)ack, strlen(ack), MSG_CONFIRM, (const struct sockaddr *) &clientUDP, len); 

  return(0);
}








/*--------------------------------------SEND AND WAIT FOR ACK---------------------------------------*/
int sendAndWaitForAck(struct sockaddr_in clientUDP, int server_descUDP, char *segment, int segmentSize, int timeoutU){
  
  /*instanciate the variables for sending and receiving*/
  int rcv_size = 12;
  char ack_rcv[rcv_size];
  int n;
  int len = sizeof(clientUDP);

  /*File descriptor set for selector*/
  fd_set socket_set;

  /*instanciate the time structures*/
  struct timeval stop, start, timeout;
  
  /*send segment to client*/
  sendto(server_descUDP, (const char *)segment, segmentSize, MSG_CONFIRM, (const struct sockaddr *) &clientUDP, len); 
  printf("Meesage sent\n");

  /*setup the fd set with the proper file descriptor*/
  FD_ZERO(&socket_set);
  FD_SET(server_descUDP,&socket_set);

  /*initialize the proper timeout time*/
  timeout.tv_sec = 0;
  timeout.tv_usec = timeoutU;

  //while(1){
  /*start timer*/
  gettimeofday(&start, NULL);

  /*wait for segment to arrive or go out because of timeout*/
  if( select(6,&socket_set, NULL,NULL,&timeout) == 0){
    printf("TIME OUT :(\n");
  }else{
    gettimeofday(&stop, NULL);
    n = recvfrom(server_descUDP, (char *)ack_rcv, rcv_size, MSG_WAITALL, ( struct sockaddr *) &clientUDP, &len);
    printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec); 
    ack_rcv[n]='\0';
    printf("ack from client : %s\n",ack_rcv);
  }
    //selector = select(5,&socket_set, NULL,NULL,&timeout); //if return 0 then timed out
    //selector = select(5,&socket_set, NULL,NULL,NULL);

    //teste si il y a écriture dans la socket :
    /*printf("waiting for ack\n");
    if(FD_ISSET(server_descUDP, &socket_set)){
      gettimeofday(&current_time, NULL);
      if( (current_time.tv_sec - start.tv_sec) * 1000000 + current_time.tv_usec - start.tv_usec < 100 ){
        n = recvfrom(server_descUDP, (char *)ack_rcv, rcv_size, MSG_WAITALL, ( struct sockaddr *) &clientUDP, &len);
        gettimeofday(&stop, NULL);
        printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec); 
        ack_rcv[n]='\0';
        printf("ack from client : %s\n",ack_rcv);
        //get out of while
      }else{
        printf("BAAAAAAAAAAD BOYYYYY\n");
      }*/
  //}

  /*set the buffers to 0*/
  memset(ack_rcv, '\0', sizeof(rcv_size));

  return(0);
}








/*--------------------------------3 WAY HANDSHAKE FUNCTION-----------------------------------------------*/
int threeWayHandshake(struct sockaddr_in clientUDP, int server_descUDP){
  char bufferUDP[RCVSIZE];
  int len = sizeof(clientUDP);
  int n;

  /*Receive SYN*/
  n = recvfrom(server_descUDP, (char *)bufferUDP, RCVSIZE, MSG_WAITALL, ( struct sockaddr *) &clientUDP, &len);
  bufferUDP[n] = '\0';
  char delim[]=":";
  char *ptc = strtok(bufferUDP,delim);
  char *prt = strtok(NULL,delim);
  if (strcmp(ptc,"SYN") == 0) { 
    /*send SYN-ACK*/
    char synack[1024] = "SYN-ACK:6667";//mettre en paramettre?
    //strcat(synack,prt);
    sendto(server_descUDP, (const char *)synack, strlen(synack), MSG_CONFIRM, (const struct sockaddr *) &clientUDP, len); 
    printf("SYN_ACK envoye\n");      
    //bug, is not sending syn_ack when strcat() is used
    /*attente de ACK*/
    n = recvfrom(server_descUDP, (char *)bufferUDP, RCVSIZE, MSG_WAITALL, ( struct sockaddr *) &clientUDP, &len);
    bufferUDP[n] = '\0';
    printf("Message : %s\n",bufferUDP);
    char delim[]=":";
    char *ptc2 = strtok(bufferUDP,delim);
    char *prt2 = strtok(NULL,delim);
    if (strcmp(ptc2,"ACK") == 0) {
      printf("ACK ressu\n");
      printf("\n");
    }
  }
  return(0);
}






/*-----------------------------------FRAGMENT FILE---------------------------------------------*/
int fragmentFile(struct sockaddr_in clientUDP, int server_descUDP){
  int len = sizeof(clientUDP);

  FILE *outFile;
  FILE *inFile  = fopen("TP3.pdf", "r");
  outFile = fopen("TP3b.pdf", "a");

  /*Put the file content in "source" array*/
  char *source = NULL;
  if (inFile != NULL) {
    /* Go to the end of the file. */
    if (fseek(inFile, 0L, SEEK_END) == 0) {
        /* Get the size of the file. */
        long bufsize = ftell(inFile);
        if (bufsize == -1) { /* Error */ }

        /* Allocate our buffer to that size. */
        source = malloc(sizeof(char) * (bufsize + 1));

        /* Go back to the start of the file. */
        if (fseek(inFile, 0L, SEEK_SET) != 0) { /* Error */ }

        /* Read the entire file into memory. */
        size_t newLen = fread(source, sizeof(char), bufsize, inFile);

        if ( ferror( inFile ) != 0 ) {
            fputs("Error reading file", stderr);
        } else {
            source[newLen++] = '\0'; /* Just to be safe. */

            /*divise "source" en plusieurs segments avec header*/
            //sizes of buffers
            int segmentSize = 1024;           
            int headerSize = 8;
            int chunkSize = 1016;

            //buffers
            int remain = newLen;
            char *sgmnt = NULL;
            sgmnt = malloc(sizeof(char) * (segmentSize + 1));
            char chunk[chunkSize];
            char header[headerSize];

            //for receiver
            char *rcv_chunk = NULL;
            rcv_chunk = malloc(sizeof(char) * (chunkSize + 1));
            
            //to count the packets
            int i = 0;

            /*begin semgmentation*/
            while(remain){

              /*extract a chunk of the data to send*/
              int toCpy = remain > chunkSize ? chunkSize : remain; //nb of bytes to copy
              memcpy(chunk,source,toCpy);
              source += toCpy; //take out from source
              remain -= toCpy;
          
              /*set the segment ID in the header*/
              i++;
              sprintf(header, "%d", i);
              memcpy(sgmnt,"00000000",headerSize);
              if(i<10){
                memcpy(sgmnt+7,header,headerSize);
              }else if(i<100){
                memcpy(sgmnt+6,header,headerSize);
              }else if(i<1000){
                memcpy(sgmnt+5,header,headerSize);
              }
              

              /*Put the chunk to send in the segment*/
              memcpy(sgmnt+8,chunk,sizeof(chunk));

              /*Send messages to client and wait for ack*/
              sendAndWaitForAck(clientUDP,server_descUDP,sgmnt,segmentSize,10);
              //sendto(server_descUDP, (const char *)sgmnt, segmentSize, MSG_CONFIRM, (const struct sockaddr *) &clientUDP, len); 


              /*TO WAIT*/
              /*struct timeval timeout;
              int selector;
              timeout.tv_sec = 0;
              timeout.tv_usec = 500000;
              selector = select(NULL,NULL, NULL,NULL,&timeout);*/


              /*Reset the buffers*/
              memset(sgmnt, '\0', sizeof(sgmnt));
              memset(chunk, '\0', sizeof(chunk));
              memset(header, '\0', sizeof(header));
        
            
              //RECEIVER
              rcv_chunk = sgmnt + 8;
              //appaend in file
              fwrite(rcv_chunk,sizeof(char),chunkSize,outFile);//toCPy
              //to delete
              memset(rcv_chunk, '\0', sizeof(rcv_chunk));
              
            }
            printf("Finisehd copiing\n");
        }
    }
    fclose(inFile);
    //free(source);
    fclose(outFile);
  }

  return(0);
}

//1024 *100 = 100kb
//test
  /*
  int len = sizeof(clientUDP);
  int n;
  while(1){
    n = recvfrom(server_descUDP, (char *)bufferUDP, RCVSIZE, MSG_WAITALL, ( struct sockaddr *) &clientUDP, &len);
    bufferUDP[n] = '\0'; 
    printf("Client : %s\n", bufferUDP);

  	char *synack = "SYN-ACK\n";
  	sendto(server_descUDP, (const char *)synack, strlen(synack), MSG_CONFIRM, (const struct sockaddr *) &clientUDP, len);
  }*/