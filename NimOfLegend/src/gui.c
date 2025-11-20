#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "gui.h"
#include "protocol.h"

/* GUI très simple: un cercle (joueur) et plusieurs carrés (destinations).
 * - Cliquer sur le cercle pour le sélectionner.
 * - Cliquer sur un carré sélectionne la destination et lance une animation.
 *
 * Ce fichier est volontairement autonome : il peut être compilé seul
 * avec la cible `make gui`.
 */

static const int WINDOW_WIDTH = 640;
static const int WINDOW_HEIGHT = 480;
static const int SQUARE_SIZE = 60;
static const int SQUARE_MARGIN = 20;

typedef struct Point { double x, y; } Point;

typedef struct AppState {
    Point circle_pos;
    double circle_radius;
    bool selected;
    Point squares[STRIP_SIZE];
    int n_squares;
    Point dest;
    bool animating;
    GtkWidget *drawing;
} AppState;

static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    (void)widget;
    AppState *s = (AppState*)data;

    cairo_set_source_rgb(cr, 1,1,1);
    cairo_paint(cr);

    for (int i = 0; i < s->n_squares; ++i) {
        double cx = s->squares[i].x;
        double cy = s->squares[i].y;
        double half = SQUARE_SIZE / 2.0;

        cairo_set_source_rgb(cr, 0.8,0.8,0.85);
        cairo_rectangle(cr, cx-half, cy-half, SQUARE_SIZE, SQUARE_SIZE);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, 0.2,0.2,0.25);
        cairo_set_line_width(cr, 2.0);
        cairo_rectangle(cr, cx-half, cy-half, SQUARE_SIZE, SQUARE_SIZE);
        cairo_stroke(cr);
    }

    cairo_set_source_rgb(cr, 0.1,0.6,0.9);
    cairo_arc(cr, s->circle_pos.x, s->circle_pos.y, s->circle_radius, 0, 2*M_PI);
    cairo_fill(cr);

    if (s->selected) {
        cairo_set_source_rgba(cr, 0.1,0.6,0.9,0.3);
        cairo_arc(cr, s->circle_pos.x, s->circle_pos.y, s->circle_radius+8, 0, 2*M_PI);
        cairo_fill(cr);
    }

    return FALSE;
}

static bool point_in_circle(AppState *s, double x, double y) {
    double dx = x - s->circle_pos.x;
    double dy = y - s->circle_pos.y;
    return (dx*dx + dy*dy) <= (s->circle_radius * s->circle_radius);
}

static int point_in_square(AppState *s, double x, double y) {
    for (int i = 0; i < s->n_squares; ++i) {
        double cx = s->squares[i].x;
        double cy = s->squares[i].y;
        double half = SQUARE_SIZE / 2.0;
        if (x >= cx-half && x <= cx+half && y >= cy-half && y <= cy+half)
            return i;
    }
    return -1;
}

static gboolean animate_step(gpointer data) {
    AppState *s = (AppState*)data;
    if (!s->animating) return G_SOURCE_REMOVE;

    double dx = s->dest.x - s->circle_pos.x;
    double dy = s->dest.y - s->circle_pos.y;
    double dist = sqrt(dx*dx + dy*dy);

    if (dist < 1.5) {
        s->circle_pos = s->dest;
        s->animating = false;
    } else {
        double step = 6.0;
        s->circle_pos.x += dx / dist * step;
        s->circle_pos.y += dy / dist * step;
    }

    if (s->drawing) gtk_widget_queue_draw(s->drawing);
    return s->animating ? G_SOURCE_CONTINUE : G_SOURCE_REMOVE;
}

static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    AppState *s = (AppState*)data;
    double x = event->x;
    double y = event->y;

    if (point_in_circle(s, x, y)) {
        s->selected = !s->selected;
        gtk_widget_queue_draw(widget);
        return TRUE;
    }

    if (s->selected) {
        int idx = point_in_square(s, x, y);
        if (idx >= 0) {
            s->dest = s->squares[idx];
            s->animating = true;
            s->selected = false;
            g_timeout_add(16, animate_step, s);
        }
    }

    return TRUE;
}

static AppState* app_state_new(int n_squares) {
    AppState *s = g_new0(AppState, 1);
    s->circle_radius = 20.0;
    s->circle_pos.x = 80.0;
    s->circle_pos.y = WINDOW_HEIGHT / 2.0;
    s->selected = false;
    s->animating = false;
    s->drawing = NULL;

    if (n_squares < 1) n_squares = 1;
    if (n_squares > STRIP_SIZE) n_squares = STRIP_SIZE;
    s->n_squares = n_squares;

    double start_x = 220.0;
    double gap = SQUARE_SIZE + SQUARE_MARGIN;
    for (int i = 0; i < s->n_squares; ++i) {
        s->squares[i].x = start_x + i * gap;
        s->squares[i].y = WINDOW_HEIGHT / 2.0;
    }

    return s;
}

static void app_state_free(AppState *s) {
    if (!s) return;
    g_free(s);
}

int run_gui(void) {
    return run_gui_with_message(NULL);
}

int run_gui_with_message(void *msg) {
    GameMessage *gmsg = (GameMessage*)msg;

    gtk_init(NULL, NULL);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_window_set_title(GTK_WINDOW(window), "NimOfLegend - GUI");

    int n = 5;
    if (gmsg) n = gmsg->board.nb_coins;
    if (n < 1) n = 1;

    AppState *state = app_state_new(n);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *label = gtk_label_new(NULL);
    if (gmsg && gmsg->message[0]) gtk_label_set_text(GTK_LABEL(label), gmsg->message);
    else gtk_label_set_text(GTK_LABEL(label), "NimOfLegend - GUI");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 6);

    GtkWidget *drawing = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing, WINDOW_WIDTH, WINDOW_HEIGHT - 40);
    gtk_box_pack_start(GTK_BOX(vbox), drawing, TRUE, TRUE, 0);

    state->drawing = drawing;

    g_signal_connect(G_OBJECT(drawing), "draw", G_CALLBACK(on_draw), state);
    gtk_widget_add_events(drawing, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(G_OBJECT(drawing), "button-press-event", G_CALLBACK(on_button_press), state);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    app_state_free(state);
    return 0;
}
#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "gui.h"
#include "protocol.h"

/*
 * GUI très simple produit en C avec GTK3.
 * - Affiche un rond (le "joueur") et plusieurs carrés (les destinations).
 * - L'utilisateur clique sur le rond pour le sélectionner, puis clique sur
 *   un carré pour déplacer le rond vers ce carré (animation simple).
 *
 * Le but est didactique : le code est commenté en français simple.
 */

/* Configuration visuelle */
static const int WINDOW_WIDTH = 640;
static const int WINDOW_HEIGHT = 480;
static const int SQUARE_SIZE = 60;
static const int SQUARE_MARGIN = 20;

/* Position courante du rond */
typedef struct Point { double x, y; } Point;

/* Structure d'état partagée */
typedef struct AppState {
    Point circle_pos;      /* position actuelle du rond */
    double circle_radius;  /* rayon du rond */
    bool selected;         /* vrai si le rond est sélectionné */
    Point squares[STRIP_SIZE];      /* centres des carrés disponibles */
    int n_squares;
    Point dest;            /* destination en cours (pour animation) */
    bool animating;
    GtkWidget *drawing;    /* widget de dessin pour rafraîchir */
} AppState;

/* Dessine tout sur le cairo */
static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    AppState *s = (AppState*)data;

    /* Fond blanc */
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    /* Dessiner les carrés */
    for (int i = 0; i < s->n_squares; ++i) {
        double cx = s->squares[i].x;
        double cy = s->squares[i].y;
        double half = SQUARE_SIZE / 2.0;

        /* Rectangle gris clair */
        cairo_set_source_rgb(cr, 0.8, 0.8, 0.85);
        cairo_rectangle(cr, cx - half, cy - half, SQUARE_SIZE, SQUARE_SIZE);
        cairo_fill(cr);

        /* Bord foncé */
        cairo_set_source_rgb(cr, 0.2, 0.2, 0.25);
        cairo_set_line_width(cr, 2.0);
        cairo_rectangle(cr, cx - half, cy - half, SQUARE_SIZE, SQUARE_SIZE);
        cairo_stroke(cr);
    }

    /* Dessiner le cercle (joueur) */
    cairo_set_source_rgb(cr, 0.1, 0.6, 0.9);
    cairo_arc(cr, s->circle_pos.x, s->circle_pos.y, s->circle_radius, 0, 2 * M_PI);
    cairo_fill(cr);

    /* Si sélectionné, tracer un halo */
    if (s->selected) {
        cairo_set_source_rgba(cr, 0.1, 0.6, 0.9, 0.3);
        cairo_arc(cr, s->circle_pos.x, s->circle_pos.y, s->circle_radius + 8, 0, 2 * M_PI);
        cairo_fill(cr);
    }

    return FALSE;
}

/* Vérifie si le point (x,y) est dans le cercle */
static bool point_in_circle(AppState *s, double x, double y) {
    double dx = x - s->circle_pos.x;
    double dy = y - s->circle_pos.y;
    return (dx*dx + dy*dy) <= (s->circle_radius * s->circle_radius);
}

/* Vérifie si le point est dans un carré et renvoie l'indice, sinon -1 */
static int point_in_square(AppState *s, double x, double y) {
    for (int i = 0; i < s->n_squares; ++i) {
        double cx = s->squares[i].x;
        double cy = s->squares[i].y;
        double half = SQUARE_SIZE / 2.0;
        if (x >= cx - half && x <= cx + half && y >= cy - half && y <= cy + half)
            return i;
    }
    return -1;
}

/* Animation : avance la position du rond vers dest et redessine */
static gboolean animate_step(gpointer data) {
    AppState *s = (AppState*)data;
    if (!s->animating) return G_SOURCE_REMOVE;

    double dx = s->dest.x - s->circle_pos.x;
    double dy = s->dest.y - s->circle_pos.y;
    double dist = sqrt(dx*dx + dy*dy);

    if (dist < 1.5) {
        /* Arrivé */
        s->circle_pos = s->dest;
        s->animating = false;
    } else {
        /* Déplacer une fraction pour une animation fluide */
        double step = 6.0; /* pixels par tick */
        s->circle_pos.x += dx / dist * step;
        s->circle_pos.y += dy / dist * step;
    }

    /* Forcer le redraw du drawing area */
    if (s->drawing) gtk_widget_queue_draw(s->drawing);

    return s->animating ? G_SOURCE_CONTINUE : G_SOURCE_REMOVE;
}

/* Handler pour clics souris */
static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    AppState *s = (AppState*)data;
    double x = event->x;
    double y = event->y;

    /* Si on clique sur le cercle -> sélection/déselection */
    if (point_in_circle(s, x, y)) {
        s->selected = !s->selected;
        gtk_widget_queue_draw(widget);
        return TRUE;
    }

    /* Si on a sélectionné le cercle et qu'on clique sur un carré -> déplacer */
    if (s->selected) {
        int idx = point_in_square(s, x, y);
        if (idx >= 0) {
            s->dest = s->squares[idx];
            s->animating = true;
            s->selected = false; /* on désélectionne pendant l'animation */

            /* Démarrer l'animation : timer qui appelle animate_step */
            g_timeout_add(16, animate_step, s); /* ~60 FPS */
        }
    }

    return TRUE;
}

/* Création et initialisation de l'état */
/* Création et initialisation de l'état (nombre de carrés paramétrable) */
static AppState* app_state_new(int n_squares) {
    AppState *s = g_new0(AppState, 1);
    s->circle_radius = 20.0;
    s->circle_pos.x = 80.0;
    s->circle_pos.y = WINDOW_HEIGHT / 2.0;
    s->selected = false;
    s->animating = false;
    s->drawing = NULL;

    if (n_squares < 1) n_squares = 1;
    if (n_squares > STRIP_SIZE) n_squares = STRIP_SIZE;
    s->n_squares = n_squares;

    /* Centrer les carrés horizontalement dans la zone de dessin */
    double total_width = n_squares * SQUARE_SIZE + (n_squares - 1) * SQUARE_MARGIN;
    double start_x = 220.0; /* position fixe minimale */
    double gap = SQUARE_SIZE + SQUARE_MARGIN;
    for (int i = 0; i < s->n_squares; ++i) {
        s->squares[i].x = start_x + i * gap;
        s->squares[i].y = WINDOW_HEIGHT / 2.0;
    }

    return s;
}
/* Nettoyage */
static void app_state_free(AppState *s) {
    if (!s) return;
    g_free(s);
}
int run_gui(void) {
    return run_gui_with_message(NULL);
}

/* Lance la GUI en utilisant un GameMessage si fourni (opaque pointer)
 * Le paramètre `msg` est attendu comme un pointeur vers `GameMessage`.
 */
int run_gui_with_message(void *msg) {
    GameMessage *gmsg = (GameMessage*)msg;

    gtk_init(NULL, NULL);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_window_set_title(GTK_WINDOW(window), "NimOfLegend - GUI Client");

    /* Nombre de carrés basé sur le message réseau s'il est présent */
    int n = 5;
    if (gmsg) {
        n = gmsg->board.nb_coins;
    }
    if (n < 1) n = 1;

    AppState *state = app_state_new(n);

    /* Box verticale: label + drawing area */
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *label = gtk_label_new(NULL);
    if (gmsg && gmsg->message[0]) {
        gtk_label_set_text(GTK_LABEL(label), gmsg->message);
    } else {
        gtk_label_set_text(GTK_LABEL(label), "NimOfLegend - GUI Client");
    }
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 6);

    GtkWidget *drawing = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing, WINDOW_WIDTH, WINDOW_HEIGHT - 40);
    gtk_box_pack_start(GTK_BOX(vbox), drawing, TRUE, TRUE, 0);

    state->drawing = drawing;

    /* Connecter les callbacks */
    g_signal_connect(G_OBJECT(drawing), "draw", G_CALLBACK(on_draw), state);
    gtk_widget_add_events(drawing, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(G_OBJECT(drawing), "button-press-event", G_CALLBACK(on_button_press), state);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    app_state_free(state);
    return 0;
}
