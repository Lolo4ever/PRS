main: client.o serveur.o clientUDP.o
	gcc client.o -o client
	gcc clientUDP.o -o clientUDP
	gcc serveur.o -o serveur

client.o: client.c
	gcc -c client.c -o client.o

clientUDP.o: clientUDP.c
	gcc -c clientUDP.c -o clientUDP.o

serveur.o: serveur.c
	gcc -c serveur.c -o serveur.o

clean:
	rm -f *.o

