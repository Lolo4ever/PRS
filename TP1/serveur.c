#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h> 
#include <unistd.h>
#include <stdlib.h>

int main()
{
    /* Creation de la socket */
	int sock_serv;
	sock_serv=socket(AF_INET,SOCK_STREAM,0);

    int reuse =1;
	setsockopt(sock_serv, SOL_SOCKET, SO_REUSEADDR,&reuse, sizeof(reuse));//liberer le process
     
    if ( sock_serv == -1)//prendre en compte l'erreur si il y en a une
    {
	    perror("Creation de socket impossible"); 
	    return -1; 
    }

    printf("La valeur du descripteur (serveur) est: %d\n", sock_serv);



    /*socket opt*/
    struct sockaddr_in my_addr; //definition structure
	struct sockaddr_in client_addr; //pour plus tard, defini la structure sokcet client
	memset((char*)&my_addr, 0, sizeof(my_addr));//remettre dà 0 la structure

	my_addr.sin_family=AF_INET;
	my_addr.sin_addr.s_addr =  htonl(INADDR_ANY); 
    //my_addr.sin_addr.s_addr =  INADDR_LOOPBACK;  pour la question 2
	my_addr.sin_port=htons(20000);	
	printf("s_addr: %d\n", my_addr.sin_addr.s_addr);




    /*Connecter à la structure*/
	bind(sock_serv,(struct sockaddr*)&my_addr,sizeof(struct sockaddr_in));
    listen(sock_serv, 1);

    //printf(" listen%d\n", listen);
	printf("attente\n");





    /*Traitement*/
    socklen_t sock_serv_len=sizeof(struct sockaddr_in);//initialisé avec la taille de la structure sockaddr vide, qui pointera après l’appel vers la taille réelle des données retournées.
    int socket_service;
    while(1)
    {

        /*attend des demande de conection*/		
		socket_service = accept(sock_serv,(struct sockaddr*)&client_addr,&sock_serv_len);	
		printf("accept: %d\n",socket_service);
        printf("Connexion établie avec client d'adresse IP : %s; Port : %d\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));


        /*reception message*/
		char recvBuff[100];
		memset(recvBuff, '\0', sizeof(recvBuff));
		read(socket_service, recvBuff, sizeof(recvBuff));
        printf( "[Message du Client] : %s || [Taille du message] : %ld\n", recvBuff,sizeof(recvBuff));


		/*Transmettre message*/
		char sendBuff[1024];
    	memset(sendBuff, '\0', sizeof(sendBuff));
		sprintf(sendBuff, "message recu!");
		write(socket_service, sendBuff, strlen(sendBuff));


        close(socket_service);
    }
    close(sock_serv);
}