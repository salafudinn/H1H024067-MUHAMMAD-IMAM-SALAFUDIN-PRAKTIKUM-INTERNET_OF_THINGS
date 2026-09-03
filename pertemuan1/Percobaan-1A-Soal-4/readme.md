# Modifikasi Percobaan 1.5: Akuisisi Data Sensor DHT22 (Rata-Rata 5 Sampel)

## Deskripsi

Program asli membaca dan menampilkan data suhu serta kelembaban secara langsung setiap 2 detik sekali. Modifikasi ini mengubah alur program agar mengumpulkan **5 sampel data** terlebih dahulu, menghitung nilai rata-ratanya, baru kemudian menyajikan hasil rata-rata tersebut ke Serial Monitor.

> **Tujuan:** mereduksi noise atau fluktuasi pembacaan acak pada sensor.

---

## Ringkasan Alur Program

1. Siapkan variabel akumulator (`totalSuhu`, `totalKelembaban`) dan penghitung sampel valid (`sampelValid`).
2. Ambil 5 sampel suhu & kelembaban secara berurutan, dengan jeda 2 detik antar sampel.
3. Validasi setiap sampel — hanya data yang bukan `NaN` yang diakumulasi.
4. Setelah 5 sampel terkumpul, hitung rata-rata dan tampilkan hasilnya.
5. Jika seluruh sampel gagal terbaca, tampilkan pesan kegagalan alih-alih membagi dengan nol.

---

## Penjelasan Baris yang Ditambahkan/Diubah

### 1. Inisialisasi Variabel Akumulator dan Pencatat Sampel Valid

```cpp
float totalSuhu = 0;
float totalKelembaban = 0;
int sampelValid = 0;
```

Di awal fungsi `loop()`, tiga variabel lokal ini dibuat dan diisi nilai awal `0`:

- **`totalSuhu`** dan **`totalKelembaban`** (bertipe `float`) — menampung penjumlahan (akumulasi) nilai suhu dan kelembaban dari sampel-sampel yang valid.
- **`sampelValid`** (bertipe `int`) — menghitung berapa banyak pembacaan sensor yang berhasil (bukan `NaN`). Variabel ini penting sebagai penyebut saat pembagian rata-rata, agar hasil perhitungan tetap akurat meskipun ada sampel yang gagal terbaca.

### 2. Perulangan Pengambilan Sampel (`for` loop)

```cpp
for (int i = 0; i < 5; i++) {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
```

Perulangan `for` dijalankan sebanyak 5 kali (`i = 0` sampai `i = 4`) untuk melakukan proses akuisisi data berulang secara otomatis. Di dalamnya, `dht.readHumidity()` dan `dht.readTemperature()` dipanggil untuk mengambil nilai instan saat itu.

### 3. Validasi Sampel dan Kalkulasi Akumulasi

```cpp
if (!isnan(h) && !isnan(t)) {
  totalSuhu += t;
  totalKelembaban += h;
  sampelValid++;
} else {
  Serial.println("Peringatan: Ada pembacaan sampel yang gagal!");
}
```

- Pengecekan `if (!isnan(h) && !isnan(t))` memastikan hanya data valid yang ditambahkan ke penampung.
- Jika valid, `t` dan `h` ditambahkan ke `totalSuhu` dan `totalKelembaban`, lalu `sampelValid` bertambah 1.
- Jika data `NaN` (gagal), program **tidak** menjumlahkan data tersebut dan menampilkan peringatan di Serial Monitor tanpa menghentikan proses pengambilan sampel lainnya.

### 4. Jeda Antar-Pengambilan Sampel

```cpp
delay(2000); // Jeda 2 detik antar sampel pembacaan
```

Diletakkan di dalam perulangan `for`, baris ini menjaga jarak interval waktu antar-pengambilan sampel sebesar 2 detik — sesuai spesifikasi teknis sensor DHT22 yang memerlukan interval minimal 2 detik untuk memperbarui data internalnya.

### 5. Perhitungan dan Penyajian Data Rata-Rata

```cpp
if (sampelValid > 0) {
  float rataSuhu = totalSuhu / sampelValid;
  float rataKelembaban = totalKelembaban / sampelValid;

  Serial.print("Hasil Rata-Rata ");
  Serial.print(sampelValid);
  Serial.println(" sampel ");
  Serial.print("Suhu Rata-rata       : ");
  Serial.print(rataSuhu);
  Serial.println(" °C");
  Serial.print("Kelembaban Rata-rata : ");
  Serial.print(rataKelembaban);
  Serial.println(" %");
} else {
  Serial.println("Gagal total: Tidak ada data sampel yang valid!");
}
```

Setelah perulangan `for` selesai mengambil 5 sampel:

- `if (sampelValid > 0)` memastikan pembagian rata-rata hanya dieksekusi jika minimal ada 1 sampel valid (menghindari error *division by zero* jika semua sampel gagal).
- Nilai rata-rata dihitung dengan membagi `totalSuhu` dan `totalKelembaban` dengan `sampelValid`.
- Hasil akhir dicetak ke Serial Monitor. Jika seluruh 5 sampel bernilai `NaN`, cabang `else` mencetak pesan bahwa seluruh proses sampling gagal.

---

## Ringkasan Perubahan

| Aspek | Program Asli | Program Modifikasi |
|---|---|---|
| Frekuensi tampil data | Setiap pembacaan (tiap 2 detik) | Setelah 5 sampel terkumpul |
| Penanganan data gagal (`NaN`) | Tidak ada validasi eksplisit | Divalidasi & dihitung sebagai `sampelValid` |
| Hasil yang ditampilkan | Nilai instan | Nilai rata-rata |
| Ketahanan terhadap noise | Rendah | Lebih tinggi |
