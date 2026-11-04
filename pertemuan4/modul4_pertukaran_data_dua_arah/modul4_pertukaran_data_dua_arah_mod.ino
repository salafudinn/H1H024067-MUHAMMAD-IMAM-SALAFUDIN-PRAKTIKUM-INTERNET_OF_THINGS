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
const char* topicPerintah = "unsoed/tk245004/kelompok6/sensor"; // Topic untuk aktuator 1 (LED)
// [MODIFIKASI] Menambahkan topic perintah baru untuk mengendalikan aktuator kedua (Buzzer)
const char* topicBuzzer   = "unsoed/tk245004/kelompok6/buzzer"; // Topic untuk aktuator 2 (Buzzer)

// --- PINS & HARDWARE ESP8266 ---
#define DHTPIN D2      // Pin D2 (GPIO 4) untuk Sensor DHT
#define DHTTYPE DHT11  // Tipe sensor DHT11
const int ledPin    = D1; // Pin D1 (GPIO 5) untuk LED Indikator
// [MODIFIKASI] Mendefinisikan pin aktuator kedua (Buzzer) pada pin D5 (GPIO 14)
const int buzzerPin = D5; 

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

// Timing Variabel untuk Non-blocking Delay (millis)
unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000; // Publish data suhu setiap 5 detik (5000 ms)

// Fungsi Callback: Menangani pesan MQTT yang masuk dan membedakan aksi berdasarkan topic
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan = "";
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  // [MODIFIKASI] Mencetak nama topic pengirim dan payload isi pesan ke Serial Monitor
  Serial.print("Pesan diterima pada topic [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(pesan);

  JsonDocument doc;
  if (deserializeJson(doc, pesan)) {
    Serial.println("Gagal parsing JSON!");
    return; // Abaikan jika format JSON tidak valid
  }

  const char* perintah = doc["perintah"];

  // [MODIFIKASI] Membedakan instruksi berdasarkan topic yang menerima pesan
  if (strcmp(topic, topicPerintah) == 0) {
    // --- AKTUATOR 1: KONTROL LED ---
    if (String(perintah) == "ON") {
      digitalWrite(ledPin, HIGH);
      Serial.println("Aktuator LED -> ON");
    } else if (String(perintah) == "OFF") {
      digitalWrite(ledPin, LOW);
      Serial.println("Aktuator LED -> OFF");
    }
  } else if (strcmp(topic, topicBuzzer) == 0) {
    // --- AKTUATOR 2: KONTROL BUZZER ---
    if (String(perintah) == "ON") {
      digitalWrite(buzzerPin, HIGH);
      Serial.println("Aktuator Buzzer -> ON");
    } else if (String(perintah) == "OFF") {
      digitalWrite(buzzerPin, LOW);
      Serial.println("Aktuator Buzzer -> OFF");
    }
  }
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
      // Subscribe ke topic perintah LED (Aktuator 1)
      client.subscribe(topicPerintah);
      // [MODIFIKASI] Mendaftarkan subscribe ke topic perintah Buzzer (Aktuator 2)
      client.subscribe(topicBuzzer);
      Serial.println("Terhubung ke MQTT & Berhasil Subscribe topic LED dan Buzzer!");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // [MODIFIKASI] Inisialisasi pin Buzzer sebagai OUTPUT dan kondisi awal mati (LOW)
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

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

