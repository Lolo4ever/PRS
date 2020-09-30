# PRS TP1 : Mise en place d’un serveur TCP

Dans ce TP, vous allez développer un serveur et un client TCP en utilisant l’APISockets en C. L’utilisation du serveur et du client sera très simple : le texte qui va être écrit du coté client sera transmis au serveur, affiché à l’écran par le serveur, retransmis au client et affiché à l’écran par le client.
Pour pouvoir bien résoudre ce sujet, il y a deux pré-requis importants : savoir écrire un programme simple en C et savoir compiler des programmes C.

Vous devez créer deux fichiers, **serveur.c** et **client.c**, dans lesquelles vous allez pro-grammer un serveur TCP et un client TCP respectivement.
Nous allons commencer par la création d’une socket, qui se fait tout simplement enappelant la fonction **socket()**. Cette fonction prend trois paramètres :
* **domaine-** donne la famille d’adresses dans laquelle la socket sera créée. Pour le domaine IPv4, il faut utiliser la valeur AF_INET. Pour le domaine IPv6, il faut utiliser la valeur AF_INET6.
* **type-** le type de couche transport désirée, en fonction de critères applicatifs. Pour TCP, il faut utiliser la valeur SOCK_STREAM. Pour UDP, il faut utiliser la valeur SOCK_DGRAM. Si vous ne voulez pas utiliser de couche transport, vous pouvez utiliser la valeur SOCK_RAW.
* **protocole-** définit un protocole spécifique, au cas où le domaine choisi propose plusieurs protocoles du type désiré. Dans le cas de la pile TCP/IP, cette situation n’existe pas, et la valeur de ce paramètre est toujours 0.

La fonction **socket()** retourne une valeur **int**. En effet, une socket est traitée comme un fichier, dans lequel on peut écrire et lire, et la valeur retournée n’est rien d’autre que le descripteur de ce fichier.

Dans les fichiers **serveur.c** et **client.c**, créez une socket TCP et affichez la valeur du descripteur de fichier obtenu. Attention, en plus des entêtes C classiques, vous devez inclure dans votre fichier les entêtes **<sys/types.h>** et **<sys/socket.h>**, qui contiennent, entre autres, la définition de toutes les constantes nécessaires (AF_INET, SOCK_STREAM,etc.).

**Question 1 : Quelle est la valeur du descripteur ? Quelle est l’explication de ce résultat ?**
**Answer :** *La valeur du descripteur pour le client et le serveur c'est 3. Si il n'y a pas d'erreur le descripteur contient une valeur entière non nulle. Le chiffre 3 indique un nouveau standard, descritpeur libre pour l'utilisateur à utiliser, la valeur 0,1 et 2 étant réservé par le système pour les standard in et out*

Dans le cas d’une valeur négative du descripteur de fichier, la socket n’a pas été créé ecorrectement. Traitez cette possibilité d’erreur dans votre code en arrêtant l’exécution du programme. De manière générale, toutes les fonctions de l’API Sockets retournent des valeurs int, inférieures à 0 en cas d’erreur. Pensez à traiter à chaque appel ces erreurs possibles ; ça peut être très important pour le débuggage.

Note : Certains systèmes d’exploitation gardent la socket bloquée même après l’arrêt du processus qui l’utilisait, au cas où il reste des packets entransit. Cependant, ce mécanisme peut poser des problèmes, surtout dans une phase de développement quand on veut pouvoir tuer des processus et les relancer sur la même socket. La fonction **setsockopt()** permet de modifier certains paramètres de la socket, dont le paramètre SO_REUSEADDR, unevariable booléenne qui permet (ou pas) la réutilisation immédiate de la socket. Par défaut à 0 (faux), ce paramètre peut être passé à 1 (vrai) en utilisant le code suivant :
**int reuse= 1 ;setsockopt(descripteur, SOL_SOCKET, SO_REUSEADDR,&reuse, sizeof(reuse)) ;**

La socket que vous venez de créer n’est pour l’instant pas associée à la couche réseau et elle peut être utilisée soit du côté client, soit du côté serveur. Pour le serveur, il faut faire le lien entre cette socket et un couple (adresse IP, port). Pour ce faire, vous devez utilisez la structure sockaddr, qui est un container générique pour tout type d’adresse réseau. sockaddr utilise une représentation opaque, dans un format chaine de caractères suffisamment grand pour toute famille d’adresses. Pour simplifier la manipulation des adresses, dans le cas d’une couche réseau IP, on utilise struct sockaddr_in, définie dans l’en-tête <netinet/in.h>, qui a la même taille qu’une structure sockaddr, mais une représentation interne différente. Cela facilite un certain nombre d’opérations, mais il ne faut pas oublier que struct sockaddr_in n’est a priori pas connue par les fonctions de l’API Sockets, qui sont définies avec des paramètres struct sockaddr. Il ne faut donc pas oublier de faire un cast chaque fois quand vous passez une variable sockaddr_in comme paramètre d’une telle fonction.

Note : Il est conseillé de remettre à zéro une structure struct sockaddr_in avant son initialisation, pour éviter tout problème d’accès à des zones mémoire libérées mais non effacées. Une possibilité pour cette remise à zéro utilise la fonction memset(), définie dans  <string.h>: 
**struct sockaddr_in my_addr ; memset((char*)&my_addr, 0, sizeof(my_addr)) ;**

Pour pouvoir utiliser une structure sockaddr_in, il faut initialiser trois de ses champs :
* sin_family: qui doit correspondre à la famille (au domaine) de la socket à laquelle on associera l’adresse.
* sin_port: le numéro du port sur lequel la socket sera ouverte.
* sin_addr: l’adresse réseau associée à la socket (dans notre cas une adresse IP). Ce champ est, à son tour, une structure struct in_addr. Pour l’initialiser, il faut pratiquement initialiser le champs_addr de la structure sin_addr. Par exemple,cela peut se faire avec le code suivant :
    **my_addr.sin_addr.s_addr= INADDR_ANY ;**
Du côté serveur, les deux adresses qu’on utilise le plus souvent pour connecter la socket sont INADDR_ANY et INADDR_LOOPBACK. La première option permet de connecter la socket à toutes les interfaces disponibles, tandis que la deuxième permet la connexion à l’hôte local (localhost).

**Question 2 : Quelle est la valeur (en base 10) de INADDR_ANY et INADDR_LOOPBACK ? Qu’est-ce que cela donne en format IP ?**
**Answer :**  
* *Pour ANY: print "0"; c'est l'adresse 0.0.0.0 .* 
* *Pour LOOPBACK : print "2130706433"; c'est l'adresse 127.0.0.1 (locale)*

**Question 3 : Comment expliquez-vous l’utilisation d’une adresse de type INADDR_ANY ?Pourquoi on ne récupère pas explicitement l’adresse IP d’une interface ?**
**Answer :** *On utilise ANY pour pouvoir acccepter les connections de machines en dehors du réseau locale. On ne récupère pas explicitement l'adresse IP d'une interface pour pouvoir se connecter à plusieurs interfaces, et pouvoir binder sur n'importe quelle adrresse IP*

Différents architectures matérielles utilisent différentes ordres d’organisation en mémoire des octets qui forment une structure de données (la fameuse disputebig endian vs.little endian). L’API Sockets a comme objectif de permettre l’interconnexion de machines hétérogènes et, pour cela, le module <arpa/inet.h> propose une série de fonctions permettant de traduire les structures dans un format réseau (qui est en réalité le format big endian). Ces fonctions sont :
* int htonl(int host_long) - passe un entier long du format machine en format réseau ;
* int htons(int host_short) - passe un entier court du format machine en format réseau ;
* int ntohl(int host_long) - passe un entier long du format réseau en format machine ;
* int ntohs(int host_short) - passe un entier court du format réseau en format machine. 
Vous devez utiliser ces fonctions à l’initialisation de la structure sockaddr_in (pour le port et l’adresse IP) et chaque fois quand vous voulez transmettre une structure multi-octets sur le réseau.

**Question 4 : Est-il nécessaire d’utiliser les fonction shtonl() et ntohl() lorsque l’adresse IP utilisée est INADDR_ANY ? Pourquoi ?**
**Answer :** *On utlise htonl() avec INADDR_ANY car on veux passer d'une addresse en base 10 à une addresse au format réseau*

Du côté client, vous devez aussi définir une structure sockaddr_in. Par contre, différemment du serveur, cette structure n’enregistrera pas le numéro de port et l’adresse locale (le client), mais ceux de la machine distante (le serveur auquel on veut se connecter). Cela veut dire que le client doit connaitre l’adresse IP et le port sur lesquels fonctionnent le serveur. Deux fonctions qui peuvent beaucoup faciliter l’initialisation du champs_addr avec une adresse IP connue est inet_aton() et inet_ntoa(). L’avantage de cette fonctionest qu’on peut lui donner comme paramètre une chaine de caractères contenant l’adresse IP, qu’elle convertira au bon format.

**Question 5 : Afficher la valeur retournée par la fonction inet_aton(). Est-ce qu’il faut combiner cette fonction avec les fonctions htonl() et ntohl()?**
**Answer :** *inet_aton() retourne 1 si c'est bon ou 0 si il y a une erreur. Nous n'avons pas besoin de combiner cela avec les fonctions htonl() ou ntohl() car inet_aton() converti dejà en format réseau la valeur passé en paramètre*


Pour le serveur, une fois la structure sockaddr_in initialisée, vous devez réaliser le lien entre cette structure et la socket, en utilisant la fonction int bind(). Cette fonction demande trois paramètres :
* descripteur- le descripteur de la socket serveur.
* adresse- un pointeur vers une structure sockaddr à associer à la socket. Vous pouvez utiliser un pointeur vers votre structure sockaddr_in, en n’oubliant pas le cast.
* taille- spécifie la taille, en octets, de la structure pointée paradresse.

Note :La fonction sizeof()vous permet d’obtenir automatiquement la taille d’une variable.

La socket est maintenant créée et initialisée. Il ne faut pas oublier que vous êtes du coté serveur : le prochain pas est donc de déclarer cette socket comme passive, en attente d’une demande de connexion. Cela se fait en utilisant la fonction listen(), avec deux paramètres : le descripteur de la socket en question et un paramètre int qui donne le nombre maximal de connexions en attente sur la socket. 
Pour le client, vous n’avez pas besoin d’utiliser la fonction bind() (mais vous pouvez le faire). Le serveur n’a pas besoin de connaitre en avance l’adresse du client, car la connexion est toujours initialisée par ce dernier, et ses identifiants seront envoyés dans la demande de connexion. Si la socket client n’est pas associée à une structure sockaddr_in, le système d’exploitation va automatiquement lui associer une lorsqu’une demande de connexion est faite. Le client fait une demande de connexion en utilisant la fonction connect(), avec ces trois paramètres :
* descripteur- le descripteur de la socket client.
* adresse- un pointeur vers une structure sockaddr qui donne l’adresse du serveur.
* taille- comme dans le cas de bind, ce paramètre spécifie la taille, en octets, de la structure pointée par adresse.
Le serveur et le client doivent prendre la forme d’un daemon, c’est à dire d’un processus qui s’exécute en permanence, sans l’intervention de l’utilisateur. Pour cela, votre serveur et votre client doivent inclure, après l’appel à la fonction listen(), une boucle infinie.

**Question 6 : Donnez deux syntaxes différentes pour réaliser une boucle infinie.**
**Answer :** *On peut utiliser soit while(1) ou for(;;)*


A l’intérieur de cette boucle infinie, du côté serveur, vous devez rester en attente d’une demande de connexion et, éventuellement, la traiter. Le traitement d’une demande de connexion se fait à l’aide de la fonction int accept(). Cette fonction est bloquante,ce qui implique que votre programme n’avancera pas au delà de cet appel tant qu’une demande de connexion n’est reçue. La fonction accept() prend trois paramètres :
* descripteur- le descripteur de la socket ouverte par le serveur ;
* adresse- est un paramètre résultat. La valeur initiale doit être un pointeur vers une structure sockaddr non-initialisée. Après l’appel, la structure contiendra les identifiants du client ;
* taille- un pointeur vers un entier de type socklen_t initialisé avec la taille de la structure sockaddr vide, qui pointera après l’appel vers la taille réelle des données retournées.

**Question 7 : Quelle est la valeur retournée par la fonction accept()? Que représente cette valeur ?**
**Answer :** *Elle retourne la valeur '4' qui est encore une fois une description de fichier, le 3 est déjà utilisé par l'autre descripeur de fichier que nous avons initialisé*


Vous avez établi une connexion entre le serveur et le client. Tout ce qu’il vous reste à faire maintenant est de transmettre et de recevoir des messages. Pour ce faire, vous pouvez simplement écrire et lire sur les sockets correspondants, en utilisant les fonctions read() et write() définies dans l’entête <unistd.h>. Les deux fonctions ont la même signature :
* descripteur- le descripteur de la socket utilisée pour lire ou écrire ;
* buffer- un tableau de caractères, qui représente le buffer dans lequel vous allez lire ou écrire ;
* taille- un entier qui donne le nombre d’octets écrits dans le buffer (dans le cas de write()) ou la taille maximale du buffer de lecture (pour read()) .

**Question 8 : Donnez le code utilisé pour transmettre et recevoir des messages coté serveur.**
**Answer :** 


        /*reception message*/
		char recvBuff[100];
		memset(recvBuff, '\0', sizeof(recvBuff));
		read(socket_service, recvBuff, sizeof(recvBuff));
		printf( "%s\n", recvBuff);



		/*Transmettre message*/
		char sendBuff[1024];
    	memset(sendBuff, '\0', sizeof(sendBuff));
		sprintf(sendBuff, "message recu!");
		write(socket_service, sendBuff, strlen(sendBuff));


**Question 9 : Donnez le code utilisé pour transmettre et recevoir des messages coté client.**
**Answer :**


        /*Transmettre message*/
		char sendBuff[1024];
    	memset(sendBuff, '\0', sizeof(sendBuff));
		sprintf(sendBuff, "Bonjour");
		write(sock_client, sendBuff, strlen(sendBuff));


		/*reception message*/
		char recvBuff[100];
		memset(recvBuff, '\0', sizeof(recvBuff));
		read(sock_client, recvBuff, sizeof(recvBuff));
		printf( "%s\n", recvBuff);


Faites attention, les sockets TCP transportent un stream d’octets, pas un stream de packets, mais derrière ces sockets, il y a bien une fragmentation en segments TCP et packets IP. Cela veut dire que, si vous écrivez 20 octets sur une socket, vous n’êtes pas sûrs de lire 20 octets au récepteur. En effet, vous pouvez recevoir moins si vos donnée sont été fragmentées. Si la quantité de données à lire est importante pour vous, c’est àvous de développer un protocole qui vous permet de transmettre cette information avec les données utiles (plus de détails sur cela dans les prochains TPs).

**Question 10 : Du coté serveur, récupérez le port et l’adresse IP du client. Quelles sont les valeurs de ces paramètres ?**
**Answer :** *L'adresse IP du client est 127.0.0.1 et le port est 39570 (quand on converti du format réseau à une valeur décimale avec les fionctions inet_ntoa et ntohs pour passer au dela des problème de bit de poids faible/fort)*

Côté client, si vous souhaitez découvrir le port de la connexion (du client), vous pouvez utiliser la fonction : **getsockname (int sockfd, struct sockaddr *addr, socklen_t *addrlen)**.

Finalement, n’oubliez pas de fermer vos sockets, en utilisant la fonction close().