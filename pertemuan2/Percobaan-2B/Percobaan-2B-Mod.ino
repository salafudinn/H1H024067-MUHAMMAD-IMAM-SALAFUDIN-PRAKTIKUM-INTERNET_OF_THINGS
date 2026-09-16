#include <WiFi.h>

// Kredensial untuk Mode Station (Terhubung ke WiFi Rumah/Hotspot)
const char* sta_ssid = "NAMA_WIFI_RUMAH";
const char* sta_password = "PASSWORD_WIFI_RUMAH";

// Kredensial untuk Mode Access Point (Hotspot lokal ESP32)
const char* ap_ssid = "ESP32_AccessPoint";
const char* ap_password = "12345678"; // Minimal 8 karakter

const int ledPin = 2; // LED indikator koneksi Station

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Set mode WiFi menjadi kombinasi Access Point dan Station
  WiFi.mode(WIFI_AP_STA);

  // 1. Mengaktifkan Access Point
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress apIP = WiFi.softAPIP();
  
  Serial.println("=== Access Point Aktif ===");
  Serial.print("SSID AP      : ");
  Serial.println(ap_ssid);
  Serial.print("IP Address AP: ");
  Serial.println(apIP);
  Serial.println("==========================");

  // 2. Menghubungkan ke jaringan WiFi Station
  WiFi.begin(sta_ssid, sta_password);
  Serial.print("Menghubungkan ke WiFi Station (");
  Serial.print(sta_ssid);
  Serial.println(")...");

  // Timeout koneksi Station (maksimal 10 detik/20 iterasi)
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED && counter < 20) {
    delay(500);
    Serial.print(".");
    counter++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("=== Station Terhubung ===");
    Serial.print("IP Address STA : ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC Address    : ");
    Serial.println(WiFi.macAddress());
    Serial.print("RSSI (dBm)     : ");
    Serial.println(WiFi.RSSI());
    Serial.println("=========================");
    digitalWrite(ledPin, HIGH);
  } else {
    Serial.println();
    Serial.println("[Peringatan] Gagal terhubung ke Station, AP tetap berjalan.");
  }
}

void loop() {
  // Menampilkan jumlah client yang terhubung ke AP ESP32
  int jumlahClient = WiFi.softAPgetStationNum();
  Serial.print("[AP] Perangkat terhubung: ");
  Serial.println(jumlahClient);

  // Memeriksa status koneksi Station
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[STA] Terhubung | IP STA: ");
    Serial.println(WiFi.localIP());
    digitalWrite(ledPin, HIGH);
  } else {
    Serial.println("[STA] Terputus! Memproses reconnect...");
    digitalWrite(ledPin, LOW);
    WiFi.reconnect();
  }

  delay(5000);
}