#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "utils.h"
#include "protocol.h"

int main() {
    int port = 8080;
    int server_sock = init_server_connection(port);

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    printf("[SERVEUR] En attente...\n");

    // Accepter un client
    int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);
    if (client_sock < 0) {
        perror("Accept error");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("[SERVEUR] Client connecte !\n");

    // Préparer le message
    GameMessage msg;
    memset(&msg, 0, sizeof(msg)); // Nettoyer la mémoire
    
    msg.type = MSG_CONNECT;
    strncpy(msg.message, "Bienvenue dans le jeu CoinStrip !", 127);
    
    // Exemple : on place une pièce fictive pour tester
    msg.board.nb_coins = 1;
    msg.board.coins[0] = 1; 

    // Envoyer la structure
    send(client_sock, &msg, sizeof(msg), 0);
    printf("[SERVEUR] Structure envoyee.\n");

    // Fermeture
    close(client_sock);
    close(server_sock);
    return 0;
}