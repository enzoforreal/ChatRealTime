#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
///#include <gtk/gtk.h>

#define PORT 8080
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024 + 20 + 1 

int clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg)
{
    int client_socket = *((int *)arg);

    char buffer[BUFFER_SIZE];
    int received, sent;

    char username[20];
    username[received] = '\0';
    buffer[20] = ':';

    //Add the username to the message buffer
    memcpy(buffer,username,20);

    int exit_flag = 0 ;

    // Get username from client
    received = recv(client_socket, username, 20, 0);
    if (received < 0)
    {
        perror("Failed to receive username from client");
        exit_flag = 1 ;
    }
    while(!exit_flag)
    {
        // Receive message from client
        received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (received < 0)
        {
            perror("Failed to receive data from client");
            exit_flag = 1 ;
        }
        else if (received == 0)
        {
            printf("Client disconnected\n");
            exit_flag = 1 ;
        }

        strcat(buffer,"\n");

        // Broadcast message to all clients
        if(!exit_flag){
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < client_count; i++)
        {
            if (clients[i] != client_socket)
            {
                sent = send(clients[i], buffer, received, 0);
                if (sent < 0)
                {
                    perror("Failed to send data to client");
                }
            }
        }
            pthread_mutex_unlock(&clients_mutex);
       }
    }

    // Remove client from list and close socket
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++)
    {
        if (clients[i] == client_socket)
        {
            clients[i] = clients[client_count - 1];
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    close(client_socket);

    return NULL;
}

int main(int argc, char const *argv[])
{

    gtk_init(&argc, &argv);
    GtkWidget *window;
    GtkWidget *button;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    button = gtk_button_new_with_label("Hello World!");
    gtk_widget_show_all(window);
    gtk_main();

    gtk_container_add(GTK_CONTAINER(window), button);
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_size;
    pthread_t thread;

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Failed to create socket");
        return 1;
    }

    // Set server address information
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Failed to bind socket");
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) < 0)
    {
        perror("Failed to listen for incoming connections");
        return 1;
    }

    printf("Server is running on port %d\n", PORT);

    while (1)
    {
        // Accept incoming connection
        client_address_size = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_size);
        if (client_socket < 0)
        {
            perror("Failed to accept incoming connection");
            continue;
        }

        // Add client to list and start handling it
        pthread_mutex_lock(&clients_mutex);
        if (client_count < MAX_CLIENTS)
        {
            clients[client_count++] = client_socket;
            pthread_create(&thread, NULL, handle_client, &client_socket);
        }
        else
        {
            close(client_socket);
        }
        pthread_mutex_unlock(&clients_mutex);
    }

    close(server_socket);
    return 0;
}