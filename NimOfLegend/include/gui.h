/* GUI minimal pour NimOfLegend
 * Fournit une petite interface didactique en GTK3.
 */
#ifndef GUI_H
#define GUI_H

/* Lance la GUI en mode démo (sans message réseau) */
int run_gui(void);

/* Lance la GUI en lui passant un pointeur vers GameMessage (peut être NULL).
 * Le paramètre est pris comme `void*` pour éviter d'inclure `protocol.h`
 * partout ; `src/gui.c` inclut `protocol.h` et caste le pointeur.
 */
int run_gui_with_message(void *msg);

#endif /* GUI_H */
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
