# Modifikasi Program: Penambahan Data Waktu (millis()) pada JSON HTTP POST

## Perubahan yang Dilakukan
Program dasar HTTP POST dimodifikasi agar mengirimkan satu field tambahan
berupa waktu sejak board dinyalakan (dalam milidetik), diambil menggunakan
fungsi `millis()`, dan disertakan ke dalam JSON yang dikirim melalui HTTP
POST.

## Baris yang Ditambahkan

```cpp
doc["waktu"] = millis();
```

Baris ini disisipkan setelah baris `doc["kelembaban"] = 65.0;` dan sebelum
proses `serializeJson(doc, requestBody);`.

## Penjelasan

| Bagian | Penjelasan |
|---|---|
| `millis()` | Fungsi bawaan Arduino yang mengembalikan jumlah milidetik sejak board terakhir kali dinyalakan atau di-reset, dalam tipe data `unsigned long`. |
| `doc["waktu"]` | Membuat key baru bernama `"waktu"` pada objek `JsonDocument` yang sama dengan yang menampung data `suhu` dan `kelembaban`. |
| `doc["waktu"] = millis();` | Mengisi key `"waktu"` dengan nilai `millis()` saat data dikirim, sehingga setiap data memiliki penanda waktu (timestamp relatif) kapan data tersebut dibuat. |

## Efek pada Data yang Dikirim

Sebelum modifikasi, JSON yang dikirim:
```json
{"suhu":28.5,"kelembaban":65.0}
```

Setelah modifikasi, JSON yang dikirim menjadi:
```json
{"suhu":28.5,"kelembaban":65.0,"waktu":123456}
```

Nilai `waktu` akan terus bertambah setiap kali `loop()` berjalan (setiap 10
detik) karena dihitung berdasarkan waktu sejak board mulai berjalan, bukan
waktu real (jam dinding), sehingga cocok digunakan untuk mengetahui jeda
antar pengiriman data, bukan untuk mengetahui tanggal/jam saat ini.