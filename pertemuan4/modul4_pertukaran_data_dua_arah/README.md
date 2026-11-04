# Penjelasan Modifikasi Kode Percobaan 4B (Kontrol Aktuator Kedua via Pembedaan Topic MQTT)

Dokumen ini berisi dokumentasi dan rincian penjelasan setiap baris kode yang ditambahkan pada program `modul4_pertukaran_data_dua_arah_mod.ino` guna memenuhi **Pertanyaan Praktikum 4B Nomor 4**:
> *"Modifikasi program agar menambahkan satu topic perintah baru untuk mengendalikan aktuator kedua (misalnya buzzer), dengan fungsi callback yang dapat membedakan topic mana yang menerima pesan, dan berikan penjelasan di setiap baris kode yang ditambahkan dalam bentuk README.md!"*

---

## 1. Deskripsi Modifikasi
Pada program asli Percobaan 4B, sistem hanya mendengarkan (*subscribe*) satu topic perintah MQTT untuk mengendalikan satu aktuator (LED), sembari mempublikasikan (*publish*) data pembacaan sensor DHT secara periodik menggunakan mekanisme pewaktuan `millis()`.

Pada program modifikasi:
- Ditambahkan satu buah topic perintah MQTT baru khusus untuk mengendalikan aktuator kedua (Buzzer).
- ESP8266 didaftarkan (*subscribe*) ke kedua topic tersebut saat terhubung ke broker.
- Fungsi `callback()` dimodifikasi sehingga dapat mendeteksi nama topic dari paket pesan yang masuk menggunakan pembanding string `strcmp()`. Dengan begitu, perintah diarahkan secara tepat ke aktuator LED atau aktuator Buzzer tanpa saling mengganggu.

---

## 2. Rincian Baris Kode yang Ditambahkan dan Penjelasannya

### A. Deklarasi Topic MQTT Baru untuk Aktuator Kedua
```cpp
const char* topicBuzzer = "unsoed/tk245004/kelompok6/buzzer";
```
* **Penjelasan:** Mendefinisikan variabel konstanta string yang menyimpan alamat topic unik MQTT untuk penerimaan perintah kendali buzzer. Topik ini dibedakan dari topic perintah LED (`topicPerintah`) agar pengiriman instruksi bersifat modular dan spesifik per aktuator.

---

### B. Konfigurasi Pin Hardware Aktuator Kedua
```cpp
const int buzzerPin = D5;
```
* **Penjelasan:** Mendefinisikan nomor pin GPIO ESP8266 yang dihubungkan ke kaki sinyal aktuator buzzer, yaitu pin `D5` (GPIO 14). Pin ini dipilih karena aman dari konflik jalur komunikasi I2C/SPI dan tidak mengganggu proses booting modul ESP8266.

---

### C. Inisialisasi Pin pada Fungsi `setup()`
```cpp
pinMode(buzzerPin, OUTPUT);
digitalWrite(buzzerPin, LOW);
```
* **Penjelasan:**
  - `pinMode(buzzerPin, OUTPUT)`: Mengonfigurasi pin `D5` sebagai jalur keluaran (*output*) digital agar mikrokontroler dapat mengirimkan sinyal kendali tegangan ke modul buzzer.
  - `digitalWrite(buzzerPin, LOW)`: Memastikan kondisi awal buzzer dalam keadaan tidak berbunyi (*mati*) sesaat setelah perangkat melakukan booting atau reset.

---

### D. Pendaftaran Subscribe Topic Baru pada Fungsi `hubungkanMQTT()`
```cpp
client.subscribe(topicBuzzer);
```
* **Penjelasan:** Mendaftarkan perangkat (*subscribe*) ke topic perintah buzzer pada broker MQTT HiveMQ. Baris ini diletakkan di dalam fungsi `hubungkanMQTT()` agar setiap kali koneksi MQTT terputus dan terhubung kembali (*reconnect*), langganan terhadap topic buzzer otomatis diperbarui bersamaan dengan topic LED.

---

### E. Logika Pembedaan Topic pada Fungsi `callback()`
```cpp
Serial.print("Pesan diterima pada topic [");
Serial.print(topic);
Serial.print("]: ");
Serial.println(pesan);
```
* **Penjelasan:** Mencetak informasi topic asal dan isi payload JSON yang diterima ke Serial Monitor untuk memudahkan observasi dan pelacakan alur data secara *real-time*.

```cpp
if (strcmp(topic, topicPerintah) == 0) {
  // --- AKTUATOR 1: KONTROL LED ---
  if (String(perintah) == "ON") {
    digitalWrite(ledPin, HIGH);
    Serial.println("Aktuator LED -> ON");
  } else if (String(perintah) == "OFF") {
    digitalWrite(ledPin, LOW);
    Serial.println("Aktuator LED -> OFF");
  }
} else if (strcmp(topic, topicBuzzer) == 0) {
  // --- AKTUATOR 2: KONTROL BUZZER ---
  if (String(perintah) == "ON") {
    digitalWrite(buzzerPin, HIGH);
    Serial.println("Aktuator Buzzer -> ON");
  } else if (String(perintah) == "OFF") {
    digitalWrite(buzzerPin, LOW);
    Serial.println("Aktuator Buzzer -> OFF");
  }
}
```
* **Penjelasan:**
  - Fungsi `strcmp(topic, ...)` membandingkan nama topic pesan yang masuk (`topic`) dengan daftar konstanta topic yang telah didefinisikan. Jika bernilai `0`, berarti nama kedua string identik (*match*).
  - Jika pesan berasal dari `topicPerintah`, program mengevaluasi nilai string dari `doc["perintah"]` untuk menyalakan (`HIGH`) atau mematikan (`LOW`) pin LED (`D1`).
  - Jika pesan berasal dari `topicBuzzer`, program mengevaluasi isi perintah untuk membunyikan (`HIGH`) atau mematikan (`LOW`) pin Buzzer (`D5`).
  - Struktur pengkondisian ini memastikan pesan kendali untuk satu aktuator tidak akan memicu perubahan kondisi pada aktuator lainnya.

---

## 3. Format Pengujian Perintah MQTT

Pengguna atau antarmuka MQTT client (misal: HiveMQ Web Client atau MQTTX) dapat mengirim perintah secara independen ke masing-masing topic:

1. **Mengendalikan LED:**
   - **Topic:** `unsoed/tk245004/kelompok6/sensor`
   - **Payload Nyala:** `{"perintah": "ON"}`
   - **Payload Mati:** `{"perintah": "OFF"}`

2. **Mengendalikan Buzzer:**
   - **Topic:** `unsoed/tk245004/kelompok6/buzzer`
   - **Payload Bunyi:** `{"perintah": "ON"}`
   - **Payload Senyap:** `{"perintah": "OFF"}`

---

## 4. Tabel Ringkasan Perubahan Kode

| Komponen / Fitur | Program Asli (`modul4_pertukaran_data_dua_arah.ino`) | Program Modifikasi (`modul4_pertukaran_data_dua_arah_mod.ino`) |
| :--- | :--- | :--- |
| **Topic Subscribe** | 1 Topic (`topicPerintah`) | 2 Topic (`topicPerintah` & `topicBuzzer`) |
| **Jumlah Aktuator** | 1 Aktuator (LED pada pin `D1`) | 2 Aktuator (LED pada `D1`, Buzzer pada `D5`) |
| **Logika Callback** | Langsung mengubah status LED tanpa pengecekan topic | Pengecekan nama topic dengan `strcmp()` sebelum mengeksekusi aktuator terkait |
| **Komunikasi Full Duplex** | Mengirim data suhu & menerima perintah LED | Mengirim data suhu & menerima perintah multi-aktuator (LED & Buzzer) secara simultan |

