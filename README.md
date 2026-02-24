# 💬 TCP Chat Application - C Socket Programming


Ce projet a été réalisé dans le cadre du module **4TIN807U Réseaux et Protocoles** (TD 6) à l'**Université de Bordeaux**. 
L'objectif est de concevoir une application de chat multi-utilisateurs en langage C, utilisant le protocole **TCP** pour garantir une communication fiable.

---

## 🚀 Fonctionnalités

* **Architecture Client-Serveur** : Un serveur centralisé gère les connexions et redistribue les messages.
* **Multi-threading** : Utilisation de la bibliothèque `pthread` pour gérer plusieurs clients simultanément sans bloquer le serveur.
* **Diffusion (Broadcast)** : Chaque message envoyé par un utilisateur est instantanément relayé à tous les autres participants connectés.
* **Gestion des Événements** :
    * Notification lors de l'arrivée d'un nouveau membre.
    * Détection automatique et notification lors du départ/déconnexion d'un client.
* **Synchronisation** : Utilisation de `mutex` (`pthread_mutex_lock`) pour éviter les conditions de concurrence sur les ressources partagées.

---

## 🛠️ Architecture Technique

Le projet repose sur l'API des sockets Berkeley. Voici le flux de communication :



1.  **Serveur** : `socket()` ➡️ `bind()` ➡️ `listen()` ➡️ `accept()`.
2.  **Client** : `socket()` ➡️ `connect()`.

---

## 💻 Installation et Utilisation

### Prérequis
* Un compilateur C (**GCC** recommandé).
* Un environnement Linux/Unix (pour le support des threads POSIX).

### Compilation
Ouvrez un terminal et compilez les fichiers source :

```bash
# Compilation du serveur (avec l'option -lpthread)
gcc -o server server.c -lpthread

# Compilation du client
gcc -o client client.c

Exécution :
Lancer le serveur
./server

Lancer le client
./client

Notions abordées
Programmation réseau en C.

Gestion des protocoles de transport (TCP/IP).

Programmation concurrente avec les Threads POSIX.

Gestion de la mémoire et des structures de données partagées.
