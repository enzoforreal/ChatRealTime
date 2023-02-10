#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "client_connection.h"

#define PORT 12345

void client_connection(void)
{
    int client_socket;
    struct sockaddr_in server_address;

    // Create the client socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));

    // Assign IP and port
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address,
                sizeof(server_address)) < 0)
    {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    // Send the message to the server
    const char *message = "Hello from client";
    send(client_socket, message, strlen(message), 0);

    // Close the socket
    close(client_socket);
}
