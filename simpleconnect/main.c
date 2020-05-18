#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#define LOCAL_ADDR "127.0.0.1"
#define NODE "www.google.com"

void print_addr(struct addrinfo *res) {
    char addr_string[INET6_ADDRSTRLEN];
    void *addr;
    char *ipver;

    if (res->ai_family == AF_INET) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in*) res->ai_addr;
        addr = &(ipv4->sin_addr);
        ipver = "IPv4";
    } else {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*) res->ai_addr;
        addr = &(ipv6->sin6_addr);
        ipver = "IPv6";
    }
    
    inet_ntop(res->ai_family, addr, addr_string, sizeof addr_string);

    printf("%s: %s\n", ipver, addr_string);
}

int main(int argc, char **argv) {
    struct addrinfo *res;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status;
    if ((status = getaddrinfo(NODE, "http", &hints, &res) != 0)) {
        printf("%s\n", gai_strerror(status));
        return 1;
    }

    // Create a new socket for the first entry
    int s;
    if ((s = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
        printf("Could not open a socket for google.\n");
        return 1;
    }

    // Bind the local address to a port
    // This is needed if we are a server, but if we're just connecting, the connect
    // system call does it for us
    // if ((status = bind(s, res->ai_addr, res->ai_addrlen)) == -1) {
    //    printf("bind error: %d\n", errno);
    //    return 1;
    //}

    print_addr(res);

    if ((status = connect(s, res->ai_addr, res->ai_addrlen)) == -1) {
        printf("Could not connect to the address.\n");
        printf("%d\n", errno);
        return 1;
    }

    freeaddrinfo(res);
    return 0;
}
