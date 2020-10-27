main: clientUDP.o serveur.o
	gcc clientUDP.o -o clientUDP
	gcc serveur.o -o serveur
	
clientUDP.o: clientUDP.c
	gcc -Wall -c clientUDP.c -o clientUDP.o

serveur.o: serveur.c 
	gcc -Wall -c serveur.c -o serveur.o

clean:
	rm -f *.o

