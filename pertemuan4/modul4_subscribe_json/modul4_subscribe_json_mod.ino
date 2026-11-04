#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid          = "akumw5";
const char* password      = "12345678";
const char* mqttServer    = "broker.hivemq.com";
const int   mqttPort      = 1883;
const char* topicPerintah = "unsoed/tk245004/kelompok6/sensor";

const int ledPin = D1; // Pin D1 (GPIO 5) untuk LED Indikator / Aktuator PWM

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String pesan = "";
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(pesan);

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, pesan);

  if (error) {
    Serial.print("Gagal parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  const char* perintah = doc["perintah"];

  // [MODIFIKASI] Mengambil nilai intensitas kecerahan PWM dari key "intensitas"
  // Nilai default 255 (maksimal) digunakan jika key "intensitas" tidak disertakan
  int intensitas = doc["intensitas"] | 255;

  if (String(perintah) == "ON") {
    // [MODIFIKASI] Membatasi nilai intensitas ke rentang aman 0 - 255
    intensitas = constrain(intensitas, 0, 255);
    // [MODIFIKASI] Mengatur kecerahan LED menggunakan sinyal PWM analogWrite
    analogWrite(ledPin, intensitas);
    Serial.print("Aktuator: ON | Intensitas PWM: ");
    Serial.println(intensitas);
  } else if (String(perintah) == "OFF") {
    // [MODIFIKASI] Mematikan LED sepenuhnya dengan duty cycle PWM 0
    analogWrite(ledPin, 0);
    Serial.println("Aktuator: OFF");
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
    Serial.print("Menghubungkan ke broker MQTT...");
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
      client.subscribe(topicPerintah);
      Serial.print("Subscribe ke topic: ");
      Serial.println(topicPerintah);
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  // [MODIFIKASI] Mengatur rentang resolusi PWM ke 0 - 255 (standar 8-bit)
  analogWriteRange(255);
  // [MODIFIKASI] Memastikan kondisi awal LED mati dengan nilai duty cycle 0
  analogWrite(ledPin, 0);

  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    hubungkanMQTT();
  }
  client.loop();
}

