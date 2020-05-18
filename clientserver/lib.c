#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>


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
