#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024
#define DNS_PORT 8080

int main(int argc, char const *argv[]) {
    int sockfd;
    struct sockaddr_in servaddr;
    char domain_name[100];
    char response[BUFF_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Could not create socket\n");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(DNS_PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter domain name: ");
    scanf("%s", domain_name);

    if (sendto(sockfd, domain_name, strlen(domain_name), 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Could not send data\n");
        exit(1);
    }

    int n = recvfrom(sockfd, response, BUFF_SIZE, 0, NULL, NULL);
    response[n] = '\0';

    printf("DNS Response: %s\n", response);

    close(sockfd);
    return 0;
}
