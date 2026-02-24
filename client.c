#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>

#define PORT 2301
#define SERVER "127.0.0.1"

void end_if_error(bool error_condition, char *msg) {
    if (error_condition) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

// --- THREAD D'ÉCOUTE ---
// Ce thread attend les messages du serveur et les affiche
void *receive_messages(void *arg) {
    int s = *(int*)arg;
    char buf[1024];
    int n;

    while ((n = read(s, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("\n[Message reçu] : %s\n> ", buf);
        fflush(stdout);
    }
    
    printf("\n[Serveur] Déconnecté.\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    int s, ret;
    struct sockaddr_in server_addr;
    char buf[1024];

    // Création du socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    end_if_error(s == -1, "socket");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER);

    // Connexion au serveur
    ret = connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr));
    end_if_error(ret == -1, "connect");

    printf("Connecté au chat ! Tapez votre message :\n");

    // Lancement du thread qui va écouter les autres
    pthread_t tid;
    pthread_create(&tid, NULL, receive_messages, &s);

    // BOUCLE PRINCIPALE : Lecture clavier et envoi
    while (1) {
        printf("> ");
        fflush(stdout);
        if (fgets(buf, sizeof(buf), stdin) != NULL) {
            // Envoi du message au serveur
            write(s, buf, strlen(buf));
        }
    }

    close(s);
    return EXIT_SUCCESS;
}
