
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>  // for strlen()
#include <sys/types.h>
#include <sys/socket.h>  // for socket(), bind()
#include <arpa/inet.h>   // for sockaddr_in
#include <netinet/in.h>
	
#define PORT	 8080
#define MAXLINE 1024

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
 	

int main() {
	int sockfd;
	char buffer[MAXLINE];
	char hello[MAXLINE];
	struct sockaddr_in servaddr, cliaddr;
		
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
		
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);
		
	// Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
		
	int len, n;
	
	len = sizeof(cliaddr);
	
	n = recvfrom(sockfd, (char *)buffer, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr,
				&len);
	buffer[n] = '\0';
	printf("Client : %s\n", buffer);
	revstr(buffer);
	
	sendto(sockfd, (const char *)buffer, strlen(buffer),
		MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
			len);
	printf("Server : %s\n",buffer);
		
	return 0;
}

