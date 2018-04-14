PROG = client server
OBJS = client.o server.o
CC = gcc
CPPFLAGS = -c  #compile flags
#LDFLAGS =

client: client.o server
	$(CC) -o  client client.o
client.o: client.c
	$(CC) $(CPPFLAGS)  client.c
server: server.o
	$(CC) -o server server.o
server.o: server.c
	$(CC) $(CPPFLAGS) server.c
clean:
	rm -f core $(PROG) $(OBJS)
