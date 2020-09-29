# PRS TP1 : Mise en place d’un serveur TCP

Dans ce TP, vous allez développer un serveur et un client TCP en utilisant l’APISockets en C. L’utilisation du serveur et du client sera très simple : le texte qui va être écrit du coté client sera transmis au serveur, affiché à l’écran par le serveur, retransmis au client et affiché à l’écran par le client.
Pour pouvoir bien résoudre ce sujet, il y a deux pré-requis importants : savoir écrire un programme simple en C et savoir compiler des programmes C.

Vous devez créer deux fichiers, **serveur.c** et **client.c**, dans lesquelles vous allez pro-grammer un serveur TCP et un client TCP respectivement.
Nous allons commencer par la création d’une socket, qui se fait tout simplement enappelant la fonction **socket()**. Cette fonction prend trois paramètres :
* **domaine-** donne la famille d’adresses dans laquelle la socket sera créée. Pour le domaine IPv4, il faut utiliser la valeur AF_INET. Pour le domaine IPv6, il faut utiliser la valeur AF_INET6.
* **type-** le type de couche transport désirée, en fonction de critères applicatifs. Pour TCP, il faut utiliser la valeur SOCK_STREAM. Pour UDP, il faut utiliser la valeur SOCK_DGRAM. Si vous ne voulez pas utiliser de couche transport, vous pouvez utiliser la valeur SOCK_RAW.
* **protocole-** définit un protocole spécifique, au cas où le domaine choisi propose plusieurs protocoles du type désiré. Dans le cas de la pile TCP/IP, cette situation n’existe pas, et la valeur de ce paramètre est toujours 0.

La fonction **socket()** retourne une valeur **int**. En effet, une socket est traitée comme un fichier, dans lequel on peut écrire et lire, et la valeur retournée n’est rien d’autre que le descripteur de ce fichier.Dans les fichiers **serveur.c** et **client.c**, créez une socket TCP et affichez la valeur du descripteur de fichier obtenu. Attention, en plus des entêtes C classiques, vous devez inclure dans votre fichier les entêtes **<sys/types.h>** et **<sys/socket.h>**, qui contiennent, entre autres, la définition de toutes les constantes nécessaires (AF_INET, SOCK_STREAM,etc.).
