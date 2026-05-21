#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include "../include/auth.h"
#include "../include/mail.h"

#define PORT 2525
#define BUFFER_SIZE 4096
#define MAX_CLIENTS 5

int active_clients = 0;
int server_fd;
int client_socket = -1;


/* Graceful shutdown handler */
void shutdown_server(int sig)
{
    printf("\n[SERVER] Shutting down...\n");

    if(client_socket != -1)
    {
        char *msg = "421 Server shutting down\r\nServer Shutted";
        send(client_socket, msg, strlen(msg), 0);
        close(client_socket);
    }

    close(server_fd);
    exit(0);
}


void handle_client(int client) {

    char buffer[BUFFER_SIZE];
    char sender[100] = "";
    char receiver[100] = "";
    char data[BUFFER_SIZE] = "";
    char current_user[100] = "";
    int authenticated = 0;

    printf("[NEW CONNECTION]\n");
    fflush(stdout);

    send(client, "+OK Custom SMTP Server Ready\n", 31, 0);

    while (1) {

        memset(buffer, 0, sizeof(buffer));
        int len = recv(client, buffer, sizeof(buffer) - 1, 0);

        if (len <= 0)
            break;

        buffer[strcspn(buffer, "\r\n")] = 0;

        if (strncmp(buffer, "REGISTER", 8) == 0) {

            char username[100], password[100];

            if (sscanf(buffer, "REGISTER %s %s", username, password) != 2) {
                send(client, "-ERR Invalid REGISTER format\n", 29, 0);
                continue;
            }

            if (register_user(username, password))
                send(client, "+OK Registered Successfully\n", 29, 0);
            else
                send(client, "-ERR Registration Failed\n", 25, 0);
        }

        else if (strncmp(buffer, "AUTH", 4) == 0) {

            char username[100], password[100];

            if (sscanf(buffer, "AUTH %s %s", username, password) != 2) {
                send(client, "-ERR Invalid AUTH format\n", 24, 0);
                continue;
            }

            if (verify_user(username, password)) {

                authenticated = 1;
                strcpy(current_user, username);

                printf("[LOGIN] %s logged in\n", username);
                fflush(stdout);

                send(client, "+OK Authentication Successful\n", 30, 0);
            }
            else {
                send(client, "-ERR Authentication Failed\n", 27, 0);
            }
        }

        else if (strncmp(buffer, "MAIL FROM:", 10) == 0) {

            if (!authenticated) {
                send(client, "-ERR Authenticate First\n", 24, 0);
                continue;
            }

            sscanf(buffer, "MAIL FROM: %s", sender);
            send(client, "+OK Sender OK\n", 14, 0);
        }

        else if (strncmp(buffer, "RCPT TO:", 8) == 0) {

            sscanf(buffer, "RCPT TO: %s", receiver);

            if (!user_exists(receiver)) {
                send(client, "-ERR User does not exist\n", 25, 0);
                continue;
            }

            send(client, "+OK Recipient OK\n", 17, 0);
        }

        else if (strcmp(buffer, "DATA") == 0) {

            send(client, "354 End with . on new line\n", 27, 0);

            memset(data, 0, sizeof(data));

            while (1) {

                memset(buffer, 0, sizeof(buffer));
                recv(client, buffer, sizeof(buffer) - 1, 0);

                if (strcmp(buffer, ".\r\n") == 0 ||
                    strcmp(buffer, ".\n") == 0)
                    break;

                strcat(data, buffer);
            }

            save_mail(current_user, receiver, data);

            send(client, "+OK Mail Saved\n", 15, 0);
        }

        else if (strcmp(buffer, "QUIT") == 0) {

            send(client, "+OK Goodbye\n", 12, 0);
            break;
        }

        else {

            send(client, "-ERR Unknown Command\n", 21, 0);
        }
    }

    if(strlen(current_user) > 0)
    {
        printf("[LOGOUT] %s disconnected\n", current_user);
        fflush(stdout);
    }

    close(client);
}


int main() {

    int client;
    struct sockaddr_in server, client_addr;
    socklen_t addrlen = sizeof(client_addr);

    signal(SIGINT, shutdown_server);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&server, sizeof(server));
    listen(server_fd, 5);

    printf("SMTP Server running on port %d...\n", PORT);

    while (1) {

        client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
        client = client_socket;

        if (active_clients >= MAX_CLIENTS) {

            char *msg = "421 Server is full\r\n";

            send(client, msg, strlen(msg), 0);
            shutdown(client, SHUT_RDWR);
            close(client);

            continue;
        }

        active_clients++;

        printf("[CLIENTS] Active connections: %d\n", active_clients);
        fflush(stdout);

        handle_client(client);

        active_clients--;

        printf("[CLIENTS] Active connections: %d\n", active_clients);
        fflush(stdout);
    }

    return 0;
}
