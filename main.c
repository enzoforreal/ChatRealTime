// main.c
#include <stdio.h>
#include <stdlib.h>
#include "client_connection.h"
#include "server_connection.h"

int main(int argc, char *argv[])
{
    // Code pour établir une connexion client
    client_connection();

    // Code pour établir une connexion serveur
    server_connection();

    return 0;
}
