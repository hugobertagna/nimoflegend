CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
OBJ_DIR = obj
BIN_DIR = bin
SRC_DIR = src

# Flags pour la compilation de l'interface GTK (utilise pkg-config)
GTK_CFLAGS := $(shell pkg-config --cflags gtk+-3.0 2>/dev/null)
GTK_LIBS := $(shell pkg-config --libs gtk+-3.0 2>/dev/null)

# Crée les dossiers s'ils n'existent pas
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))

all: $(BIN_DIR)/server $(BIN_DIR)/client

# Cible optionnelle pour construire seulement la GUI (ne modifie pas le client)
gui: $(BIN_DIR)/gui

# Compilation du Serveur
$(BIN_DIR)/server: $(OBJ_DIR)/server.o $(OBJ_DIR)/utils.o
	$(CC) $(CFLAGS) -o $@ $^

# Compilation du Client
$(BIN_DIR)/client: $(OBJ_DIR)/client.o $(OBJ_DIR)/utils.o
	$(CC) $(CFLAGS) -o $@ $^

# Compilation générique des .c en .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Règle pour compiler l'objet GUI avec les flags pkg-config
$(OBJ_DIR)/gui.o: $(SRC_DIR)/gui.c
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -c -o $@ $<

# Binaire GUI autonome (lié avec GTK) ; dépend de pkg-config present
$(BIN_DIR)/gui: $(OBJ_DIR)/gui.o
	$(CC) $(CFLAGS) -o $@ $^ $(GTK_LIBS) -lm

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)