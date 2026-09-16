# Modifikasi Percobaan 2.5: Auto Reconnect WiFi ESP32

## Deskripsi

Program asli hanya melakukan koneksi WiFi satu kali di `setup()` menggunakan `WiFi.begin()`, tanpa ada mekanisme untuk menangani kondisi apabila koneksi WiFi terputus di tengah program berjalan. Akibatnya, jika sinyal WiFi hilang sementara (misalnya router restart), ESP32 tidak akan pernah terhubung kembali secara otomatis.

Modifikasi ini menambahkan **mekanisme reconnect otomatis** menggunakan pendekatan non-blocking berbasis `millis()`:
- Status koneksi WiFi dipantau terus-menerus di dalam `loop()`.
- Jika koneksi terputus, ESP32 akan mencoba menghubungkan ulang setiap 10 detik.
- Proses reconnect tidak menggunakan `delay()` agar tidak menghentikan (blocking) eksekusi program lain.

---

## Ringkasan Alur Program

1. Fungsi `connectWiFi()` dipanggil sekali di `setup()` untuk melakukan koneksi awal.
2. Di dalam `loop()`, status koneksi WiFi dicek setiap iterasi menggunakan `WiFi.status()`.
3. Jika status bukan `WL_CONNECTED`, program memeriksa apakah sudah 10 detik sejak percobaan reconnect terakhir.
4. Jika sudah 10 detik, ESP32 melakukan `WiFi.disconnect()` lalu `WiFi.reconnect()`.
5. Proses ini berulang otomatis setiap kali koneksi terputus, tanpa perlu reset manual.

---

## Penjelasan Baris yang Ditambahkan/Diubah

### 1. Variabel Timer Non-Blocking

```cpp
unsigned long previousMillis = 0; // menyimpan waktu terakhir percobaan reconnect
const long interval = 10000;      // jeda 10 detik antar percobaan reconnect
```

Pada program asli tidak ada mekanisme timer sama sekali karena koneksi hanya dilakukan sekali di `setup()`. Dua variabel ini ditambahkan agar `loop()` bisa menghitung selisih waktu tanpa memakai `delay()` yang bersifat blocking.

### 2. Fungsi `connectWiFi()` (Dipisah dari `setup()`)

```cpp
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}
```

Logika koneksi awal yang semula ditulis langsung di `setup()` dipindahkan ke fungsi terpisah. Tujuannya agar proses koneksi bisa dipanggil ulang kapan saja tanpa menduplikasi kode, meskipun pada versi ini reconnect dilakukan lewat `WiFi.reconnect()`, bukan memanggil `connectWiFi()` lagi.

### 3. Pengecekan Status Koneksi di `loop()`

```cpp
if (WiFi.status() != WL_CONNECTED) {
  if (currentMillis - previousMillis >= interval) {
    ...
  }
}
```

Bagian ini adalah **inti mekanisme reconnect**. Program asli tidak memiliki `loop()` yang memantau status koneksi sama sekali. Kondisi `WiFi.status() != WL_CONNECTED` memastikan proses reconnect hanya berjalan ketika koneksi benar-benar terputus, dan kondisi selisih `millis()` memastikan percobaan reconnect tidak dilakukan berulang-ulang tanpa jeda (yang bisa membebani modul WiFi).

### 4. Perintah Reconnect

```cpp
WiFi.disconnect();
WiFi.reconnect();
```

`WiFi.disconnect()` membersihkan sesi koneksi lama yang sudah tidak valid, kemudian `WiFi.reconnect()` memerintahkan ESP32 mencoba terhubung kembali menggunakan SSID dan password yang sama seperti percobaan `WiFi.begin()` sebelumnya.

---

## Ringkasan Perubahan

| Aspek | Program Asli | Program Modifikasi |
|---|---|---|
| Jumlah percobaan koneksi | 1 kali (di `setup()`) | Berulang otomatis (di `loop()`) |
| Penanganan koneksi terputus | Tidak ada | Ada, dicek setiap iterasi `loop()` |
| Jenis delay | `delay()` blocking | `millis()` non-blocking |
| Ketahanan sistem | Rendah (harus reset manual) | Tinggi (otomatis reconnect) |