#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>

#define PAYLOAD_SIZE 1032

typedef struct
{
    int type;
    int seq_num;
    char payload[1024];
    unsigned int checksum;
} packet_t;

int num_packets_1 = 0;
int num_packets_2 = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

packet_t packet_buffer[1024];
int buffer_size = 0;

void *receive_thread(void *arg)
{
    int sockfd = *(int *)arg;

    while (1)
    {
        // Receive packet
        packet_t packet;
        bzero(&packet, sizeof(packet));
        recvfrom(sockfd, &packet, sizeof(packet), 0, NULL, NULL);

        // Check for errors
        unsigned int checksum = 0;
        for (int i = 0; i < sizeof(packet_t)-sizeof(unsigned int); i++)
        {
            checksum ^= *(((unsigned char *)&packet) + i);
        }
        if (checksum != packet.checksum)
        {
            printf("Checksums do not match! DROP the packet\n");
            continue;
        }
        // printf("Checksum match\n");
        // Add packet to buffer
        pthread_mutex_lock(&mutex);
        packet_buffer[buffer_size++] = packet;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void *process_packet_1_thread(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        while (buffer_size == 0)
        {
            pthread_cond_wait(&cond, &mutex);
        }

        // Find a packet of type 1 in the buffer
        int i;
        for (i = 0; i < buffer_size; i++)
        {
            if (packet_buffer[i].type == 1)
            {
                printf("#########################################################\n");
                printf("Type = %d  Seq No = %d\n%s\n\n",packet_buffer[i].type,packet_buffer[i].seq_num,packet_buffer[i].payload);
                printf("#########################################################\n");
                break;
            }
        }
        // printf("Type = %d  Seq No = %d\n%s\n\n",packet_buffer[i].type,packet_buffer[i].seq_num,packet_buffer[i].payload);

        if (i < buffer_size)
        {
            // Process packet of type 1
            num_packets_1++;

            // Remove packet from buffer
            memmove(&packet_buffer[i], &packet_buffer[i + 1],
                    (buffer_size - i - 1) * sizeof(packet_t));
            buffer_size--;
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void *process_packet_2_thread(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        while (buffer_size == 0)
        {
            pthread_cond_wait(&cond, &mutex);
        }

        // Find a packet of type 2 in the buffer
        int i;
        for (i = 0; i < buffer_size; i++)
        {
            if (packet_buffer[i].type == 2)
            {
                printf("#########################################################\n");
                printf("Type = %d  Seq No = %d\n%s\n\n",packet_buffer[i].type,packet_buffer[i].seq_num,packet_buffer[i].payload);
                printf("#########################################################\n");
                break;
            }
        }

        if (i < buffer_size)
        {
            // Process packet of type 2
            num_packets_2++;

            // Remove packet from buffer
            memmove(&packet_buffer[i], &packet_buffer[i + 1],
                    (buffer_size - i - 1) * sizeof(packet_t));
            buffer_size--;
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void *print_stats_thread(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        //if (num_packets_1 != 0 && num_packets_2 != 0)
        //{
            printf("Number of packets of type 1 received: %d\n", num_packets_1);
            printf("Number of packets of type 2 received: %d\n\n", num_packets_2);
        //}
        pthread_mutex_unlock(&mutex);

        // Wait 300ms before printing again
        usleep(300000);
    }

    return NULL;
}

int main()
{
    // Set up UDP socket for receiving
    printf("Starting server...\n\n");
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    printf("Socket created...\n\n");
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(12345);
    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Socket bind successful...\n\n");

    pthread_t tid1, tid2, tid3, tid4;
    pthread_create(&tid1, NULL, receive_thread, &sockfd);
    printf("Receive thread started...\n\n");
    pthread_create(&tid2, NULL, process_packet_1_thread, NULL);
    printf("Process packet 1 thread started...\n\n");
    pthread_create(&tid3, NULL, process_packet_2_thread, NULL);
    printf("Process packet 2 thread started...\n\n");
    pthread_create(&tid4, NULL, print_stats_thread, NULL);
    printf("Print stats thread started...\n\n");

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    pthread_join(tid4, NULL);

    return 0;
}