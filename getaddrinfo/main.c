#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: ./main <url>\n");
        return 1;
    }

    const char *node = argv[1];
    struct addrinfo *res;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status;
    if ((status = getaddrinfo(node, "http", &hints, &res) != 0)) {
        printf("%s\n", gai_strerror(status));
        return 1;
    }

    char addr_string[INET6_ADDRSTRLEN];
    while (res != NULL) {
        void *addr;

        if (res->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in*) res->ai_addr;
            addr = &(ipv4->sin_addr);
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*) res->ai_addr;
            addr = &(ipv6->sin6_addr);
        }
        
        inet_ntop(res->ai_family, addr, addr_string, sizeof addr_string);

        printf("%s\n", addr_string);

        res = res->ai_next;
    }

    freeaddrinfo(res);
    return 0;
}
