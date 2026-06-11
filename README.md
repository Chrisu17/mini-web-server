# Panduan Menjalankan Mini Web Server

File ini berisi instruksi singkat untuk melakukan kompilasi, menjalankan, dan menguji server HTTP berbasis C menggunakan Makefile.

## 1. Cara Kompilasi (Build)
Server ini menggunakan Makefile untuk otomatisasi kompilasi. Buka terminal di dalam direktori proyek ini, lalu cukup ketik perintah berikut:
make

## 2. Cara Menjalankan Server
Setelah proses kompilasi selesai, jalankan server dengan menentukan nomor port yang diinginkan (contoh: port 8080):
./server 8080
Server akan masuk ke mode listening dan bersiap menerima koneksi.

## 3. Cara Menguji Server (Testing)
Ada dua cara untuk menguji apakah server berjalan dengan baik:

### A. Pengujian Fungsional (Web Browser)
Buka aplikasi web browser (seperti Firefox atau Chrome) dan masukkan URL berikut:
* http://localhost:8080 (Untuk melihat halaman utama HTML)
* http://localhost:8080/ngasal (Untuk menguji fitur respon 404 Not Found)

### B. Pengujian Beban / Concurrency (Apache Benchmark)
Untuk menguji kemampuan sistem multi-processing server dalam menangani banyak koneksi sekaligus, buka terminal baru dan gunakan alat penguji ab:
ab -n 1000 -c 100 http://localhost:8080/

## 4. Cara Membersihkan File Kompilasi
Untuk menghapus file biner hasil kompilasi, ketik perintah:
make clean
