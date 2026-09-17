#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// Konfigurasi WiFi
const char* ssid = "faqof";       // Ganti dengan SSID WiFi kamu
const char* password = "imds1357"; // Ganti dengan Password WiFi kamu

// Endpoint Uji HTTP POST
const char* serverUrl = "https://httpbin.org/post";

void setup() {
  Serial.begin(115200); // Inisialisasi komunikasi serial baud rate 115200

  // Koneksi ke WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi berhasil terhubung!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClientSecure client;
    client.setInsecure();

    http.begin(client, serverUrl); // Inisialisasi koneksi HTTP ke URL server

    // Set header Request agar server tahu format data adalah JSON
    http.addHeader("Content-Type", "application/json");

    // Membuat objek data sensor dalam format JSON menggunakan ArduinoJson
    JsonDocument doc;
    doc["suhu"] = 28.5;         // Contoh data suhu (°C)
    doc["kelembaban"] = 65.0;   // Contoh data kelembaban (%)
    doc["waktu"] = millis();    // Waktu sejak ESP dinyalakan (ms)

    String requestBody;
    serializeJson(doc, requestBody); // Konversi objek JSON ke bentuk String

    Serial.print("Mengirim data: ");
    Serial.println(requestBody);

    // Mengirim data melalui HTTP POST
    int httpResponseCode = http.POST(requestBody);

    // Memeriksa respon dari server
    if (httpResponseCode > 0) {
      Serial.print("Kode Response HTTP: ");
      Serial.println(httpResponseCode);
      Serial.println("Isi Response:");
      Serial.println(http.getString()); // Tampilkan balasan dari server
    } else {
      Serial.print("Pengiriman gagal, kode error: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Menutup koneksi HTTP
  }

  delay(10000); // Kirim data setiap 10 detik
}