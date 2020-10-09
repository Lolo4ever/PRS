* TP2 : Mise en place d'un serveur TCP echo (suite)

**Question 1**: Quelles sont les valeurs attendues pour la variable argc dans le cas du serveur et du client? Quelle est la valeur de argv[0]?

**Answer 1**: Pour le serveur c'est on attend argc==2: le port et argv[0]. Pour le client on attend argc==3: le port, l'adresse ip et argv[0]. argv[0] représente le programme lui même.


**Question 2**: Quel est le résultat? Comment l'expliquez vous?

**Answer 2**: Marche pas. On ouvre q'une seule socket coté serveur, qui ensuite reste bloqué dans la boucle while, il n'est plus à l'écoute de nouvelles connections.


**Question 3**: Trouvez la valeur retournée par fork() dans le processus père. Que signifie cette valeur?

**Answer 3**: Le valeur du fork() retournée par le porccessus père c'est 28780, qui est équivalent au process id du processus papa.


**Question4**: Dans le processus fils, affichez la valeur des deux sockets ouvertes (celle sur laquel le serveur attend les connexions et celle créée par la fonction accept() ). Comment expliquez-vous ces valeurs?

**Answer 4**: socket_serv=3 et accept=4 (dans le procesus fils). Ce ne sont pas les mêmes valeurs car on creer un autre progeamme exactement identique avec des varialbes qui vont changer ou être le même(comme pour le socket_serv).


**Question 5**: Est-ce qu'on peut associer deux ports différents à une même socket? Quelles seraient les conséquences?

**Answer 5**: Non nous ne pouvonts pas. Le système ferait "capout".


**Question 6:** Que se passerait-il si on utilisait deux sockets différentes, en essayant d'accepter des connexions sur les deux?

**Answer 6**: Nous nous retrouvons avec deux appel blockant, un serais donc bloqué par l'autre.


**Question 7**: Donnez une fonction équivalente à un appel select() avec les trois paramètres fd_set à NULL et le paramètre timeval initialisée à 5 secondes.

**Answer 7**: C'est équivalent à un wait(5).


**Question 8**: Est-ce que vous allez effectuer cette opération d'activation à l'intérieur ou à l'extérieur de la boucle inffinie du serveur?

**Answer 8**: L'instanciation de fd_set se fait à l'extérieur de la boucle while, mais on fait FD_ZERO dans la boucle while ainsi que le FD_SET pour reinitialiser la valeur à chaque fois.


**Question 9**: Quelle sera la valeur du paramètre timeval pour être équivalents au fonctionnement préalable, qui utilisait accept()?

**Answer 9**: Accept était un appel bloquant donc ici on  va mettre timeval à NULL comme ça lui aussi sera bloquant.


**Question 10**: Donnez un squelette du code utilisée dans la boucle inffinie du serveur.

**Answer 10**: 
while(1){
	FD_ZERO(&socket_set);
    FD_SET(server_descTCP,&socket_set);
    FD_SET(server_descUDP,&socket_set);
    
	selector = select(max,&socket_set, NULL,NULL,NULL);	
	

    if(FD_ISSET(server_descTCP, &socket_set)){
        //accept TCP Client
        //Ouvre un processus pour la nouvelle écoute
        //code pour TCP
    }

	if(FD_ISSET(server_descUDP,&socket_set)){
        //code pour UDP
	}    
}