#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// --- KETENTUAN JARINGAN ---
const char* ssid          = "oyell";
const char* password      = "12345678";
const char* mqttServer    = "broker.hivemq.com";
const int   mqttPort      = 1883;

// --- TOPIC MQTT ---
const char* topicData     = "unsoed/tk245004/kelompok6/data";
const char* topicPerintah = "unsoed/tk245004/kelompok6/sensor";

// --- PINS & HARDWARE ESP8266 ---
#define DHTPIN D2     // Pin D2 (GPIO 4) untuk Sensor DHT
#define DHTTYPE DHT11 // Ganti ke DHT22 jika sensor fisik kamu DHT22
const int ledPin = D1; // Pin D1 (GPIO 5) untuk LED Indikator

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

// Timing Variabel untuk Non-blocking Delay (millis)
unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000; // Publish data suhu setiap 5 detik (5000 ms)

// Fungsi Callback: Dipanggil otomatis saat pesan perintah diterima
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan = "";
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  JsonDocument doc;
  if (deserializeJson(doc, pesan)) return; // Abaikan jika format JSON tidak valid

  const char* perintah = doc["perintah"];
  digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW);
  
  Serial.print("Perintah diterima -> Aktuator: ");
  Serial.println(perintah);
}

void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

void hubungkanMQTT() {
  while (!client.connected()) {
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe(topicPerintah); // Subscribe ke topik perintah
      Serial.println("Terhubung ke MQTT & Subscribe topic perintah!");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  dht.begin();

  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    hubungkanMQTT();
  }
  client.loop(); // Memproses pesan masuk secara real-time tanpa memblokir sistem

  // Publish data sensor secara berkala menggunakan millis() (Non-blocking)
  if (millis() - waktuTerakhirPublish > intervalPublish) {
    waktuTerakhirPublish = millis();

    float suhu = dht.readTemperature();

    // Pastikan pembacaan sensor valid
    if (!isnan(suhu)) {
      JsonDocument doc;
      doc["suhu"] = suhu;

      char buffer[128];
      serializeJson(doc, buffer);

      client.publish(topicData, buffer);
      Serial.print("Data terkirim ke MQTT: ");
      Serial.println(buffer);
    } else {
      Serial.println("Gagal membaca data dari sensor DHT!");
    }
  }
}