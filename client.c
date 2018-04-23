#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
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
	printf("\nHere is the checksum for your file: %u\n", buffer[0]);
//	write(1, buffer, 6); //write checksum to user
	bzero(buffer, 20);
}
void getFile(int socket)
{
	char fileData[10000];
	bzero(fileData, 10000);
	char fileName[20];
	bzero(fileName, 20);
	FILE* file;
	
	//read the name of the file requested from socket
	//open the file in client's folder
	read(socket, fileName, 20);
	file = fopen(fileName, "w");
	fseek(file, 0, SEEK_SET); //set to start of file
	
	//start reading in data
	int numBytes = read(socket, fileData, 10000);
	printf("Got %d bytes to write into my file\n", numBytes);
	//
	//write received data into new file
	fwrite(fileData, 1, numBytes, file);
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
		
/*		printf("Please enter the message: ");
		bzero(buffer,256);
		fgets(buffer,255,stdin);
		n = write(sockfd,buffer,strlen(buffer));
		if (n < 0)
		{			
			error("ERROR writing to socket");
		}
		
		bzero(buffer,256);
		n = read(sockfd,buffer,255);
		
		if (n < 0) 
		{
			error("ERROR reading from socket");
		}'*/

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
		
/*		printf("Please enter the message: ");
		bzero(buffer,256);
		fgets(buffer,255,stdin);
		n=sendto(sock,buffer, strlen(buffer),0,(const struct sockaddr *)&server,length);
		
		if (n < 0) error("Sendto");
		n = recvfrom(sock,buffer,256,0,(struct sockaddr *)&from, &length);
		
		if (n < 0) error("recvfrom");
		write(1,"Got an ack: ",12);
		write(1,buffer,n); */
		
		askForFile(sock);
		printf("%s\n",buffer);
		close(sock);
	} 
	//----------------------------------end UDP-------------------------------//

    return 0;
}
