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
struct nsrecord{
	char name[100];
	char value[100];
	char type[10];
};
struct arecord{
	char name[100];
	char value[100];
	char type[10];
};

int main(int argc, char const *argv[]) {
    int sockfd;
    struct sockaddr_in servaddr, authaddr, root_servaddr;
    
    struct nsrecord ns1[2];
    strcpy(ns1[0].name,"unicef.org");
    strcpy(ns1[0].value,"dns.unicef.org");
    strcpy(ns1[0].type,"NS");
    strcpy(ns1[1].name,"redcross.org");
    strcpy(ns1[1].value,"dns.redcross.org");
    strcpy(ns1[1].type,"NS");
    
    struct arecord a1[2];
    strcpy(a1[0].name,"dns.unicef.org");
    strcpy(a1[0].value,"127.0.0.6");
    strcpy(a1[0].type,"A");
    strcpy(a1[1].name,"dns.redcross.org");
    strcpy(a1[1].value,"127.0.0.7");
    strcpy(a1[1].type,"A");
    
    char request[100];
    char response[BUFF_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Could not create socket\n");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(DNS_PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.23");

    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Could not bind socket\n");
        exit(1);
    }

    memset(&authaddr, 0, sizeof(authaddr));
    authaddr.sin_family = AF_INET;
    authaddr.sin_port = htons(DNS_PORT);
    authaddr.sin_addr.s_addr = inet_addr("127.0.0.6");
    
    struct cachebro storeit[100];
    int count=0;
    
    while (1) {
        int len = sizeof(root_servaddr);
        int n = recvfrom(sockfd, request, 100, 0, (struct sockaddr *) &root_servaddr, &len);
        request[n] = '\0';

       
        int flag=0;
        for(int index=0;index<count;index++){
        	if(strcmp(request,storeit[index].domain)==0){
        		strcpy(response,storeit[index].ipofit);
        		printf("Returned from tld cache\n");
        		flag=1;
        		break;
        	}
        }
        if(flag==0){
        
                char tempreq[200],d1[100],d2[100];
                strcpy(tempreq,request);
             	const char ss[4] = ".";
                char *tok = strtok(tempreq,ss);
                strcpy(d2,tok);
                tok = strtok(0,ss);
        	char temp[100];
        	while(tok!=0){
        		strcpy(d1,d2);
        		strcpy(d2,tok);
        		tok = strtok(0,ss);
        	}
        	strcat(d1,".");
        	strcat(d1,d2);
        	char authdns[100];int f1=0;
        	int ww;
        	//printf("d1:%s\n",d1);
        	for(ww=0;ww<2;ww++){
        		if(strcmp(d1,ns1[ww].name)==0){
        			strcpy(authdns,ns1[ww].value);
        			f1=1;
        			break;
        		}
        	}
        	if(f1==1){
        		//printf("authdns:%s\n",authdns);
        		for(ww=0;ww<2;ww++){
        			if(strcmp(authdns,a1[ww].name)==0){
        				 printf("Request sent to %s\n",a1[ww].name);
        				 authaddr.sin_addr.s_addr = inet_addr(a1[ww].value);
        				 break;
        			}
        		}
        	}
        	
 
             int lenn = sizeof(authaddr);
       	     if(sendto(sockfd, request, strlen(request), 0, (struct sockaddr *) &authaddr, lenn) < 0) {
            	perror("Could not send data\n");
            	exit(1);
        	}
             int nn = recvfrom(sockfd, response, 100, 0, (struct sockaddr *) &authaddr, &lenn);
             response[nn]='\0';	
             strcpy(storeit[count].domain,request);
             strcpy(storeit[count].ipofit,response);
             count++;
        }
        

        if (sendto(sockfd, response, strlen(response), 0, (struct sockaddr *) &root_servaddr, len) < 0) {
            perror("Could not send data\n");
            exit(1);
        }
    }

    close(sockfd);
    return 0;
}
