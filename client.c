#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>

char gChecksum;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
void askForFileUDP(int sockfd, unsigned int length, struct sockaddr_in server, struct sockaddr_in from)
{
	int n;
	char buffer[20];
	bzero(buffer, 20);
	printf("What file do you want?\n");
	int lengthOfFileName = read(0, buffer, 20); //read from user
	n = sendto(sockfd, buffer, lengthOfFileName - 1, 0,(const struct sockaddr *)&server,length); //send user input to server
	bzero(buffer, 20);
	
	printf("testing 1");
	
	n = recvfrom(sockfd, buffer, 20, 0,(struct sockaddr *)&from, &length); //read in whether the file exists or not
	
	printf("testing 2");
	
	write(1, buffer, 20); //write out server response to stdout
	bzero(buffer, 20);
	n = recvfrom(sockfd, buffer, 1, 0,(struct sockaddr *)&from, &length); //read in the checksum
	gChecksum = buffer[0];
	printf("\nHere is the server side checksum for your file: %u\n", buffer[0]);
	bzero(buffer, 20);
}
void getFileUDP(int socket, unsigned int length, struct sockaddr_in server, struct sockaddr_in from)
{
	int numBytes = 10000, i;
	char fileData[10000];
	bzero(fileData, 10000);
	char fileName[20];
	bzero(fileName, 20);
	FILE* file;
	
	printf("pre opening file");
	
	//read the name of the file requested from socket
	//open the file in client's folder
	recvfrom(socket, fileName, 20, 0, (struct sockaddr *)&from, &length);
	file = fopen(fileName, "w+");
	fseek(file, 0, SEEK_SET); //set to start of file
	
	//start reading in data
	while(numBytes == 10000)
	{
		numBytes = recvfrom(socket, fileData, 10000, 0, (struct sockaddr *)&from, &length);
		printf("Got %d bytes to write into my file\n", numBytes);
		//write received data into new file
		fwrite(fileData, 1, numBytes, file);
	}
	
	//calculate checksum
	fseek(file, 0, SEEK_END);
	int sizeOfFile = ftell(file);
	printf("The size of the file is %d\n", sizeOfFile);
	
	fseek(file, 0, SEEK_SET);
	unsigned char checksum = 0;
	char * charFromFile = malloc(1);
	
	//walk through the file
	//read in one char at a time
	//subtract that char from the checksum char
	//divide in order to notice if packets come in in the wrong order
	for(i = 0; i < sizeOfFile; i++)
	{
		fread(charFromFile, 1, 1, file);
		fseek(file, i, SEEK_SET);
		checksum -= *charFromFile;
		checksum = checksum/ 2;
	}
	//free memory from malloc call
	free(charFromFile);
	printf("Here is the client version of the checksum: %u\n", checksum);
//	printf("Here is the global checksum: %u\n", gChecksum);
	if(checksum == gChecksum)
		printf("Checksums match, file successfully transferred\n");
	else
		printf("Checksums do not match, something went wrong\n");
}

void askForFile(int sockfd)
{
	char buffer[20];
	bzero(buffer, 20);
//	printf("What file do you want from the server?\n");
	write(1, "What file you want? ", 20);
	int lengthOfFileName = read(0, buffer, 20); //read from user
	write(sockfd, buffer, lengthOfFileName - 1); //send user input to server
	bzero(buffer, 20);
	
	read(sockfd, buffer, 20); //read in whether the file exists or not
	write(1, buffer, 20); //write out server response to stdout
	bzero(buffer, 20);
	read(sockfd, buffer, 1); //read in the checksum
	gChecksum = buffer[0];
	printf("\nHere is the server side checksum for your file: %u\n", buffer[0]);
//	write(1, buffer, 6); //write checksum to user
	bzero(buffer, 20);
}
void getFile(int socket)
{
	int numBytes = 10000, i;
	char fileData[10000];
	bzero(fileData, 10000);
	char fileName[20];
	bzero(fileName, 20);
	FILE* file;
	
	//read the name of the file requested from socket
	//open the file in client's folder
	read(socket, fileName, 20);
	file = fopen(fileName, "w+");
	fseek(file, 0, SEEK_SET); //set to start of file
	
	//start reading in data
	while(numBytes == 10000)
	{
		numBytes = read(socket, fileData, 10000);
//		printf("Got %d bytes to write into my file\n", numBytes);
		//write received data into new file
		fwrite(fileData, 1, numBytes, file);
	}
	
	//calculate checksum
	fseek(file, 0, SEEK_END);
	int sizeOfFile = ftell(file);
//	printf("The size of the file is %d\n", sizeOfFile);
	
	fseek(file, 0, SEEK_SET);
	unsigned char checksum = 0;
	char * charFromFile = malloc(1);
	
	//walk through the file
	//read in one char at a time
	//subtract that char from the checksum char
	//divide in order to notice if packets come in in the wrong order
	for(i = 0; i < sizeOfFile; i++)
	{
		fread(charFromFile, 1, 1, file);
		fseek(file, i, SEEK_SET);
		checksum -= *charFromFile;
		checksum = checksum/ 2;
	}
	//free memory from malloc call
	free(charFromFile);
	printf("Here is the client version of the checksum: %u\n", checksum);
//	printf("Here is the global checksum: %u\n", gChecksum);
	if(checksum == gChecksum)
		printf("Checksums match, file successfully transferred\n");
	else
		printf("Checksums do not match, something went wrong\n");
}

int main(int argc, char *argv[])
{
	//checking if udp or tcp using strcmp
	char tcp1[3] = "tcp";
	char udp1[3] = "udp";
	
	//----------------------------------start TCP------------------------//
	if (strcmp(argv[3], tcp1) == 0) //checks to see if tcp was in commandline 
	{ 
		int sockfd, portno, n;
		struct sockaddr_in serv_addr;
		struct hostent *server;

		char buffer[256];
		if (argc < 3) 
		{
			fprintf(stderr,"usage %s hostname port\n", argv[0]);
			exit(0);
		}	
		
		portno = atoi(argv[2]);	
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) 
		{
			error("ERROR opening socket");
		}
		server = gethostbyname(argv[1]);
		if (server == NULL) 
		{
			fprintf(stderr,"ERROR, no such host\n");
			exit(0);
		}
		
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
		serv_addr.sin_port = htons(portno);
		
		if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		{
			error("ERROR connecting");
		}

		askForFile(sockfd);
		getFile(sockfd);
		close(sockfd);
	} 
	//----------------------------------end TCP---------------------------//
	
	//---------------------------------start UDP--------------------------//
	else if (strcmp(argv[3], udp1) == 0) //checks to see if udp was in commandline
	{ 
		int sock, n;
		unsigned int length;
		struct sockaddr_in server, from;
		struct hostent *hp;
		char buffer[256];
   
		if (argc < 3) 
		{ 
			printf("Usage: server port\n");
        	   	exit(1);
		}
		
		sock= socket(AF_INET, SOCK_DGRAM, 0);
		if (sock < 0) 
		{
			error("socket");
		}

		server.sin_family = AF_INET;
		hp = gethostbyname(argv[1]);
		if (hp==0) error("Unknown host");

		bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
		server.sin_port = htons(atoi(argv[2]));
		length=sizeof(struct sockaddr_in);
		
		
		askForFileUDP(sock, length, server, from);
		getFileUDP(sock, length, server, from);
		close(sock);
	} 
	//----------------------------------end UDP-------------------------------//

    return 0;
}
