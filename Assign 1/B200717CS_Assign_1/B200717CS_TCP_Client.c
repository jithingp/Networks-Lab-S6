#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void chatfunc(int sockfd)
{
	char buff[MAX];
	int n;
	
		bzero(buff, sizeof(buff));
		printf("Enter the string : ");
		n = 0;
		char temp;
		while (( temp = getchar()) != '\n')
			buff[n++]=temp;
		//write(sockfd, buff, sizeof(buff));
		if(send(sockfd, buff, strlen(buff),0) < 0){
			printf("Send failed");
			exit(0);
		}
		bzero(buff, sizeof(buff));
		//read(sockfd, buff, sizeof(buff));
		if(recv(sockfd, buff, 1000, 0)< 0){
			printf("recv failed");
			exit(0);
		}
		printf("From Server : %s\n", buff);


}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
	chatfunc(sockfd);

	// close the socket
	close(sockfd);
}

