/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void createChecksumUDP(FILE* file, int size, int socket, struct sockaddr_in from, socklen_t fromlen)
{
	fseek(file, 0, SEEK_SET);
	unsigned char checksum = 0;
	char * charFromFile = malloc(1);
	int i;
	
	//walk through the file
	//read in one char at a time
	//subtract that char from the checksum char
	//divide in order to notice if packets come in in the wrong order
	for(i = 0; i < size; i++)
	{
		fread(charFromFile, 1, 1, file);
		fseek(file, i, SEEK_SET);
		checksum -= *charFromFile;
		checksum = checksum/ 2;
	}
	//free memory from malloc call
	free(charFromFile);
	//create a pointer so the checksum can be sent to client
	char *pChecksum = &checksum;
	sendto(socket, pChecksum, 1, 0, (struct sockaddr *)&from,fromlen);
	printf("Here is the checksum: %u\n", checksum);
}

void sendFileUDP(FILE* file, int size, int socket, struct sockaddr_in from, socklen_t fromlen)
{
	//IF THE SLEEP IS REMOVED IT WILL BREAK
	sleep(1); //to make sure the fileName doesnt start reading data
	int i;
	char fileData[10000];
	bzero(fileData, 10000);
	int numBytesRead;
	
	//read into fileData buffer, then send to client via socket
	fseek(file, 0, SEEK_SET);
	sleep(1);
	for(i = 0; i * 10000 < size; i++)
	{
		numBytesRead = fread(fileData, 1, 10000, file);
		sendto(socket, fileData, numBytesRead, 0, (struct sockaddr *)&from,fromlen);
		bzero(fileData, 10000);
		sleep(1);
	}
}

void checkAndSendFileUDP(int newsockfd, struct sockaddr_in from, socklen_t fromlen)
{
	int i, n, lengthOfName;
	FILE* data;	
	char fileName[20];
	lengthOfName = recvfrom(newsockfd, fileName, 20, 0, (struct sockaddr *)&from,&fromlen);
		
	fileName[lengthOfName] = '\0'; //convert to a string by adding a null terminator
	printf("file name length: %d\n", lengthOfName);
	printf("%s is the string you are looking for\n", fileName);
	sleep(1);
	
	if(access( fileName, F_OK ) != -1 ) 
	{
		//file exists
		n = sendto(newsockfd, "File exists\n", 11, 0, (struct sockaddr *)&from,fromlen);
	} 
	else 
	{
		//file does not exist
		n = sendto(newsockfd, "File does not exist\n", 19, 0, (struct sockaddr *)&from,fromlen);	
		//this probably needs a pointer or something
		//good luck future ian
		//fileName is blank in the printf
		exit(0);
	}
	
	data = fopen(fileName, "r");
		
	
	fseek(data, 0, SEEK_END);
	int sizeOfFile = ftell(data); //get the length of the file
	char buffer[sizeOfFile];
	fseek(data, 0, SEEK_SET); //go back to the start of the file
	
	printf("%d is the size of the file\n", sizeOfFile);
	
	sleep(1);
	//generate a checksum - we want to get 6 chars from the file
	//first char, then 1/5, 2,5, 3/5, 4/5, last
	createChecksumUDP(data, sizeOfFile, newsockfd, from, fromlen);
	sendto(newsockfd, fileName, lengthOfName, 0, (struct sockaddr *)&from,fromlen); //send client requested file name so client can open the file
	sendFileUDP(data, sizeOfFile, newsockfd, from, fromlen);
	
}

void createChecksum(FILE* file, int size, int socket)
{
	fseek(file, 0, SEEK_SET);
	unsigned char checksum = 0;
	char * charFromFile = malloc(1);
	int i;
	
	//walk through the file
	//read in one char at a time
	//subtract that char from the checksum char
	//divide in order to notice if packets come in in the wrong order
	for(i = 0; i < size; i++)
	{
		fread(charFromFile, 1, 1, file);
		fseek(file, i, SEEK_SET);
		checksum -= *charFromFile;
		checksum = checksum/ 2;
	}
	//free memory from malloc call
	free(charFromFile);
	//create a pointer so the checksum can be sent to client
	char *pChecksum = &checksum;
	write(socket, pChecksum, 1);
	printf("Here is the checksum: %u\n", checksum);
}

void sendFile(FILE* file, int size, int socket)
{
	//IF THE SLEEP IS REMOVED IT WILL BREAK
	sleep(1); //to make sure the fileName doesnt start reading data
	int i;
	char fileData[10000];
	bzero(fileData, 10000);
	int numBytesRead;
	
	//read into fileData buffer, then send to client via socket
	fseek(file, 0, SEEK_SET);
	for(i = 0; i * 10000 < size; i++)
	{
		numBytesRead = fread(fileData, 1, 10000, file);
		write(socket, fileData, numBytesRead);
		bzero(fileData, 10000);
//		sleep(1);
	}
}

void checkAndSendFile(int newsockfd)
{
	int i, n, lengthOfName;
	FILE* data;	
	char fileName[20];
	lengthOfName = read(newsockfd, fileName, 20);
		
	fileName[lengthOfName] = '\0'; //convert to a string by adding a null terminator
	printf("file name length: %d\n", lengthOfName);
	printf("%s is the string you are looking for\n", fileName);
	
	if( access( fileName, F_OK ) != -1 ) 
	{
		//file exists
		n = write(newsockfd, "File exists\n", 11);
	} 
	else 
	{
		//file does not exist
		n = write(newsockfd, "File does not exist\n", 19); 		
		//this probably needs a pointer or something
		//good luck future ian
		//fileName is blank in the printf
		exit(0);
	}
	
	data = fopen(fileName, "r");
		
	
	fseek(data, 0, SEEK_END);
	int sizeOfFile = ftell(data); //get the length of the file
	char buffer[sizeOfFile];
	fseek(data, 0, SEEK_SET); //go back to the start of the file
	
	printf("%d is the size of the file\n", sizeOfFile);
	
	//generate a checksum - we want to get 6 chars from the file
	//first char, then 1/5, 2,5, 3/5, 4/5, last
	createChecksum(data, sizeOfFile, newsockfd);
	write(newsockfd, fileName, lengthOfName); //send client requested file name so client can open the file
	sendFile(data, sizeOfFile, newsockfd);
	
}

int main(int argc, char *argv[])
{
	//checking if udp or tcp using strcmp
	char tcp1[3] = "tcp";
	char udp1[3] = "udp";
	
	//----------------------------------start TCP------------------------//
	if (strcmp(argv[2], tcp1) == 0) //checks to see if tcp was in commandline 
	{ 
		int sockfd, newsockfd, portno;
		socklen_t clilen;
		char buffer[256];
		struct sockaddr_in serv_addr, cli_addr;
		int n;
		
		if (argc < 2) 
		{
			fprintf(stderr,"ERROR, no port provided\n");
			exit(1);
		}
	
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		
		if (sockfd < 0) 
		{
			error("ERROR opening socket");
		}
		
		bzero((char *) &serv_addr, sizeof(serv_addr));
		portno = atoi(argv[1]);
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(portno);
		
		if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		{
              error("ERROR on binding");
		}
		
		listen(sockfd,5);
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		
		if (newsockfd < 0) 
		{
          error("ERROR on accept");
		}
		
		checkAndSendFile(newsockfd);
		
		close(newsockfd);
		close(sockfd);
	}
	//----------------------------------end TCP---------------------------//
	
	//---------------------------------start UDP--------------------------//
	else if (strcmp(argv[2], udp1) == 0) //checks to see if udp was in commandline
	{
		int sock, length, n;
		socklen_t fromlen;
		struct sockaddr_in server;
		struct sockaddr_in from;
		char buf[1024];

		if (argc < 2) 
		{
			fprintf(stderr, "ERROR, no port provided\n");
			exit(0);
		}
   
		sock=socket(AF_INET, SOCK_DGRAM, 0);
		if (sock < 0) 
		{
			error("Opening socket");
		}
		
		length = sizeof(server);
		bzero(&server,length);
		server.sin_family=AF_INET;
		server.sin_addr.s_addr=INADDR_ANY;
		server.sin_port=htons(atoi(argv[1]));
		if (bind(sock,(struct sockaddr *)&server,length)<0) 
		{
			error("binding");
		}
		
		fromlen = sizeof(struct sockaddr_in);
		
		checkAndSendFile(sock);
	}
	
    return 0;  
}
