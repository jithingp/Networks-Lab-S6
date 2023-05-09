#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024
#define DNS_PORT 8080

struct storebro{
	char domain[100];
	char ipofit[100];
};

int main(int argc, char const *argv[]) {
    int sockfd;
    struct sockaddr_in servaddr, tldaddr, root_servaddr;
    char request[100];
    char response[BUFF_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Could not create socket\n");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(DNS_PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.5");

    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Could not bind socket\n");
        exit(1);
    }

    struct storebro storeit[4];
    strcpy(storeit[0].domain,"www.site1.amnesty.com");
    strcpy(storeit[0].ipofit,"20.56.44.33");
    strcpy(storeit[1].domain,"www.site2.amnesty.com");
    strcpy(storeit[1].ipofit,"30.6.224.33");
    strcpy(storeit[2].domain,"www.site3.amnesty.com");
    strcpy(storeit[2].ipofit,"40.100.44.33");
    strcpy(storeit[3].domain,"www.site4.amnesty.com");
    strcpy(storeit[3].ipofit,"50.56.44.33");
    int count=0;
    
    while (1) {
        int len = sizeof(tldaddr);
        int n = recvfrom(sockfd, request, 100, 0, (struct sockaddr *) &tldaddr, &len);
        request[n] = '\0';

       
        int flag=0;
        for(int index=0;index<4;index++){
        	if(strcmp(request,storeit[index].domain)==0){
        		strcpy(response,storeit[index].ipofit);
        		printf("Returned from auth\n");
        		flag=1;
        		break;
        	}
        }
        if(flag==0){
            strcpy(response,"Not Found");
            printf("Not Found\n");
        }
        

        if (sendto(sockfd, response, strlen(response), 0, (struct sockaddr *) &tldaddr, len) < 0) {
            perror("Could not send data\n");
            exit(1);
        }
    }

    close(sockfd);
    return 0;
}
