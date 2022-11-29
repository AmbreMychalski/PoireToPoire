TARGET=client server 
CC=gcc
CFLAGS= -Wall -Wextra -g

normal: $(TARGET)
client: client2.c
    $(CC) $(CFLAGS) ./Client/client2.c -o client
server: server2.c
    $(CC) $(CFLAGS) ./Server/server2.c -o server
clean:
    $(RM) $(TARGET)