#ifndef SERVER_H
#define SERVER_H

int initiate_socket(int port);
void handle_client(int client_fd);

#endif
