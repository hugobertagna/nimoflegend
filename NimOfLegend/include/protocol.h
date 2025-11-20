#ifndef PROTOCOL_H
#define PROTOCOL_H

#define STRIP_SIZE 20
#define MAX_NAME 32

// Les différents types de messages possibles
typedef enum {
    MSG_CONNECT,    // Connexion initiale
    MSG_WAIT,       // Demande d'attente
    MSG_YOUR_TURN,  // C'est au joueur de jouer
    MSG_MOVE,       // Envoi d'un mouvement
    MSG_GAME_OVER,  // Fin de partie
    MSG_ERROR       // Erreur
} MessageType;

// Structure simple représentant le plateau (pour l'instant)
typedef struct {
    int coins[STRIP_SIZE];
    int nb_coins;
} GameState;

// LE MESSAGE QUI CIRCULE SUR LE RÉSEAU
typedef struct {
    MessageType type;             // Quel est le but de ce message ?
    char player_name[MAX_NAME];   // Nom du joueur (optionnel)
    char message[128];            // Petit message texte (ex: "Bienvenue")
    GameState board;              // L'état du jeu
    int move_src;                 // Mouvement : Départ
    int move_dest;                // Mouvement : Arrivée
} GameMessage;

#endif