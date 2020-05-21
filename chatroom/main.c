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


int main() {
    int sockfd;

    int nfds = 0;
    struct pollfd *pfds = (struct pollfd *) malloc(sizeof(struct pollfd));

    if ((sockfd = open_server()) == -1) {
        printf("Unable to open the server.\n");
        return 1;
    }

    printf("Listening on port %s\n", PORT);

    int sockpeer;
    struct sockaddr_in peeraddr;
    socklen_t peer_addr_size = sizeof (struct sockaddr);

    int numevents;
    while (1) {
        if ((sockpeer = accept(sockfd, (struct sockaddr *) &peeraddr, &peer_addr_size)) == -1) {
            continue;
        }

        char *peeraddr_str = inet_ntoa(peeraddr.sin_addr);
        printf("Got a connection from: %s\n", peeraddr_str);

        // TODO: Add this client's sockfd to the array
        // TODO: Go into the poll block, then send out the data

        //send(sockpeer, message, strlen(message), 0);
        //shutdown(sockpeer, 2);

        nfds = expand_pfds(&pfds, nfds);
        pfds[nfds - 1].fd = sockpeer;
        pfds[nfds - 1].events = POLLIN;

        printf("Number fds: %d\n", nfds);

        if ((numevents = poll(pfds, nfds, 10)) > 0) {
            // TODO: There has been an input
            
            printf("something happened\n");
            for (int i = 0; i < nfds; i++) {
                if (pfds[i].revents & POLLIN) {
                    printf("Something to send!\n");
                }
            }
        } else if (numevents == 0) {
            // There has been a timeout
            printf("timeout\n");
        } else {
            printf("Poll error code: %d\n", errno);
            exit(1);
        }

        //if (nfds == 5) {
        //    for (int i = 0; i < nfds; i++) {
        //        send(pfds[i].fd, message, strlen(message), 0);
        //    }
        //}
    }


    return 0;
}
