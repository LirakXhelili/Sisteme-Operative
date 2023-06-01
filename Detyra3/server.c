#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 5002
#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 256
#define MAX_RSP_SIZE 275
#define DISCONNECT_MSG "disconnect"
#define CLIENT_MSG "hello server"
#define SERVER_MSG "hello client"

typedef struct {
    int id;
    int socket;
} Client;

typedef struct {
    char message[MAX_RSP_SIZE];
    Client *client;
} Message;

Client *clients[MAX_CLIENTS];
Message *messageList[MAX_CLIENTS];
int client_count = 0;
int message_count = 0;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t message_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t message_cond = PTHREAD_COND_INITIALIZER;


void *client_thread(void *arg) {
    Client *client = (Client *)arg;
    char buffer[MAX_MSG_SIZE];
    char response[MAX_RSP_SIZE];
    while (1) {
        ssize_t bytes_read = recv(client->socket, buffer, MAX_MSG_SIZE - 1, 0);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Received: %s\n", buffer);
            if (strcmp(buffer, DISCONNECT_MSG) == 0) {
            printf("Client %d disconnected\n", client->id);
                break;
            }
            if (strcmp(buffer, CLIENT_MSG) == 0) {
                strcpy(buffer, SERVER_MSG);
            }
            // Process the message here
            pthread_mutex_lock(&message_mutex);
                if (message_count < MAX_CLIENTS) {
                    //strcpy(response, buffer);
                    sprintf(response, "Server response-> %s", buffer);
                    Message *message = malloc(sizeof(Message));
                    strcpy(message->message, response);
                    message->client = client;
                    messageList[message_count++] = message;
                    pthread_cond_signal(&message_cond);
                }
            pthread_mutex_unlock(&message_mutex);
         } else if (bytes_read == 0) {
            printf("Client %d disconnected\n", client->id);
            break;
        } else {
            perror("Error receiving message");
            break;
        }
    }
    close(client->socket);
    return NULL;
}

void *message_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&message_mutex);
        while (message_count == 0) {
            pthread_cond_wait(&message_cond, &message_mutex);
        }
        Message *message = messageList[--message_count];
        send(message->client->socket, message->message, strlen(message->message), 0);
        free(message);
        pthread_mutex_unlock(&message_mutex);
    }
   
return NULL;
}

void add_client(Client *client) {
    pthread_mutex_lock(&client_mutex);
    if (client_count < MAX_CLIENTS) {
        clients[client_count++] = client;
    }
    pthread_mutex_unlock(&client_mutex);
    printf("Client %d has joined\n", client->id);
}

void remove_client(Client *client) {
    pthread_mutex_lock(&client_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i] == client) {
            clients[i] = clients[--client_count];
            break;
        }
    }
    pthread_mutex_unlock(&client_mutex);
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
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        return 1;
    }
    printf("-------%s-------\n", "SERVER HAS STARTED");
   
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Error listening on socket");
        return 1
;
    }

    pthread_t message_thread_id;
    pthread_create(&message_thread_id, NULL, message_thread, NULL);

   
while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Error accepting client connection");
            continue;
        }
       
        Client *client = malloc(sizeof(Client));
        client->id = client_count;
        client->socket = client_socket;
        add_client(client);

        pthread_t thread;
        pthread_create(&thread, NULL, client_thread, client);
    }

    close(server_socket);

    return 0;
}
