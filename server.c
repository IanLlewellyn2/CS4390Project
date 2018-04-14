/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


void checkAndSendFile(char[], int, int);
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void checkAndSendFile(char fileName[], int lengthOfName, int newsockfd)
{
	int i, n;
	FILE* data;	
		
	fileName[lengthOfName] = '\0'; //convert to a string by adding a null terminator
	
	if( access( fileName, F_OK ) != -1 ) 
	{
		//file exists
		n = write(newsockfd, "File exists", 11);
	} 
	else 
	{
		//file does not exist
		n = write(newsockfd, "File does not exist", 19); 
		exit(0);
	}
	
	data = fopen(fileName, "r");
		
	
	fseek(data, 0, SEEK_END);
	int sizeOfFile = ftell(data); //get the length of the file
	char buffer[sizeOfFile];
	fseek(data, 0, SEEK_SET); //go back to the 
	
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);
     if (newsockfd < 0)
          error("ERROR on accept");
     bzero(buffer,256);
     n = read(newsockfd,buffer,255); //n is number of bytes read
     if (n < 0) error("ERROR reading from socket");
     printf("Here is the message: %s\n",buffer);
     n = write(newsockfd,"I got your message",18);
     if (n < 0) error("ERROR writing to socket");
	 
	 
	 
     close(newsockfd);
     close(sockfd);
     return 0;
}
