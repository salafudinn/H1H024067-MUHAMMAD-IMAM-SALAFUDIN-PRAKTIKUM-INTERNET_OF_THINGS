# Penjelasan Modifikasi Kode Percobaan 5A (Perhitungan Simpangan Baku)

Dokumen ini berisi dokumentasi dan penjelasan rinci setiap baris kode yang ditambahkan pada program `percobaan5a_mod.ino` guna memenuhi **Pertanyaan Praktikum 5A Nomor 4**:
> *"Modifikasi program agar ESP32 juga menghitung simpangan baku dari data pada buffer, dan berikan penjelasan di setiap baris kode yang ditambahkan dalam bentuk README.md."*

---

## 1. Rumus Matematika yang Diimplementasikan
Simpangan baku (*standard deviation*) mengukur sebaran data di dalam buffer terhadap nilai rata-ratanya:
$$\sigma = \sqrt{\frac{1}{N} \sum_{i=0}^{N-1} (x_i - \bar{x})^2}$$

Di mana:
- $x_i$ = nilai sampel suhu ke-$i$ di dalam array `buffer`
- $\bar{x}$ = rata-rata bergerak (*moving average*) yang dihasilkan oleh fungsi `hitungMovingAverage()`
- $N$ = jumlah sampel valid yang tersimpan di dalam buffer saat ini (`jumlah`)

---

## 2. Rincian Baris Kode yang Ditambahkan dan Penjelasannya

### A. Penyertaan Header Matematika
```cpp
#include <math.h> // [MODIFIKASI] Pustaka untuk operasi matematika sqrt() dan pow()
```
* **Penjelasan:** Mengimpor pustaka standar matematika C/C++ agar mikrokontroler dapat mengenali fungsi `pow()` (untuk operasi pemangkatan kuadrat) dan `sqrt()` (untuk operasi penarikan akar kuadrat).

---

### B. Implementasi Fungsi `hitungSimpanganBaku()`
```cpp
float hitungSimpanganBaku(float rataRata) {
```
* **Penjelasan:** Mendeklarasikan fungsi `hitungSimpanganBaku` bertipe pengembalian `float` dengan menerima satu parameter input `rataRata` (berupa nilai rata-rata suhu filter saat ini).

```cpp
  if (jumlah <= 1) {
    return 0.0;
  }
```
* **Penjelasan:** Pengaman (*guard clause*) logis. Jika buffer masih kosong atau baru terisi 1 sampel data, nilai deviasi belum dapat didefinisikan (variansi = 0), sehingga fungsi langsung mengembalikan nilai `0.0` guna mencegah komputasi sia-sia.

```cpp
  float jumlahKuadratSelisih = 0.0;
```
* **Penjelasan:** Menginisialisasi variabel akumulator bertipe `float` dengan nilai awal `0.0` untuk menampung jumlahan kuadrat deviasi $\sum (x_i - \bar{x})^2$.

```cpp
  for (int i = 0; i < jumlah; i++) {
    jumlahKuadratSelisih += pow(buffer[i] - rataRata, 2);
  }
```
* **Penjelasan:** Perulangan `for` untuk mengiterasi setiap elemen aktif pada `buffer` dari indeks `0` sampai `jumlah - 1`. Pada setiap iterasi, selisih data ke-$i$ terhadap `rataRata` dikuadratkan dengan fungsi `pow(..., 2)` lalu ditambahkan ke variabel `jumlahKuadratSelisih`.

```cpp
  return sqrt(jumlahKuadratSelisih / jumlah);
}
```
* **Penjelasan:** Menghitung variansi dengan membagi `jumlahKuadratSelisih` dengan `jumlah` (banyaknya sampel dalam buffer), kemudian menarik akar kuadratnya menggunakan fungsi `sqrt()`, lalu mengembalikan hasilnya sebagai nilai simpangan baku.

---

### C. Pemanggilan Fungsi dan Penampilan di Serial Monitor pada `loop()`
```cpp
    float simpanganBaku = hitungSimpanganBaku(suhuFilter);
```
* **Penjelasan:** Memanggil fungsi `hitungSimpanganBaku` dengan mengoper argumen `suhuFilter` (nilai rata-rata bergerak yang baru saja dihitung), kemudian menyimpan hasilnya ke variabel lokal `simpanganBaku`.

```cpp
    Serial.print(" | StdDev: ");
    Serial.print(simpanganBaku);
```
* **Penjelasan:** Mencetak label teks `" | StdDev: "` dan nilai numerik variabel `simpanganBaku` ke Serial Monitor sehingga pengguna dapat memantau tingkat kestabilan/variabilitas data suhu secara *real-time*.

---

## 3. Contoh Output Serial Monitor
```text
Mentah: 28.12 | Filter: 28.12 | StdDev: 0.00 | Min: 28.12 | Max: 28.12
Mentah: 28.25 | Filter: 28.18 | StdDev: 0.07 | Min: 28.12 | Max: 28.25
Mentah: 28.05 | Filter: 28.14 | StdDev: 0.08 | Min: 28.05 | Max: 28.25
...
Mentah: 36.40 | Filter: 29.80 | StdDev: 3.32 | Min: 27.90 | Max: 36.40
```
*Saat terjadi lonjakan anomali ($36.40^\circ\text{C}$), nilai `StdDev` seketika naik drastis (misal menjadi $3.32$), menandakan adanya variabilitas ekstrem dalam buffer.*
