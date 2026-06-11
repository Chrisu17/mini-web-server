#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "../include/server.h"

#define BUFFER_SIZE 4096

// Fungsi template untuk merakit dan mengirim halaman Error secara otomatis
void send_error(int client_fd, const char *status_code, const char *message) {
    char response[1024];
    snprintf(response, sizeof(response),
             "HTTP/1.1 %s\r\n"
             "Content-Type: text/html\r\n"
             "\r\n"
             "<html><body style='font-family: Arial; text-align: center; margin-top: 50px;'>"
             "<h1 style='color: #e74c3c;'>Error %s</h1>"
             "<p style='color: #7f8c8d; font-size: 1.2em;'>%s</p>"
             "<hr><p style='font-size: 0.8em;'><i>Mini Web Server - Christian Sumangando</i></p>"
             "</body></html>",
             status_code, status_code, message);
    write(client_fd, response, strlen(response));
}

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    
    // Baca request dari browser
    int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) return;
    buffer[bytes_read] = '\0';
    
    printf("[+] Browser minta:\n%s\n", buffer);

    char method[16], path[256], protocol[16];
    
    // 1. Tangani 400 BAD REQUEST (Jika format request tidak valid / kurang dari 3 bagian)
    if (sscanf(buffer, "%15s %255s %15s", method, path, protocol) != 3) {
        send_error(client_fd, "400 Bad Request", "Waduh, format request tidak valid atau rusak.");
        printf("[-] Mengirim 400 Bad Request\n");
        return;
    }

    // 2. Tangani 405 METHOD NOT ALLOWED (Server kita cuma nerima GET)
    if (strcmp(method, "GET") != 0) {
        send_error(client_fd, "405 Method Not Allowed", "Maaf, server ini hanya melayani metode GET.");
        printf("[-] Mengirim 405 Method Not Allowed (%s)\n", method);
        return;
    }

    if (strcmp(path, "/") == 0) {
        strcpy(path, "/index.html");
    }

    char filepath[512];
    snprintf(filepath, sizeof(filepath), "www%s", path);

    // Buka file
    int file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0) {
        // 3. Tangani 403 FORBIDDEN vs 404 NOT FOUND
        if (errno == EACCES) {
            // EACCES = Error Access (File ada, tapi Linux melarang dibaca)
            send_error(client_fd, "403 Forbidden", "Akses Ditolak! Anda tidak memiliki izin untuk membuka file ini.");
            printf("[-] Mengirim 403 Forbidden untuk %s\n", filepath);
        } else {
            // ENOENT = Error No Entity (File memang tidak ada)
            send_error(client_fd, "404 Not Found", "File yang Anda cari tidak ditemukan di server ini.");
            printf("[-] Mengirim 404 Not Found untuk %s\n", filepath);
        }
    } else {
        // --- Jika File Ketemu dan Bisa Dibaca (200 OK) ---
        struct stat file_stat;
        fstat(file_fd, &file_stat);
        
        // Set default ke HTML
    char *content_type = "text/html"; 
    
    // Kalau di URL ada kata .jpg atau .jpeg, ubah jadi gambar
    if (strstr(path, ".jpeg") != NULL || strstr(path, ".jpg") != NULL) {
        content_type = "image/jpeg";
    }

    // Rakit header-nya lalu kirim
    char header[256];
    snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", content_type);
    write(client_fd, header, strlen(header));

        int bytes_read_file;
        while ((bytes_read_file = read(file_fd, buffer, sizeof(buffer))) > 0) {
            write(client_fd, buffer, bytes_read_file);
        }
        close(file_fd);
        printf("[+] Sukses mengirim file %s\n", filepath);
    }
}
