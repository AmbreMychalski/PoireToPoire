TARGET=client server 
CC=gcc
CFLAGS=

all: client server

client: ./Client/client2.c
	$(CC) $(CFLAGS) ./Client/client2.c -o client
server: ./Serveur/server2.c
	$(CC) $(CFLAGS) ./Serveur/server2.c -o server
clean:
	$(RM) $(TARGET)