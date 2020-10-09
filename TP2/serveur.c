#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h> 

#define RCVSIZE 1024

int main (int argc, char *argv[]) {

  if(argc != 3) {
    perror("2 arguments wanted\n");
    return -1;
  }
  
  //pour le TCP
  struct sockaddr_in adresseTCP, clientTCP;
  int portTCP= atoi(argv[1]);
  int validTCP= 1;
  socklen_t alenTCP= sizeof(clientTCP);
  char bufferTCP[RCVSIZE];

  //pour le UDP
  struct sockaddr_in adresseUDP, clientUDP;
  int portUDP= atoi(argv[2]);
  int validUDP = 1;
  socklen_t alenUDP = sizeof(clientUDP);
  char bufferUDP[RCVSIZE];

  //create socket
  int server_descTCP = socket(AF_INET, SOCK_STREAM, 0);
  int server_descUDP = socket(AF_INET, SOCK_DGRAM, 0);

  //handle error
  if (server_descTCP < 0 || server_descUDP < 0) {
    perror("Cannot create socket\n");
    return -1;
  }

  //TCP socket param
  setsockopt(server_descTCP, SOL_SOCKET, SO_REUSEADDR, &validTCP, sizeof(int));
  adresseTCP.sin_family= AF_INET;
  adresseTCP.sin_port= htons(portTCP);
  adresseTCP.sin_addr.s_addr= htonl(INADDR_ANY);

  //UDP socket param
  setsockopt(server_descUDP, SOL_SOCKET, SO_REUSEADDR, &validUDP, sizeof(int));
  adresseUDP.sin_family= AF_INET;
  adresseUDP.sin_port= htons(portUDP);
  adresseUDP.sin_addr.s_addr= htonl(INADDR_ANY);


  //initialize socket
  if (bind(server_descTCP, (struct sockaddr*) &adresseTCP, sizeof(adresseTCP)) == -1) {
    perror("Bind failed TCP\n");
    close(server_descTCP);
    return -1;
  }
  if (bind(server_descUDP, (struct sockaddr*) &adresseUDP, sizeof(adresseUDP)) == -1) {
    perror("Bind failed UDP\n");
    close(server_descUDP);
    return -1;
  }

  //listen to incoming clients for TCP
  if (listen(server_descTCP, 0) < 0) {
    printf("Listen TCP failed\n");
    return -1;
  }
  printf("Listen TCP done\n");


  //File descriptor set for selector
  fd_set socket_set;
  
  
  int selector;


  while (1) {
    FD_ZERO(&socket_set);
    FD_SET(server_descTCP,&socket_set);
    FD_SET(server_descUDP,&socket_set);
    printf("Accepting\n");
    
    //select
	selector = select(5,&socket_set, NULL,NULL,NULL);	
	if (selector < 0) {
		perror("Select failed\n");
		return -1;
	}


    //teste si il y a écriture dans la socjet TCP:
    if(FD_ISSET(server_descTCP, &socket_set)){
        //accept TCP Client
        int client_descTCP = accept(server_descTCP, (struct sockaddr*)&clientTCP, &alenTCP);

        /*Ouvre un processus pour la nouvelle écoute*/
        int pid = fork();
        if(pid==0)/*the child process*/
        {
            close(server_descTCP);//fils n'a pas besoin du socket
            printf("Value of child fork is:%d\n", pid);
            printf("Value of accept is:%d\n", client_descTCP);
            printf("Value of socket is:%d\n", server_descTCP);
            int msgSize = read(client_descTCP,bufferTCP,RCVSIZE);

            while (msgSize > 0) {
                write(client_descTCP,bufferTCP,msgSize);
                printf("%s",bufferTCP);
                memset(bufferTCP,0,RCVSIZE);
                msgSize = read(client_descTCP,bufferTCP,RCVSIZE);
            }
            exit(0);

        }else{
            printf("Value of daddy fork is:%d\n", pid);
            close(client_descTCP);
        }

    }

	//UDP
	if(FD_ISSET(server_descUDP,&socket_set)){
        int len, n; 
        n = recvfrom(server_descUDP, (char *)bufferUDP, RCVSIZE, MSG_WAITALL, ( struct sockaddr *) &clientUDP, &len); 
        bufferUDP[n] = '\0'; 
        printf("Client UDP says: %s\n", bufferUDP); 
	}
   
    
    
  }

  close(server_descTCP);
  close(server_descUDP);
  return 0;
}
