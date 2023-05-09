#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <fcntl.h>
#define MAX 80
#define PORT 9034
#define SA struct sockaddr

void chatfn(int sockfd){
	char buff[MAX],rmess[MAX];
	int n;
	fd_set read_fds;
	FD_ZERO(&read_fds);
	
	while(1){
		bzero(buff, sizeof(buff));
		bzero(rmess, sizeof(rmess));
		FD_SET(sockfd,&read_fds);
		FD_SET(0,&read_fds);
		if(select(sockfd+1,&read_fds,NULL,NULL,NULL)==-1){
			perror("select");
			exit(4);
		}
		if(FD_ISSET(sockfd,&read_fds)){
	
			//read(sockfd, rmess, sizeof(rmess));
			if(recv(sockfd, rmess, 1000, 0)> 0){
				printf("Received : %s\n", rmess);
			}
		}
		if(FD_ISSET(0,&read_fds)){
			n = 0;
			char temp;
			while (( temp = getchar()) != '\n')
				buff[n++]=temp;
			if((strcmp(buff,"quit")==0)||(strcmp(buff,"exit")==0)||(strcmp(buff,"part")==0))
				break;		
			//write(sockfd,buff,sizeof(buff));
			if(send(sockfd, buff, strlen(buff),0) < 0){
				printf("Send failed");
				exit(0);
			}
		}

	}
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//fcntl(sockfd, F_SETFL, O_NONBLOCK);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //inet_addr function converts the internet host address from IPv4 number and dots notation into binary data in network byte order
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))!= 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat
	chatfn(sockfd);

	// close the socket
	close(sockfd);
}

