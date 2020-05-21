#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "lib.h"

#define PORT "6180"
#define NODE "127.0.0.1" 
#define BACKLOG 10

const char *buf = "Hello, World!";

int open_server(struct addrinfo *res) {
    int sockfd = -1;
    int yes = 1;

    for (struct addrinfo *entry = res; entry != NULL; entry = entry->ai_next) {
        // Try to open the socket
        if ((sockfd = 
            socket(entry->ai_family, entry->ai_socktype, entry->ai_protocol)) == -1) {
            printf("Socket error: %d\n", errno);
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        // Try to bind the addr to the socket
        if (bind(sockfd, entry->ai_addr, entry->ai_addrlen)) {
            printf("Binding error: %d\n", errno);
            sockfd = -1;
            continue;
        }

        if (listen(sockfd, BACKLOG)) {
            printf("Cannot listen on the server.\n");
            sockfd = -1;
            continue;
        }
    }

    return sockfd;
}

void *serve_request(void *arg) {
    int sockpeer = *((int*) arg);

    send(sockpeer, buf, strlen(buf), 0);
    shutdown(sockpeer, 2);

    free(arg);

    return NULL;
}

int main() {
    int sockfd, status;
    struct addrinfo hints, *res; 

    pthread_t threads[10] = { 0 };
    int thread_iter = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    if ((status = getaddrinfo(NODE, PORT, &hints, &res)) != 0) {
        printf("%d\n", status);
        return 1;
    }

    print_addr(res);

    if ((sockfd = open_server(res)) == -1) {
        printf("Unable to open the server.\n");
        return 1;
    }

    printf("Listening on port %s\n", PORT);

    int sockpeer;
    struct sockaddr_in peeraddr;
    socklen_t peer_addr_size = sizeof (struct sockaddr);

    while (1) {
        if ((sockpeer = accept(sockfd, (struct sockaddr *) &peeraddr, &peer_addr_size)) == -1) {
            continue;
        }
        
        printf("ThreadIter: %d\n", thread_iter);
        int *thread_sockfd = malloc(sizeof(int));
        *thread_sockfd = sockpeer;

        pthread_join(threads[thread_iter], NULL);
        pthread_create(&(threads[thread_iter]), NULL, &serve_request, thread_sockfd);
        thread_iter = (thread_iter + 1) % BACKLOG;
    }

    freeaddrinfo(res);

    return 0;
}
