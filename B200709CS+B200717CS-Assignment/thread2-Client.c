#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>


#define PAYLOAD_SIZE 1032

typedef struct {
    int type;
    int seq_num;
    char payload[1024];
    unsigned int checksum;
} packet_t;

void send_packet(int type, int sockfd, struct sockaddr_in *servaddr) {
    static int seq_num = 0;

    // Create packet
    packet_t packet;
    packet.type = type;
    packet.seq_num = seq_num++;

    for (int i = 0; i < 1024; i++) {
        packet.payload[i] = (char) ('A' + rand() % 26);
    }

    // calculate checksum
    unsigned int checksum = 0;
    for (int i = 0; i < sizeof(packet_t)-sizeof(unsigned int); i++) {
        checksum ^= *(((unsigned char *) &packet) + i);
    }
    packet.checksum = checksum;

    // Send packet
    sendto(sockfd, &packet, sizeof(packet), 0,
           (struct sockaddr *)servaddr, sizeof(*servaddr));
}

void *send_packet_1_thread(void *arg) {
    int sockfd = *(int *)arg;

    // Set up server address
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(12345);

    while (1) {
        send_packet(1, sockfd, &servaddr);

        // Wait 100ms before sending next packet
        usleep(100000);
    }

    return NULL;
}

void *send_packet_2_thread(void *arg) {
    int sockfd = *(int *)arg;

    // Set up server address
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(12345);

    while (1) {
        send_packet(2, sockfd, &servaddr);

        // Wait 150ms before sending next packet
        usleep(150000);
    }

    return NULL;
}

int main() {
    // Set up UDP socket for sending
    printf("Starting client\n\n");
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    printf("Socket created successfully\n\n");
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, send_packet_1_thread, &sockfd);
    printf("Packet 1 thread created successfully\n\n");
    pthread_create(&tid2, NULL, send_packet_2_thread, &sockfd);
    printf("Packet 2 thread created successfully\n\n");

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return 0;
}