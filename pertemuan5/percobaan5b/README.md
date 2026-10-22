# Penjelasan Modifikasi Kode Percobaan 5B (Moving Average Jendela 5 Data & Perbandingan Deteksi Anomali Z-Score)

Dokumen ini berisi dokumentasi dan rincian penjelasan setiap baris kode yang ditambahkan pada skrip `modul5_analisis_data_mod.py` untuk memenuhi **Pertanyaan Praktikum 5B Nomor 4**:
> *"Modifikasi program agar menggunakan metode moving average (jendela 5 data) pada data hasil CSV sebelum dihitung z-score-nya, kemudian bandingkan jumlah anomali yang terdeteksi, dan berikan penjelasan di setiap baris kode yang ditambahkan dalam bentuk README.md."*

---

## 1. Latar Belakang Komputasi
Penerapan filter **Moving Average (MA)** dengan ukuran jendela $N=5$ dilakukan terhadap kolom data suhu sebelum menghitung statistik *Z-Score*. Tujuannya adalah meredam komponen derau impulsif frekuensi tinggi (*noise spikes*) sehingga sebaran data menjadi lebih teratur.

Rumus rata-rata bergerak:
$$\bar{x}_k = \frac{1}{M} \sum_{i=0}^{M-1} x_{k-i}$$
*(dengan $M = \min(k+1, 5)$ pada awal iterasi data).*

---

## 2. Rincian Baris Kode yang Ditambahkan dan Penjelasannya

### A. Perhitungan Moving Average pada Dataframe
```python
df["suhu_ma"] = df["suhu"].rolling(window=5, min_periods=1).mean()
```
* **Penjelasan:** Menggunakan metode `.rolling()` dari pustaka Pandas pada kolom `df["suhu"]`.
  - `window=5`: Menentukan ukuran jendela moving average sebanyak 5 titik data sampel.
  - `min_periods=1`: Memastikan bahwa pada data awal (indeks 0 sampai 3 di mana jumlah sampel belum mencapai 5), rata-rata tetap dihitung dari sampel yang tersedia sehingga tidak menghasilkan nilai `NaN` (*Not a Number*).
  - `.mean()`: Menghitung nilai rata-rata dari jendela data yang sedang bergeser.

---

### B. Perhitungan Parameter Statistik Sinyal Hasil Filter
```python
rata2_ma = df["suhu_ma"].mean()
std_ma = df["suhu_ma"].std()
```
* **Penjelasan:**
  - `rata2_ma`: Menghitung nilai rata-rata keseluruhan dari data yang sudah di-filter moving average.
  - `std_ma`: Menghitung simpangan baku (*standard deviation*) dari kolom `suhu_ma`. Nilai ini menyusut dari $1.82$ (pada data mentah) menjadi $1.23$ akibat peredaman fluktuasi ekstrem.

---

### C. Perhitungan Z-Score dan Penentuan Anomali Data Filter
```python
if std_ma == 0:
    df["zscore_ma"] = 0.0
else:
    df["zscore_ma"] = (df["suhu_ma"] - rata2_ma) / std_ma
```
* **Penjelasan:** Menghitung nilai z-score standar untuk tiap baris data filter moving average: $z = \frac{x_{ma} - \mu_{ma}}{\sigma_{ma}}$. Percabangan `if std_ma == 0:` melindungi program dari kemungkinan pembagian dengan nol (*division by zero*).

```python
df["anomali_ma"] = df["zscore_ma"].abs() > 2
```
* **Penjelasan:** Mengevaluasi kondisi boolean apakah nilai absolut z-score $|z| > 2$. Jika terpenuhi, data diklasifikasikan sebagai anomali (`True`), jika tidak maka normal (`False`).

---

### D. Visualisasi Komparasi Grafis
```python
plt.plot(df.index, df["suhu_ma"], marker="s", color="#2ca02c", linewidth=2, label="Moving Average (N=5)")
```
* **Penjelasan:** Menggambar kurva hasil filter moving average dengan warna hijau solid, penanda kotak (*square marker*), dan ketebalan garis 2 agar perbedaannya dengan kurva suhu mentah terlihat jelas.

```python
plt.savefig("grafik_perbandingan_ma.png", dpi=300)
```
* **Penjelasan:** Menyimpan visualisasi grafik perbandingan sinyal mentah vs sinyal terfilter beserta sorotan anomali ke file gambar resolusi tinggi (300 DPI).

---

## 3. Hasil Perbandingan Deteksi Anomali

Berdasarkan dataset pengamatan aktual [data_suhu.csv](data_suhu.csv):

| Metrik Evaluasi | Data Mentah (Raw) | Setelah Moving Average ($N=5$) |
| :--- | :--- | :--- |
| **Rata-rata ($\mu$)** | $28.75^\circ\text{C}$ | $28.50^\circ\text{C}$ |
| **Simpangan Baku ($\sigma$)** | $1.82$ | $1.23$ |
| **Nilai Ekstrem Maksimum** | $35.84^\circ\text{C}$ (Indeks 8) | $28.97^\circ\text{C}$ |
| **Nilai Z-Score Tertinggi** | **$z = 3.89$** | **$z = 1.84$** |
| **Jumlah Anomali Terdeteksi ($|z| > 2$)** | **1 Data** | **0 Data** |

### Analisis Hasil Komparasi:
Pada data mentah, lonjakan buatan sebesar $35.84^\circ\text{C}$ (akibat simulasi `suhu += 8.0`) terdeteksi sebagai anomali karena memiliki nilai $z = 3.89 > 2$. 
Namun, setelah diterapkan Moving Average dengan ukuran jendela 5, lonjakan tersebut diratakan bersama 4 data di sekitarnya sehingga nilai suhu pada indeks tersebut teredam menjadi $28.97^\circ\text{C}$. Akibatnya, nilai z-score tertingginya turun menjadi $1.84$ yang berada di bawah ambang batas penentu anomali ($|z| \le 2$), sehingga **tidak ada anomali yang terdeteksi**.
Hal ini membuktikan bahwa filter Moving Average sangat efektif mengeliminasi derau impulsif tunggal (*isolated spike*), namun perlu diperhatikan agar tidak menyamarkan anomali nyata yang bersifat kritis.
