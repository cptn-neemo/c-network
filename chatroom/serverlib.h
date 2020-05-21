#ifndef SERVERLIB
#define SERVERLIB

#define PORT "6180"
#define NODE "127.0.0.1" 
#define BACKLOG 10

int open_server();
int expand_pfds(struct pollfd **, int, int);
int delete_pfd(struct pollfd **, int, int);
void broadcast(struct pollfd *, size_t, void*, size_t, int, int);

#endif
