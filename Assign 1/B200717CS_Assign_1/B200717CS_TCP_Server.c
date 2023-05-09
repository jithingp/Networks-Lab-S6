#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Function designed for chat between client and server.

void revstr(char *str1){
   int i,len;
   char temp;
   len = strlen(str1);
   
   for(i=0;i<len/2;i++){
   	temp=str1[i];
   	str1[i]=str1[len-i-1];
   	str1[len-i-1]=temp;
  }
 }
 
void chatfunc(int connfd)
{
	char buff[MAX];
	int n;
	
		bzero(buff, MAX);

		// read the message from client and copy it in buffer
		//read(connfd, buff, sizeof(buff));
		if(recv(connfd, buff, 1000, 0)< 0){
			printf("recv failed");
			exit(0);
		}
		// print buffer which contains the client contents
		printf("From client: %s\n To client : ", buff);
		
		n = 0;

		// and send reverse to client
		revstr(buff);
		printf("%s\n",buff);
		//write(connfd, buff, sizeof(buff));
		if(send(connfd, buff, strlen(buff),0) < 0){
			printf("Send failed");
			exit(0);
		}
		bzero(buff, MAX);
		
	
}

int main()
{
	int sockfd, connfd, len,opt=1;
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
	
	if(setsockopt(sockfd,SOL_SOCKET,
		SO_REUSEADDR | SO_REUSEPORT, &opt,
		sizeof(opt))){
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accepts the client...\n");

	// Function for chatting between client and server
	chatfunc(connfd);

	// After chatting close the socket
	close(sockfd);
}

