#ifndef SERVERLIB
#define SERVERLIB

#define PORT "6180"
#define NODE "127.0.0.1" 
#define BACKLOG 10

static const char *message = "Hello, World!";

int open_server();
int expand_pfds(struct pollfd **, int);
void broadcast(struct pollfd *, size_t, void*, size_t, int);

#endif
