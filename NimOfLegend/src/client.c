#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h> // <--- C'est la ligne qui manquait !
#include "utils.h"
#include "protocol.h"
#include "gui.h"

int main() {
    // Connexion locale
    int sock = connect_to_server("127.0.0.1", 8080);

    GameMessage msg;
    
    // Réception de la structure
    // On lit exactement la taille de la structure
    int bytes_received = recv(sock, &msg, sizeof(msg), 0);

    if (bytes_received > 0) {
        printf("[CLIENT] Message recu du serveur :\n");
        printf("   - Type : %d\n", msg.type);
        printf("   - Texte : %s\n", msg.message);
        printf("   - Nb Pieces : %d\n", msg.board.nb_coins);

        /* Lancer la GUI pour afficher le message et l'état du plateau */
        run_gui_with_message(&msg);
    } else {
        printf("[CLIENT] Erreur de reception ou deconnexion.\n");
    }

    close(sock);
    return 0;
}