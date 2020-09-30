main: client.o serveur.o 
	gcc client.o -o client
	gcc serveur.o -o serveur

client.o: client.c
	gcc -c client.c -o client.o

serveur.o: serveur.c
	gcc -c serveur.c -o serveur.o

clean:
	rm -f *.o

