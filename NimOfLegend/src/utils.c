#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "utils.h"

int init_server_connection(int port) {
    int socket_fd;
    struct sockaddr_in server_addr;

    // 1. Création socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Erreur creation socket");
        exit(EXIT_FAILURE);
    }

    // Option pour relancer le serveur rapidement
    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. Configuration adresse
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // 3. Bind
    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur bind");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    // 4. Listen
    if (listen(socket_fd, 5) < 0) {
        perror("Erreur listen");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("[UTILS] Serveur demarre sur le port %d\n", port);
    return socket_fd;
}

int connect_to_server(const char *ip, int port) {
    int socket_fd;
    struct sockaddr_in server_addr;

    // 1. Création socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Erreur creation socket client");
        exit(EXIT_FAILURE);
    }

    // 2. Configuration
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // Conversion IP
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("Adresse IP invalide");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    // 3. Connexion
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Echec connexion");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("[UTILS] Connecte au serveur %s:%d\n", ip, port);
    return socket_fd;
}