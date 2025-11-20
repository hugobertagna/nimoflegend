/*
 * Interface publique pour la GUI
 * Fournit une fonction simple pour lancer l'application graphique
 */
#ifndef GUI_H
#define GUI_H

/* Lance la GUI.
 * Retourne 0 si ok, non-zero en cas d'erreur.
 */
int run_gui(void);

/* Lance la GUI en lui fournissant un message de jeu reçu du réseau.
 * Si `msg` est NULL, la GUI démarrera en mode démo par défaut.
 */
int run_gui_with_message(void *msg);

#endif /* GUI_H */
