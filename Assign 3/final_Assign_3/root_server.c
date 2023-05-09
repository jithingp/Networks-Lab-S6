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
    struct sockaddr_in servaddr, tldaddr1,tldaddr2, local_addr;
    char request[100];
    char response[BUFF_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Could not create socket\n");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(DNS_PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.44");

    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Could not bind socket\n");
        exit(1);
    }

    memset(&tldaddr1, 0, sizeof(tldaddr1));
    tldaddr1.sin_family = AF_INET;
    tldaddr1.sin_port = htons(DNS_PORT);
    tldaddr1.sin_addr.s_addr = inet_addr("127.0.0.22");
    
    memset(&tldaddr2, 0, sizeof(tldaddr2));
    tldaddr2.sin_family = AF_INET;
    tldaddr2.sin_port = htons(DNS_PORT);
    tldaddr2.sin_addr.s_addr = inet_addr("127.0.0.23");
    
    struct cachebro storeit[100];
    int count=0;
    
    while (1) {
        int len = sizeof(local_addr);
        int n = recvfrom(sockfd, request, 100, 0, (struct sockaddr *) &local_addr, &len);
        request[n] = '\0';

       
        int flag=0;
        for(int index=0;index<count;index++){
        	if(strcmp(request,storeit[index].domain)==0){
        		strcpy(response,storeit[index].ipofit);
        		printf("Returned from root cache\n");
        		flag=1;
        		break;
        	}
        }

        if(flag==0){
                char tempreq[200];
                strcpy(tempreq,request);
             	const char ss[4] = ".";
                char *tok = strtok(tempreq,ss);
        	char temp[100];
        	while(tok!=0){
        		strcpy(temp,tok);
        		tok = strtok(0,ss);
        	}
             
             int lenn,nn;
             if(strcmp(temp,"com")==0){
             	lenn = sizeof(tldaddr1);
       	     	if(sendto(sockfd, request, strlen(request), 0, (struct sockaddr *) &tldaddr1, lenn) < 0) {
            		perror("Could not send data\n");
            		exit(1);
        		}
                printf("Request sent to TLD server 1(.com)\n");		
             	nn = recvfrom(sockfd, response, 100, 0, (struct sockaddr *) &tldaddr1, &lenn);
             	response[nn]='\0';
             	strcpy(storeit[count].domain,request);
                strcpy(storeit[count].ipofit,response);
                count++;	
             }
             else if(strcmp(temp,"org")==0){
             	lenn = sizeof(tldaddr2);
       	     	if(sendto(sockfd, request, strlen(request), 0, (struct sockaddr *) &tldaddr2, lenn) < 0) {
            		perror("Could not send data\n");
            		exit(1);
        		}
       		printf("Request sent to TLD server 2(.org)\n"); 		
             	nn = recvfrom(sockfd, response, 100, 0, (struct sockaddr *) &tldaddr2, &lenn);
             	response[nn]='\0';
             	strcpy(storeit[count].domain,request);
                strcpy(storeit[count].ipofit,response);
                count++;	
             }
             else
                strcpy(response,"Not Found");
             

        }
        

        if (sendto(sockfd, response, strlen(response), 0, (struct sockaddr *) &local_addr, len) < 0) {
            perror("Could not send data\n");
            exit(1);
        }
    }

    close(sockfd);
    return 0;
}
