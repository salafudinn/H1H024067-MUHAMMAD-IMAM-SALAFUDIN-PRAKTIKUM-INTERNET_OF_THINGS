# Modifikasi Program Praktikum IoT — Akuisisi Data Sensor & Kendali Histerisis

Dokumen ini berisi penjelasan detail baris-baris kode yang ditambahkan/diubah pada **Percobaan 1A (Akuisisi Data Sensor Rata-Rata 5 Sampel)** dan **Percobaan 2A (Kendali Aktuator Histerisis Dua Ambang Batas)**.

---

## Part 1: Modifikasi Program Akuisisi Data Sensor DHT22 (Rata-Rata 5 Sampel)

Program asli membaca dan menampilkan data suhu serta kelembaban secara langsung setiap 2 detik sekali. Modifikasi ini mengubah alur program agar mengumpulkan **5 sampel data terlebih dahulu**, menghitung nilai rata-ratanya, baru kemudian menyajikan hasil rata-rata tersebut ke Serial Monitor. Hal ini berguna untuk mereduksi *noise* atau fluktuasi pembacaan acak pada sensor.

### Penjelasan baris-baris yang ditambahkan/diubah

#### 1. Inisialisasi variabel akumulator dan pencatat sampel valid
```cpp
float totalSuhu = 0;
float totalKelembaban = 0;
int sampelValid = 0;
Di awal fungsi loop(), tiga variabel lokal ini dibuat dan diisi nilai awal 0:totalSuhu dan totalKelembaban bertipe float berfungsi untuk menampung penjumlahan (akumulasi) nilai suhu dan kelembaban dari sampel-sampel yang valid.sampelValid bertipe int berfungsi menghitung berapa banyak pembacaan sensor yang berhasil (bukan NaN). Variabel ini sangat penting sebagai penyebut saat pembagian rata-rata agar hasil perhitungan tetap akurat meskipun ada sampel yang gagal terbaca.2. Perulangan pengambil sampel (for loop)C++for (int i = 0; i < 5; i++) {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
Perulangan for dijalankan sebanyak 5 kali (i = 0 sampai i = 4) untuk melakukan proses akuisisi data berulang secara otomatis. Di dalam perulangan ini, fungsi dht.readHumidity() dan dht.readTemperature() dipanggil untuk mengambil nilai instan saat itu.3. Validasi sampel dan kalkulasi akumulasiC++if (!isnan(h) && !isnan(t)) {
  totalSuhu += t;
  totalKelembaban += h;
  sampelValid++;
} else {
  Serial.println("Peringatan: Ada pembacaan sampel yang gagal!");
}
Pengecekan if (!isnan(h) && !isnan(t)) memastikan bahwa hanya data yang valid saja yang ditambahkan ke penampung.Jika data valid, nilai t dan h ditambahkan ke totalSuhu dan totalKelembaban, lalu sampelValid bertambah +1.Jika data bernilai NaN (gagal), program tidak menjumlahkan data tersebut dan menampilkan peringatan di Serial Monitor tanpa menghentikan proses perulangan sampel lainnya.  4. Jeda antar-pengambilan sampelC++delay(2000); // Jeda 2 detik antar sampel pembacaan
Baris delay(2000) diletakkan di dalam perulangan for. Hal ini menjaga jarak interval waktu antar-pengambilan sampel sebesar 2 detik, sesuai spesifikasi teknis sensor DHT22 yang memerlukan interval waktu minimal 2 detik untuk memperbarui data internalnya.5. Perhitungan dan penyajikan data rata-rataC++if (sampelValid > 0) {
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
Setelah perulangan for selesai mengambil 5 sampel:Blok if (sampelValid > 0) memastikan pembagian rata-rata hanya dieksekusi jika minimal ada 1 sampel yang valid (menghindari error matematika division by zero jika semua sampel gagal).Nilai rata-rata dihitung dengan membagi totalSuhu dan totalKelembaban menggunakan pembagi sampelValid.Hasil akhir rata-rata kemudian dicetak ke Serial Monitor. Jika seluruh 5 sampel bernilai NaN, cabang else akan mencetak pesan bahwa seluruh proses sampling gagal.