# Modifikasi Percobaan 1.6: Kendali Aktuator Histerisis (Dua Ambang Batas)

## Deskripsi

Program asli menggunakan satu ambang batas (`suhuThreshold = 30.0`) untuk menyalakan dan mematikan aktuator, yang berpotensi menyebabkan aktuator menyala-mati berulang kali (*chattering*) jika suhu berosilasi di sekitar 30°C.

Modifikasi ini menerapkan **histerisis**:
- Aktuator **menyala** di atas 30°C.
- Aktuator baru **mati** jika suhu turun di bawah 28°C.
- Di antara 28°C dan 30°C, status aktuator **dipertahankan** seperti sebelumnya (tidak berubah).

---

## Ringkasan Alur Program

1. Definisikan dua ambang batas: `thresholdAtas` (30°C) dan `thresholdBawah` (28°C).
2. Simpan status aktuator terakhir dalam variabel global `statusRelay`.
3. Setiap iterasi `loop()`, bandingkan suhu terhadap kedua ambang batas dengan `if / else if`.
4. Jika suhu berada di zona netral (28–30°C), status aktuator tidak diubah.
5. Terapkan status akhir ke pin relay dan tampilkan di Serial Monitor.

---

## Penjelasan Baris yang Ditambahkan/Diubah

### 1. Dua Konstanta Ambang Batas (Menggantikan Satu `suhuThreshold`)

```cpp
const float thresholdAtas = 30.0;  // ambang batas atas: aktuator menyala jika suhu melewati ini
const float thresholdBawah = 28.0; // ambang batas bawah: aktuator mati jika suhu turun di bawah ini
```

Pada program asli hanya ada satu variabel `suhuThreshold`. Di sini diganti menjadi dua variabel:

- **`thresholdAtas`** — batas atas; begitu suhu melewati nilai ini, aktuator dinyalakan.
- **`thresholdBawah`** — batas bawah; aktuator baru dimatikan setelah suhu turun di bawah nilai ini.

Selisih antara keduanya (30.0 − 28.0 = **2°C**) adalah "zona netral" atau *deadband* yang membuat sistem tidak langsung bereaksi terhadap fluktuasi kecil.

### 2. Variabel Status Aktuator

```cpp
bool statusRelay = false; // menyimpan status aktuator saat ini (true = ON, false = OFF)
```

Dideklarasikan sebagai variabel **global** bertipe `bool`. Pada program asli, status aktuator langsung ditentukan setiap kali `loop()` berjalan (berdasarkan satu kondisi `if`), sehingga tidak ada "memori" status sebelumnya.

Pada versi histerisis, status aktuator harus diingat antar-iterasi `loop()`, karena ketika suhu berada di zona netral (28–30°C), aktuator harus mempertahankan kondisi terakhirnya, bukan dihitung ulang dari nol. Nilai awal `false` berarti aktuator dianggap mati saat program pertama kali dijalankan (sesuai `digitalWrite(RELAYPIN, LOW)` di `setup()`).

### 3. Logika Histerisis dengan `if` / `else if`

```cpp
if (suhu > thresholdAtas) {
  statusRelay = true;  // suhu melewati batas atas, aktuator dinyalakan
} else if (suhu < thresholdBawah) {
  statusRelay = false; // suhu turun di bawah batas bawah, aktuator dimatikan
}
// jika suhu berada di antara thresholdBawah dan thresholdAtas, statusRelay tidak diubah
```

Ini adalah **inti mekanisme histerisis** dan berbeda signifikan dari program asli:

- `if (suhu > thresholdAtas)` mengubah `statusRelay` menjadi `true` (ON) hanya jika suhu benar-benar melewati batas atas (30°C).
- `else if (suhu < thresholdBawah)` mengubah `statusRelay` menjadi `false` (OFF) hanya jika suhu benar-benar turun di bawah batas bawah (28°C).
- Jika suhu berada di antara 28°C dan 30°C, tidak ada cabang yang terpenuhi, sehingga `statusRelay` **tidak diubah** — nilai sebelumnya tetap dipakai.

Inilah yang mencegah aktuator menyala-mati berulang saat suhu berosilasi kecil di sekitar satu titik, karena kini ada jarak (*deadband*) antara titik nyala dan titik mati.

### 4. Menerapkan Status ke Pin Relay (Diubah)

```cpp
if (statusRelay) {
  digitalWrite(RELAYPIN, HIGH);
  Serial.println("Aktuator: ON");
} else {
  digitalWrite(RELAYPIN, LOW);
  Serial.println("Aktuator: OFF");
}
```

Pada program asli, pembacaan status dan perintah `digitalWrite` dipicu langsung oleh pengecekan satu *threshold* tunggal. Pada versi modifikasi ini, perintah `digitalWrite` dan tampilan Serial Monitor dipisahkan, lalu dieksekusi berdasarkan variabel kontrol `statusRelay` yang telah melalui evaluasi logika histerisis.

### 5. Delay Pembacaan Sensor

```cpp
delay(2000); // jeda pembacaan sensor DHT22 (minimal 2 detik)
```

Ditambahkan di akhir fungsi `loop()` untuk menjaga stabilitas interval waktu akuisisi data sensor DHT22, agar siklus pembacaan berikutnya berjalan aman tanpa menyebabkan data korup atau bernilai `NaN`.

---

## Ringkasan Perubahan

| Aspek | Program Asli | Program Modifikasi |
|---|---|---|
| Jumlah ambang batas | 1 (`suhuThreshold`) | 2 (`thresholdAtas`, `thresholdBawah`) |
| Memori status aktuator | Tidak ada | Ada (`statusRelay`, global) |
| Perilaku di zona 28–30°C | Bisa *chattering* | Status dipertahankan (stabil) |
| Risiko nyala-mati berulang | Tinggi | Rendah (berkat *deadband* 2°C) |
