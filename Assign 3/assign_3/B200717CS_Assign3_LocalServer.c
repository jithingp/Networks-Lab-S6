#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024
#define DNS_PORT 8080

struct cachebro{
	char domain[100];
	char ipofit[100];
};

int main(int argc, char const *argv[]) {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr, root_servaddr;
    char request[100];
    char response[BUFF_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Could not create socket\n");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(DNS_PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Could not bind socket\n");
        exit(1);
    }

    memset(&root_servaddr, 0, sizeof(root_servaddr));
    root_servaddr.sin_family = AF_INET;
    root_servaddr.sin_port = htons(DNS_PORT);
    root_servaddr.sin_addr.s_addr = inet_addr("127.0.0.44");

    struct cachebro storeit[100];
    int count=0;
    
    while (1) {
        int len = sizeof(cliaddr);
        int n = recvfrom(sockfd, request, 100, 0, (struct sockaddr *) &cliaddr, &len);
        request[n] = '\0';

        // Check the cache for the requested domain name
        // If found, return the cached response
        // If not found, forward the request to the root server

        int flag=0;
        for(int index=0;index<count;index++){
        	if(strcmp(request,storeit[index].domain)==0){
        		strcpy(response,storeit[index].ipofit);
        		printf("Returned from local cache\n");
        		flag=1;
        		break;
        	}
        }
        if(flag==0){
             int lenn = sizeof(root_servaddr);
       	     if(sendto(sockfd, request, strlen(request), 0, (struct sockaddr *) &root_servaddr, lenn) < 0) {
            	perror("Could not send data\n");
            	exit(1);
        	}
             int nn = recvfrom(sockfd, response, 100, 0, (struct sockaddr *) &root_servaddr, &lenn);
             response[nn]='\0';
             if(strcmp(response,"Not Found")!=0){	
             	strcpy(storeit[count].domain,request);
             	strcpy(storeit[count].ipofit,response);
             	count++;
             }
        }
        

        if (sendto(sockfd, response, strlen(response), 0, (struct sockaddr *) &cliaddr, len) < 0) {
            perror("Could not send data\n");
            exit(1);
        }
    }

    close(sockfd);
    return 0;
}
