#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h> // Pour gérer plusieurs clients en même temps

#define PORT 2301
#define MAX_CLIENTS 100

// --- VARIABLES GLOBALES ---
int clients[MAX_CLIENTS]; // Tableau pour stocker tous les clients connectés
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER; // Protection pour éviter les bugs (Race conditions)

void end_if_error(bool error_condition, char *msg) {
    if (error_condition) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

// --- FONCTION DE DIFFUSION (BROADCAST) ---
// Envoie un message reçu à tout le monde, sauf à celui qui l'a envoyé
void broadcast_message(char *message, int sender_fd, int size) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != 0 && clients[i] != sender_fd) {
            write(clients[i], message, size);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// --- GESTION D'UN CLIENT (THREAD) ---
void *handle_client(void *arg) {
    int sock = *(int*)arg;
    free(arg);
    char buf[1024];
    int n;

    printf("[Serveur] Nouveau thread lancé pour le socket %d\n", sock);

    // Boucle : on lit les messages de ce client tant qu'il est connecté
    while ((n = read(sock, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("[Chat] Message reçu de %d : %s\n", sock, buf);
        
        // On renvoie le message à tous les autres
        broadcast_message(buf, sock, n);
    }

    // Si on arrive ici, le client s'est déconnecté
    printf("[Serveur] Client %d déconnecté\n", sock);
    close(sock);

    // On le retire du tableau des clients
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == sock) {
            clients[i] = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    return NULL;
}

// --- MAIN PRINCIPAL ---
int main (int argc, char *argv[]) {
    int s, ret, yes = 1;
    socklen_t len;
    struct sockaddr_in server_addr, client_addr;

    // Initialisation du tableau des clients
    for (int i = 0; i < MAX_CLIENTS; i++) clients[i] = 0;

    // Création du socket d'écoute
    s = socket(AF_INET, SOCK_STREAM, 0);
    end_if_error(s == -1, "socket");

    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(s, (struct sockaddr *)&server_addr, sizeof(server_addr));
    end_if_error(ret == -1, "bind");

    ret = listen(s, 10);
    end_if_error(ret == -1, "listen");

    printf("Serveur de Chat prêt sur le port %d...\n", PORT);

    // BOUCLE INFINIE : On attend les clients
    while (1) {
        len = sizeof(client_addr);
        int *new_sock = malloc(sizeof(int));
        *new_sock = accept(s, (struct sockaddr *)&client_addr, &len);
        
        if (*new_sock < 0) {
            free(new_sock);
            continue;
        }

        // Ajouter le nouveau client au tableau
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] == 0) {
                clients[i] = *new_sock;
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);

        // Lancer un thread pour s'occuper de ce client
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, new_sock);
        pthread_detach(tid); // Le thread se nettoie tout seul à la fin
    }

    return EXIT_SUCCESS;
}
