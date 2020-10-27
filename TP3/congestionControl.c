#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#include "congestionControl.h"
#define RCVSIZE 1024

int threeWayHandshake(sockaddr_in clientUDP, int server_descUDP){
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
        char synack[1024] = "SYN-ACK:";
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