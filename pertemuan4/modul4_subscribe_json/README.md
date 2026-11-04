# Penjelasan Modifikasi Kode Percobaan 4A (Kontrol Kecerahan LED via PWM JSON)

Dokumen ini berisi dokumentasi dan penjelasan rinci setiap baris kode yang ditambahkan pada program `modul4_subscribe_json_mod.ino` guna memenuhi **Pertanyaan Praktikum 4A Nomor 4**:
> *"Modifikasi program agar data JSON yang diterima juga memuat nilai intensitas (misalnya `{\"perintah\": \"ON\", \"intensitas\": 200}`) yang digunakan untuk mengatur kecerahan LED menggunakan PWM (`analogWrite`/`ledcWrite`), dan berikan penjelasan di setiap baris kode yang ditambahkan dalam bentuk README.md"*

---

## 1. Deskripsi Modifikasi
Pada program awal, pengontrolan LED hanya berupa logika digital biner (`digitalWrite`) yaitu kondisi menyala penuh (`HIGH`) atau mati total (`LOW`). 

Pada program modifikasi:
- Format payload JSON diperluas sehingga selain memuat kunci `"perintah"`, juga dapat memuat kunci `"intensitas"` berupa nilai numerik (0–255).
- Output pin LED dikontrol menggunakan modulasi lebar pulsa (*Pulse Width Modulation* / PWM) melalui fungsi `analogWrite()`.
- Hal ini memungkinkan pengguna tidak hanya menyalakan atau mematikan LED dari jarak jauh melalui MQTT, tetapi juga mengatur tingkat kecerahan (*brightness*) secara presisi.

---

## 2. Rincian Baris Kode yang Ditambahkan dan Penjelasannya

### A. Pengambilan Nilai Intensitas dari Dokumen JSON
```cpp
int intensitas = doc["intensitas"] | 255;
```
* **Penjelasan:** Mengambil nilai numerik dari key `"intensitas"` di dalam objek JSON `doc`. Operator *pipe* (`| 255`) berfungsi sebagai nilai *fallback* / default: jika pesan JSON yang dikirimkan tidak menyertakan key `"intensitas"`, nilai intensitas otomatis diatur ke `255` (daya penuh), sehingga program tetap kompatibel dengan perintah versi sebelumnya.

---

### B. Validasi Rentang dan Penerapan PWM saat Perintah "ON"
```cpp
intensitas = constrain(intensitas, 0, 255);
analogWrite(ledPin, intensitas);
Serial.print("Aktuator: ON | Intensitas PWM: ");
Serial.println(intensitas);
```
* **Penjelasan:**
  - `constrain(intensitas, 0, 255)`: Membatasi nilai input agar selalu berada di dalam rentang duty cycle 8-bit yang valid (antara 0 hingga 255) untuk mencegah anomali nilai negatif atau *overflow*.
  - `analogWrite(ledPin, intensitas)`: Mengirimkan sinyal PWM dengan *duty cycle* sebesar nilai `intensitas` ke pin LED (`D1`). Semakin besar nilainya (maksimal 255), semakin terang nyala LED.
  - `Serial.print(...)`: Mencetak informasi status aktuator beserta nilai intensitas yang diterapkan ke Serial Monitor untuk mempermudah pemantauan (*debugging*).

---

### C. Pemadaman LED Menggunakan PWM saat Perintah "OFF"
```cpp
analogWrite(ledPin, 0);
Serial.println("Aktuator: OFF");
```
* **Penjelasan:** Mengatur *duty cycle* sinyal PWM menjadi `0`, yang secara efektif mematikan keluaran tegangan pada pin LED (`D1`) sehingga LED padam sepenuhnya.

---

### D. Konfigurasi Resolusi PWM pada Fungsi `setup()`
```cpp
analogWriteRange(255);
analogWrite(ledPin, 0);
```
* **Penjelasan:**
  - `analogWriteRange(255)`: Menyesuaikan rentang resolusi PWM pada mikrokontroler ESP8266 (yang secara bawaan bernilai 0–1023) agar beroperasi pada skala standar 8-bit (0–255), sesuai dengan spesifikasi parameter intensitas pada instruksi soal praktikum.
  - `analogWrite(ledPin, 0)`: Memastikan bahwa saat perangkat pertama kali dinyalakan atau di-booting, sinyal PWM berada pada *duty cycle* `0` sehingga LED dipastikan dalam keadaan mati.

---

## 3. Format Pengujian Payload JSON

Contoh payload JSON yang dapat dipublikasikan ke topic `unsoed/tk245004/kelompok6/sensor`:

1. **LED Menyala Redup (~20%):**
   ```json
   {"perintah": "ON", "intensitas": 50}
   ```
2. **LED Menyala Sedang (~78% sesuai contoh soal):**
   ```json
   {"perintah": "ON", "intensitas": 200}
   ```
3. **LED Menyala Maksimal (100%):**
   ```json
   {"perintah": "ON", "intensitas": 255}
   ```
4. **LED Padam:**
   ```json
   {"perintah": "OFF"}
   ```

---

## 4. Tabel Ringkasan Perubahan Kode

| Aspek | Program Asal (`modul4_subscribe_json.ino`) | Program Modifikasi (`modul4_subscribe_json_mod.ino`) |
| :--- | :--- | :--- |
| **Metode Kendali LED** | Digital Biner (`digitalWrite`) | Modulasi Lebar Pulsa PWM (`analogWrite`) |
| **Struktur Payload JSON** | Hanya key `"perintah"` (`"ON"` / `"OFF"`) | Mendukung key `"perintah"` dan key `"intensitas"` |
| **Rentang Kendali Output** | 2 Status (Mati / Nyala Penuh) | Kontinu 0 s.d. 255 tingkat kecerahan |
| **Status Awal di Setup** | `digitalWrite(ledPin, LOW)` | `analogWriteRange(255)` & `analogWrite(ledPin, 0)` |

