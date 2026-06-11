#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../include/server.h"

#define BUFFER_SIZE 4096

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    
    // 1. Baca request dari browser (misal: "GET / HTTP/1.1")
    int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        return; // Koneksi error atau kosong
    }
    buffer[bytes_read] = '\0'; // Tutup string
    
    printf("[+] Browser minta:\n%s\n", buffer);

    // 2. Ekstrak jalur file yang diminta (Parsing)
    char method[16], path[256], protocol[16];
    sscanf(buffer, "%15s %255s %15s", method, path, protocol);

    // Kalau browser cuma ketik localhost:8080/, otomatis arahkan ke /index.html
    if (strcmp(path, "/") == 0) {
        strcpy(path, "/index.html");
    }

    // 3. Sambungkan jalur dengan folder www (jadi "www/index.html")
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "www%s", path);

    // 4. Cari dan buka file-nya
    int file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0) {
        // --- JIKA FILE GAK KETEMU (ERROR 404) ---
        char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404: Waduh, File Gak Ketemu Cuy!</h1>";
        write(client_fd, not_found, strlen(not_found));
        printf("[-] Ngirim 404 karena file %s ga ada.\n", filepath);
    } else {
        // --- JIKA FILE KETEMU (200 OK) ---
        // Ukur besar file-nya
        struct stat file_stat;
        fstat(file_fd, &file_stat);
        
        // Bikin header balasan HTTP standar
        char header[512];
        snprintf(header, sizeof(header), 
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/html\r\n"
                 "Content-Length: %ld\r\n"
                 "\r\n", file_stat.st_size);
                 
        // Kirim header-nya dulu
        write(client_fd, header, strlen(header));

        // Baru kirim isi file HTML-nya
        int bytes_read_file;
        while ((bytes_read_file = read(file_fd, buffer, sizeof(buffer))) > 0) {
            write(client_fd, buffer, bytes_read_file);
        }
        close(file_fd);
        printf("[+] Sukses ngirim file %s\n", filepath);
    }
}
