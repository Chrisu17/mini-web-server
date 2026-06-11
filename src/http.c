#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../include/server.h"

void handle_client(int client_fd) {
    printf("[+] Ada browser yang connect nih!\n");
    
    // Balasan HTTP dummy sementara
    char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHalo dari Server C buatan Christian!";
    write(client_fd, response, strlen(response));
}
