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
		
		bzero(buffer,256);
		n = read(newsockfd,buffer,255);
		if (n < 0)
		{
			error("ERROR reading from socket");
		}
		
		printf("Here is the message: %s\n",buffer);
		n = write(newsockfd,"I got your message",18);
		
		if (n < 0)
		{			
			error("ERROR writing to socket");
		}
		
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
		while (1) 
		{
			n = recvfrom(sock,buf,1024,0,(struct sockaddr *)&from,&fromlen);
			if (n < 0)
			{				
				error("recvfrom");
			}
			
			write(1,"Received a datagram: ",21);
			write(1,buf,n);
			n = sendto(sock,"Got your message\n",17, 0,(struct sockaddr *)&from,fromlen);
			
			if (n  < 0) 
			{
				error("sendto");
			}
		}
	}
	
    return 0;  
}
