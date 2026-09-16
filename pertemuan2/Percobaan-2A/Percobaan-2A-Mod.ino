#include <WiFi.h>

const char* ssid = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

const int ledPin = 2; // LED indikator status koneksi (GPIO2)

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Set mode WiFi menjadi Station (STA)
  WiFi.mode(WIFI_STA);
  
  // Mengaktifkan fitur auto-reconnect bawaan stack WiFi ESP32
  WiFi.setAutoReconnect(true);

  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Jika berhasil terhubung
  Serial.println();
  Serial.println("WiFi berhasil terhubung!");
  Serial.print("IP Address  : ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address : ");
  Serial.println(WiFi.macAddress());
  Serial.print("RSSI (dBm)  : ");
  Serial.println(WiFi.RSSI());
  
  digitalWrite(ledPin, HIGH); // Menyengalakan LED indikator koneksi
}

void loop() {
  // Memeriksa status koneksi WiFi setiap 5 detik
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Status: Terhubung ke WiFi");
    digitalWrite(ledPin, HIGH); // Memastikan LED tetap menyala saat terhubung
  } else {
    Serial.println("Status: Terputus! Mencoba reconnect...");
    digitalWrite(ledPin, LOW);  // Mematikan LED indikator karena koneksi terputus
    
    // Memanggil reconnect secara eksplisit jika auto-reconnect bawaan memerlukan trigger
    WiFi.reconnect();
  }

  delay(5000);
}