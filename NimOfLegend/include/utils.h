#ifndef UTILS_H
#define UTILS_H

// Prépare le serveur (socket -> bind -> listen)
int init_server_connection(int port);

// Connecte le client (socket -> connect)
int connect_to_server(const char *ip, int port);

#endif