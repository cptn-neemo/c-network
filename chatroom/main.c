#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>

#include "serverlib.h"


int main() {
    int sockfd;

    int nfds = 0;
    struct pollfd *pfds = (struct pollfd *) malloc(sizeof(struct pollfd));

    if ((sockfd = open_server()) == -1) {
        printf("Unable to open the server.\n");
        return 1;
    }

    nfds = expand_pfds(&pfds, nfds, sockfd);

    int sockpeer;
    struct sockaddr_in peeraddr;
    socklen_t peer_addr_size = sizeof (struct sockaddr);

    char buffer[1000] = {0};

    int numevents;
    while (1) {
        //char *peeraddr_str = inet_ntoa(peeraddr.sin_addr);
        //printf("Got a connection from: %s\n", peeraddr_str);

        //nfds = expand_pfds(&pfds, nfds, sockpeer);

        //printf("Number fds: %d\n", nfds);

        if ((numevents = poll(pfds, nfds, 1000)) > 0) {
            for (int i = 0; i < nfds; i++) {
                if ((pfds[i].revents & POLLIN) && (pfds[i].fd == sockfd)) {
                    sockpeer = accept(sockfd, (struct sockaddr *) &peeraddr, &peer_addr_size);

                    char *peeraddr_str = inet_ntoa(peeraddr.sin_addr);
                    printf("Got a connection from: %s\n", peeraddr_str);

                    nfds = expand_pfds(&pfds, nfds, sockpeer);
                } else if (pfds[i].revents & POLLIN) {
                    int size = recv(pfds[i].fd, buffer, 1000, 0);
                    if (size > 0) {
                        broadcast(pfds, nfds, buffer, size, pfds[i].fd, sockfd);
                    } else if (size == 0) {
                        // Need to close this connection
                        printf("Closing connection: %d\n", pfds[i].fd);
                        close(pfds[i].fd);
                        nfds = delete_pfd(&pfds, nfds, i);
                    } else {
                        printf("recv error code: %d\n", errno);
                    }
                }
            }
        } else if (numevents == 0) {
            // There has been a timeout
        } else {
            printf("Poll error code: %d\n", errno);
            exit(1);
        }
    }

    return 0;
}
