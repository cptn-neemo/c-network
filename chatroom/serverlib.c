#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <poll.h>

#include "serverlib.h"

void pop_results(struct addrinfo **res) {
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    int status;
    if ((status = getaddrinfo(NODE, PORT, &hints, res)) != 0) {
        printf("%d\n", status);
        exit(1);
    }
}

int open_socket(struct addrinfo *entry) {
    int yes = 1;
    int sockfd;

    // Try to open the socket
    if ((sockfd = 
        socket(entry->ai_family, entry->ai_socktype, entry->ai_protocol)) == -1) {
        printf("Socket error: %d\n", errno);
        sockfd = -1;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    // Try to bind the addr to the socket
    if (bind(sockfd, entry->ai_addr, entry->ai_addrlen)) {
        printf("Binding error: %d\n", errno);
        sockfd = -1;
    }

    if (listen(sockfd, BACKLOG)) {
        printf("Cannot listen on the server.\n");
        sockfd = -1;
    }

    return sockfd;
}

int open_server() {
    int sockfd = -1;

    struct addrinfo *res; 

    pop_results(&res);

    for (struct addrinfo *entry = res; entry != NULL; entry = entry->ai_next) {
        if ((sockfd = open_socket(entry)) != -1)
            break;
    }

    freeaddrinfo(res);

    return sockfd;
}

int expand_pfds(struct pollfd **pfds, int cursize, int fd) {
    *pfds = realloc(*pfds, (cursize + 1) * sizeof(struct pollfd));
    if (pfds == NULL) {
        printf("Could not allocate more memory for a new connection.\n");
        exit(1);
    }

    (*pfds)[cursize].fd = fd;
    (*pfds)[cursize].events = POLLIN;

    return cursize + 1;
}

int delete_pfd(struct pollfd **pfds, int nfds, int index) {
    (*pfds)[index] = (*pfds)[nfds - 1];
    *pfds = realloc(*pfds, (nfds - 1) * sizeof(struct pollfd));
    return nfds - 1;
}

void broadcast(struct pollfd *pfds, size_t nfds, void *buf, size_t len, int infd, int sfd) {
    int status;
    for (int i = 0; i < nfds; i++) {
        if (pfds[i].fd != infd && pfds[i].fd != sfd) {
            status = send(pfds[i].fd, buf, len, 0);
            if (status == -1) {
                printf("Sending error code: %d\n", errno);
                exit(1);
            }
        }
    }
}
