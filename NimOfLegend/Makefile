CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
OBJ_DIR = obj
BIN_DIR = bin
SRC_DIR = src

# Crée les dossiers s'ils n'existent pas
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))

all: $(BIN_DIR)/server $(BIN_DIR)/client

# Compilation du Serveur
$(BIN_DIR)/server: $(OBJ_DIR)/server.o $(OBJ_DIR)/utils.o
	$(CC) $(CFLAGS) -o $@ $^

# Compilation du Client
$(BIN_DIR)/client: $(OBJ_DIR)/client.o $(OBJ_DIR)/utils.o
	$(CC) $(CFLAGS) -o $@ $^

# Compilation générique des .c en .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)