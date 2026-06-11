#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "../include/server.h"

// Fungsi buat "mengubur" child process biar ga jadi zombie
void handle_zombie(int sig) {
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

int main(int argc, char *argv[]) {
    // 1. Validasi argumen CLI (Harus masukin port)
    if (argc != 2) {
        fprintf(stderr, "Cara pakai: %s <port>\n", argv[0]);
        fprintf(stderr, "Contoh: %s 8080\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]); // Ubah string port jadi angka

    // 2. Setup penangkap sinyal (SIGCHLD) untuk nangani Zombie Process
    struct sigaction sa;
    sa.sa_handler = handle_zombie;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("Gagal setup sigaction untuk zombie");
        exit(EXIT_FAILURE);
    }

    // 3. Panggil fungsi socket yang udah kita bikin di network.c
    int server_fd = initiate_socket(port);
    printf("Server Christian jalan di port %d... Menunggu koneksi...\n", port);

    // 4. Loop Utama: Server standby terus menerus
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        // Terima koneksi dari browser
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Gagal accept koneksi");
            continue;
        }

        // 5. Concurrency: Pecah proses pakai fork()
        pid_t pid = fork();
        
        if (pid == 0) {
            // --- INI DI DALAM CHILD PROCESS (Pekerja yang ngelayanin 1 browser) ---
            close(server_fd); // Pekerja ga butuh dengerin koneksi baru
            handle_client(client_fd); // Lempar ke HTTP Parser (Bagian 2)
            close(client_fd); // Tutup koneksi kalau udah selesai ngirim file
            exit(EXIT_SUCCESS); // Pekerja mati setelah tugas selesai
        } 
        else if (pid > 0) {
            // --- INI DI DALAM PARENT PROCESS (Bos utama) ---
            close(client_fd); // Bos ga ngurusin browser, biar pekerja (child) yang urus
        } 
        else {
            perror("Gagal melakukan fork");
        }
    }
    return 0;
}
