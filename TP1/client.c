#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    /* Creation de la socket */
	int sock_client;
	sock_client=socket(AF_INET,SOCK_STREAM,0);//Parametres : Domaine(IPv4 ou IPv6...) , type (TCP,UDP...), protocle(spécifique, normalement 0) 

    int reuse =1;
	setsockopt(sock_client, SOL_SOCKET, SO_REUSEADDR,&reuse, sizeof(reuse));//pour liberer le process
     
    if ( sock_client == -1)//prendre en compte l'erreur
    {
	    perror("Creation de socket impossible"); 
	    return -1; 
    } 

    printf("La valeur du descripteur (client) est: %d\n", sock_client);




    /*socket opt*/
	struct sockaddr_in client_addr;//definition structure
	memset((char*)&client_addr, 0, sizeof(client_addr)); //remettre à 0 la structure

	int test;//pour l'affichage

	client_addr.sin_family=AF_INET;
	client_addr.sin_port=htons(20000);
	test = inet_aton("0.0.0.0", &client_addr.sin_addr);
		
	printf("aton s_addr: %d\n", test);




    /*conecter socket à la structure*/
	if (connect(sock_client,(struct sockaddr*) &client_addr, sizeof(struct sockaddr_in))== -1) 
	{
		perror ("Connect failed"); 
		exit(-1);
    }
    printf("Connection OK\n");




    /*Traitement*/
    socklen_t sock_client_len=sizeof(struct sockaddr_in);
	while(1)
	{
        getsockname(sock_client,(struct sockaddr*)&client_addr, &sock_client_len);
        printf("adresse IP Client: %s || Port Clien : %d\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));


        /*Transmettre message*/
		char sendBuff[1024];
    	//memset(sendBuff, '\0', sizeof(sendBuff));
		sprintf(sendBuff, "Bonjour");
		write(sock_client, sendBuff, strlen(sendBuff));


		/*reception message*/
		char recvBuff[100];
		memset(recvBuff, '\0', sizeof(recvBuff));
		read(sock_client, recvBuff, sizeof(recvBuff));
		printf( "[Message du Serveur] : %s || [Taille du message] : %ld\n", recvBuff,sizeof(recvBuff));

        exit(-1);
    }

}