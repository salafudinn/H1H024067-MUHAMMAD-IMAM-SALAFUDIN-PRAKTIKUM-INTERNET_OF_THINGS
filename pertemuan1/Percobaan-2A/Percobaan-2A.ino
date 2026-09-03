#include <DHT.h>
#define DHTPIN 4 // pin data DHT22 terhubung ke GPIO 4
#define DHTTYPE DHT11
#define RELAYPIN 14 // pin kendali relay/LED indikator
DHT dht(DHTPIN, DHTTYPE);
const float suhuThreshold = 26.0; // ambang batas suhu (°C)
void setup() {
Serial.begin(115200);
dht.begin();
pinMode(RELAYPIN, OUTPUT);
digitalWrite(RELAYPIN, LOW); // pastikan aktuator mati di awal
}
void loop() {
float suhu = dht.readTemperature();
if (isnan(suhu)) {
Serial.println("Gagal membaca data sensor!");
} else {
Serial.print("Suhu: ");
Serial.print(suhu);
Serial.print(" °C -> ");
// Kendali aktuator berdasarkan hasil akuisisi data sensor
if (suhu > suhuThreshold) {
digitalWrite(RELAYPIN, HIGH); // aktifkan relay/LED
Serial.println("Aktuator: ON");
} else {
digitalWrite(RELAYPIN, LOW); // matikan relay/LED
Serial.println("Aktuator: OFF");
}
}
}


