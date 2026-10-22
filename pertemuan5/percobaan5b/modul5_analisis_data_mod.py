import pandas as pd
import matplotlib.pyplot as plt

# ==========================================================
# MODIFIKASI PROGRAM: Analisis Deteksi Anomali Berbasis Z-Score
# dengan Penerapan Moving Average (Jendela 5 Data) pada File CSV
# ==========================================================

# Membaca dataset hasil akuisisi sebelumnya dari data_suhu.csv
df = pd.read_csv("data_suhu.csv")

# ----------------------------------------------------------
# 1. Analisis pada Data Mentah (Sebelum Filter)
# ----------------------------------------------------------
rata2_raw = df["suhu"].mean()
std_raw = df["suhu"].std()

if std_raw == 0:
    df["zscore_raw"] = 0.0
else:
    df["zscore_raw"] = (df["suhu"] - rata2_raw) / std_raw

df["anomali_raw"] = df["zscore_raw"].abs() > 2

# ----------------------------------------------------------
# 2. [MODIFIKASI] Penerapan Moving Average (Jendela 5 Data)
# ----------------------------------------------------------
# Menghitung rata-rata bergerak jendela 5 data menggunakan rolling()
df["suhu_ma"] = df["suhu"].rolling(window=5, min_periods=1).mean()

# Menghitung statistik deskriptif dari data moving average
rata2_ma = df["suhu_ma"].mean()
std_ma = df["suhu_ma"].std()

# Menghitung Z-Score pada data yang sudah di-filter Moving Average
if std_ma == 0:
    df["zscore_ma"] = 0.0
else:
    df["zscore_ma"] = (df["suhu_ma"] - rata2_ma) / std_ma

# Deteksi anomali pada data moving average dengan ambang |z| > 2
df["anomali_ma"] = df["zscore_ma"].abs() > 2

# ----------------------------------------------------------
# 3. Menampilkan Perbandingan Hasil Analisis di Konsol/Terminal
# ----------------------------------------------------------
print("=" * 55)
print("HASIL ANALISIS DATA MENTAH (RAW)")
print("=" * 55)
print(f"Rata-rata Suhu Mentah   : {rata2_raw:.2f} °C")
print(f"Std Deviasi Mentah      : {std_raw:.2f}")
print(f"Jumlah Anomali Mentah   : {df['anomali_raw'].sum()} data")
if df["anomali_raw"].sum() > 0:
    print(df[df["anomali_raw"]][["waktu", "suhu", "zscore_raw"]])

print("\n" + "=" * 55)
print("HASIL ANALISIS DENGAN MOVING AVERAGE (WINDOW = 5)")
print("=" * 55)
print(f"Rata-rata Suhu MA (N=5) : {rata2_ma:.2f} °C")
print(f"Std Deviasi Suhu MA     : {std_ma:.2f}")
print(f"Jumlah Anomali MA       : {df['anomali_ma'].sum()} data")
if df["anomali_ma"].sum() > 0:
    print(df[df["anomali_ma"]][["waktu", "suhu_ma", "zscore_ma"]])
else:
    print("-> Tidak ada anomali yang terdeteksi setelah difilter.")
print("=" * 55)

# ----------------------------------------------------------
# 4. [MODIFIKASI] Visualisasi Grafik Komparasi
# ----------------------------------------------------------
plt.figure(figsize=(12, 6))

# Plot kurva data mentah dan data filter moving average
plt.plot(df.index, df["suhu"], marker="o", color="#1f77b4", alpha=0.6, label="Suhu Mentah (Raw)")
plt.plot(df.index, df["suhu_ma"], marker="s", color="#2ca02c", linewidth=2, label="Moving Average (N=5)")

# Sorot titik anomali pada data mentah dengan warna merah
anomali_mentah = df[df["anomali_raw"]]
plt.scatter(anomali_mentah.index, anomali_mentah["suhu"], color="#d62728", s=130, zorder=5, label="Anomali Data Mentah (|z| > 2)")

# Garis rata-rata
plt.axhline(rata2_raw, color="#7f7f7f", linestyle="--", alpha=0.7, label=f"Rata-rata Mentah ({rata2_raw:.2f}°C)")
plt.axhline(rata2_ma, color="#2ca02c", linestyle=":", alpha=0.7, label=f"Rata-rata MA ({rata2_ma:.2f}°C)")

plt.title("Perbandingan Sinyal Suhu dan Deteksi Anomali Z-Score (Raw vs Moving Average N=5)", fontsize=13, fontweight="bold")
plt.xlabel("Indeks Urutan Sampel Data", fontsize=11)
plt.ylabel("Suhu (°C)", fontsize=11)
plt.grid(True, linestyle="--", alpha=0.5)
plt.legend(loc="upper left")
plt.tight_layout()

# Simpan hasil grafik
plt.savefig("grafik_perbandingan_ma.png", dpi=300)
print("\nGrafik perbandingan berhasil disimpan sebagai 'grafik_perbandingan_ma.png'")
# plt.show() # Uncomment jika ingin menampilkan jendela plot secara interaktif
