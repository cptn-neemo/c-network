#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include "lib.h"

#define PORT "6180"
#define NODE "127.0.0.1" 
#define BACKLOG 10


int open_server(struct addrinfo *res) {
    int sockfd = -1;

    for (struct addrinfo *entry = res; entry != NULL; entry = entry->ai_next) {
        // Try to open the socket
        if ((sockfd = 
            socket(entry->ai_family, entry->ai_socktype, entry->ai_protocol)) == -1) {
            printf("Socket error: %d\n", errno);
            continue;
        }

        // Try to bind the addr to the socket
        if (bind(sockfd, entry->ai_addr, entry->ai_addrlen)) {
            printf("Binding error: %d\n", errno);
            continue;
        }

        if (listen(sockfd, BACKLOG)) {
            printf("Cannot listen on the server.\n");
            continue;
        }
    }

    return sockfd;
}

int main() {
    int sockfd, status;
    struct addrinfo hints, *res; 
    const char *buf = "Hello, World!";

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

    int sockpeer;
    struct sockaddr peeraddr;
    socklen_t peer_addr_size = sizeof (struct sockaddr);
    //char peerstr[INET6_ADDRSTRLEN];

    while (1) {
        if ((sockpeer = accept(sockfd, &peeraddr, &peer_addr_size)) == -1) {
            continue;
        }
        send(sockpeer, buf, strlen(buf), 0);
        shutdown(sockpeer, 2);
    }

    return 0;
}
