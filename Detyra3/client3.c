#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5002
#define MAX_MSG_SIZE 256
#define DISCONNECT_MSG "disconnect"

typedef struct {
    char message[MAX_MSG_SIZE];
} Message;

Message messageList[MAX_MSG_SIZE];
int message_count = 0;

pthread_t send_thread, receive_thread;
pthread_mutex_t message_mutex = PTHREAD_MUTEX_INITIALIZER;

void *send_requests(void *arg) {
    int server_socket = *(int *)arg;
    char buffer[MAX_MSG_SIZE];
    while (1) {
        // Get the request from the user
        fgets(buffer, MAX_MSG_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        if (strcmp(buffer, DISCONNECT_MSG) == 0) {
            // Send disconnect message to the server
            send(server_socket, buffer, strlen(buffer), 0);
            pthread_cancel(receive_thread);
            break;
        }
        if (send(server_socket, buffer, strlen(buffer), 0) == -1) {
            perror("Error sending message");
            break;
        }
    }
    return NULL;
}

void *receive_responses(void *arg) {
    int server_socket = *(int *)arg;
    char buffer[MAX_MSG_SIZE];
    while (1) {
        ssize_t bytes_read = recv(server_socket, buffer, MAX_MSG_SIZE - 1, 0);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            // Process the response here
            printf("Received: %s\n", buffer);
            pthread_mutex_lock(&message_mutex);
            if (message_count < MAX_MSG_SIZE) {
                strcpy(messageList[message_count].message, buffer);
                message_count++;
            }
            pthread_mutex_unlock(&message_mutex);
        } else if (bytes_read == 0) {
            printf("Disconnected\n");
            break;
        } else {
            perror("Error receiving message");
            break;
        }
    }
    return NULL;
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Error setting server IP");
        return 1;
    }

    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        return 1;
    } else {
        printf("-------%s-------\n", "JOINED SERVER");
    }

    pthread_create(&send_thread, NULL, send_requests, &server_socket);
    pthread_create(&receive_thread, NULL, receive_responses, &server_socket);

    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    // Process messages in the queue
    pthread_mutex_lock(&message_mutex);
    for (int i = 0; i < message_count; i++) {
        printf("Message in queue: %s\n", messageList[i].message);
    }
    pthread_mutex_unlock(&message_mutex);

    close(server_socket);

    return 0;
}
