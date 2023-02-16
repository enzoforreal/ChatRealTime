// ficher server_socket.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "includes/window.h"

#define PORT 8080
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

const int WIDTH = 800, HEIGHT = 600;

int clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg)
{
    ClientThreadArgs *args = (ClientThreadArgs *)arg;

    int client_socket = args->client_socket;
    Window *window = args->window;

    char buffer[BUFFER_SIZE] = {0};
    char temp_buffer[BUFFER_SIZE];
    int received, sent;

    char username[20] = {0};

    char request_username[] = "Please enter your username: ";
    sent = send(client_socket, request_username, sizeof(request_username), 0);
    if (sent < 0)
    {
        perror("Failed to request username from client");
        close(client_socket);
        return NULL;
    }

    received = recv(client_socket, username, 20, 0);
    if (received < 0)
    {
        perror("Failed to receive username from client");
        close(client_socket);
        return NULL;
    }

    while (1)
    {
        // Receive message from client
        received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (received < 0)
        {
            perror("Failed to receive data from client");
            break;
        }
        else if (received == 0)
        {
            printf("Client disconnected\n");
            break;
        }

        // Add username to message buffer
        snprintf(temp_buffer, BUFFER_SIZE, "%s: %.*s", username, (int)BUFFER_SIZE - 20 - 2, buffer);
        strcpy(buffer, temp_buffer);

        // Print message to server terminal
        printf("Received message from client %s: %s\n", username, buffer);

        // Broadcast message to all clients

        pthread_mutex_lock(&clients_mutex);

        char messages[MAX_CLIENTS][BUFFER_SIZE] = {0};

        for (int i = 0; i < client_count; i++)
        {
            if (clients[i] != client_socket)
            {
                // Check if username exists in messages array
                int found = 0;
                for (int j = 0; j < client_count; j++)
                {
                    if (strcmp(messages[j], username) == 0)
                    {
                        found = 1;
                        break;
                    }
                }
                if (!found)
                {
                    // Store username in messages array
                    strcpy(messages[i], username);
                }

                // Add username to message buffer
                snprintf(temp_buffer, BUFFER_SIZE, "%s: %.*s", username, (int)BUFFER_SIZE - 20 - 2, buffer);
                strcpy(buffer, temp_buffer);

                sent = send(clients[i], buffer, strlen(buffer), 0);
                if (sent < 0)
                {
                    perror("Failed to send data to client");
                }
            }
        }

        pthread_mutex_unlock(&clients_mutex);

        // Update SDL window with messages
        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Surface *textSurface = NULL;
        SDL_Texture *textTexture = NULL;
        SDL_Rect textRect = {10, 10, 0, 0};
        SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);

        for (int i = 0; i < client_count; i++)
        {
            if (strlen(messages[i]) > 0)
            {
                textSurface = TTF_RenderText_Solid(window->font, messages[i], textColor);
                textTexture = SDL_CreateTextureFromSurface(window->renderer, textSurface);
                SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
                SDL_RenderCopy(window->renderer, textTexture, NULL, &textRect);
                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);
                textRect.y += textRect.h + 10;
            }
        }
    }

    SDL_RenderPresent(window->renderer);

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

int main(int argc, char *argv[])
{
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

    // Create SDL window
    Window *window = CreateWindow();

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

        // Create client thread args
        ClientThreadArgs *args = (ClientThreadArgs *)malloc(sizeof(ClientThreadArgs));
        args->client_socket = client_socket;
        args->window = window;

        // Add client to list and start handling it
        pthread_mutex_lock(&clients_mutex);
        if (client_count < MAX_CLIENTS)
        {
            clients[client_count++] = client_socket;
            pthread_create(&thread, NULL, handle_client, args);
        }
        else
        {
            close(client_socket);
            free(args);
        }
        pthread_mutex_unlock(&clients_mutex);
    }

    // Clean up
    close(server_socket);
    DestroyWindow(window);
    SDL_Quit();
    return 0;
}
