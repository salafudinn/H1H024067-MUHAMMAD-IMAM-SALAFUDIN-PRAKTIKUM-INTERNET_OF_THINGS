# Modifikasi Percobaan 2.6: Mode AP+STA ESP32

## Deskripsi

Program asli hanya menjalankan ESP32 sebagai Access Point tunggal (`WiFi.mode(WIFI_AP)`), sehingga perangkat yang terhubung ke ESP32 tidak memiliki akses ke internet karena ESP32 sendiri tidak terhubung ke jaringan WiFi rumah.

Modifikasi ini mengubah mode operasi ESP32 menjadi **AP+STA (dual mode)**:
- ESP32 tetap memancarkan Access Point sendiri (mode AP) agar perangkat lain bisa terhubung langsung ke ESP32.
- Secara bersamaan, ESP32 juga terhubung sebagai Station (mode STA) ke jaringan WiFi rumah yang memiliki akses internet.

---

## Ringkasan Alur Program

1. Mode WiFi diatur ke `WIFI_AP_STA` agar kedua fungsi (AP dan STA) aktif bersamaan.
2. Access Point diaktifkan lebih dulu dengan `WiFi.softAP()` menggunakan SSID dan password ESP32.
3. IP Address Access Point ditampilkan ke Serial Monitor.
4. ESP32 kemudian mencoba terhubung ke WiFi rumah dengan `WiFi.begin()`.
5. Setelah terhubung, IP Address hasil koneksi STA juga ditampilkan ke Serial Monitor.

---

## Penjelasan Baris yang Ditambahkan/Diubah

### 1. Kredensial Access Point (Baru)

```cpp
const char* ap_ssid = "ESP32_AccessPoint";  // nama jaringan yang dipancarkan ESP32
const char* ap_password = "esp32password";  // password Access Point ESP32
```

Program asli hanya memiliki satu pasang kredensial (untuk AP saja). Dua variabel ini ditambahkan sebagai kredensial khusus Access Point, terpisah dari kredensial WiFi rumah (`sta_ssid`, `sta_password`), karena kini ESP32 harus mengelola dua identitas jaringan sekaligus.

### 2. Mode Dual: AP + STA

```cpp
WiFi.mode(WIFI_AP_STA);
```

Ini adalah **perubahan utama** dari program asli yang menggunakan `WiFi.mode(WIFI_AP)`. Mode `WIFI_AP_STA` mengaktifkan dua fungsi radio WiFi ESP32 secara bersamaan: sebagai pemancar Access Point dan sebagai klien (Station) yang terhubung ke router lain.

### 3. Mengaktifkan Access Point

```cpp
WiFi.softAP(ap_ssid, ap_password);
Serial.println(WiFi.softAPIP());
```

Baris ini tetap ada seperti program asli, namun kini dijalankan berdampingan dengan proses koneksi STA, bukan berdiri sendiri. `WiFi.softAPIP()` menampilkan IP default Access Point (umumnya `192.168.4.1`) agar perangkat lain bisa mengaksesnya.

### 4. Koneksi sebagai Station (Baru)

```cpp
WiFi.begin(sta_ssid, sta_password);
while (WiFi.status() != WL_CONNECTED) {
  delay(500);
}
Serial.println(WiFi.localIP());
```

Blok ini sepenuhnya baru dibandingkan program asli. Setelah Access Point aktif, ESP32 juga mencoba login ke jaringan WiFi rumah sebagai Station. `WiFi.localIP()` menampilkan IP yang diberikan oleh router rumah kepada ESP32, berbeda dari `WiFi.softAPIP()` yang merupakan IP milik ESP32 sendiri sebagai AP.

---

## Ringkasan Perubahan

| Aspek | Program Asli | Program Modifikasi |
|---|---|---|
| Mode WiFi | `WIFI_AP` (AP saja) | `WIFI_AP_STA` (AP + STA) |
| Akses internet untuk klien AP | Tidak ada | Ada (lewat koneksi STA ke WiFi rumah) |
| Jumlah kredensial WiFi | 1 pasang (AP) | 2 pasang (AP dan STA) |
| IP yang ditampilkan | Hanya IP AP | IP AP dan IP STA |