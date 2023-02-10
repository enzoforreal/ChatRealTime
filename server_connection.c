#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define MAX_CLIENTS 5

struct client_info
{
    int socket;
    struct sockaddr_in address;
};

void *handle_client(void *arg)
{
    struct client_info *client = (struct client_info *)arg;
    char buffer[1024];
    int len;

    // Recevoir des données du client
    len = recv(client->socket, buffer, sizeof(buffer), 0);
    if (len < 0)
    {
        perror("recv");
        exit(1);
    }

    // Envoyer des données au client
    send(client->socket, buffer, len, 0);

    // Fermer la connexion avec le client
    close(client->socket);

    // Libérer la mémoire allouée pour le client
    free(client);

    return NULL;
}

int server_connection(void)
{
    int server_socket;
    struct sockaddr_in server_address;
    int opt = 1;
    int client_socket;
    struct sockaddr_in client_address;
    socklen_t client_len;
    pthread_t thread;

    // Créer une socket pour le serveur
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("socket");
        exit(1);
    }

    // Configurer les options de la socket
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configurer les informations d'adresse pour le serveur
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Lier la socket au port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("bind");
        exit(1);
    }

    // Écouter les connexions de clients
    if (listen(server_socket, MAX_CLIENTS) < 0)
    {
        perror("listen");
        exit(1);
    }

    printf("Server is running on port %d\n", PORT);

    while (1)
    {
        // Accepter une connexion de client
        client_len = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
        if (client_socket < 0)
        {
            perror("accept");
            exit(1);
        }

        printf("Accepted connection from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        // Allouer de la mémoire pour le client
        struct client_info *client = (struct client_info *)malloc(sizeof(struct client_info));
        if (client == NULL)
        {
            perror("malloc");
            exit(1);
        }

        // Stocker les informations du client
        client->socket = client_socket;
        memcpy(&client->address, &client_address, sizeof(client_address));

        // Créer un nouveau thread pour le client
        if (pthread_create(&thread, NULL, handle_client, (void *)client) != 0)
        {
            perror("pthread_create");
            exit(1);
        }

        // Détacher le thread pour libérer les ressources associées au thread
        pthread_detach(thread);
    }

    // Fermer la socket du serveur
    close(server_socket);

    return 0;
}
