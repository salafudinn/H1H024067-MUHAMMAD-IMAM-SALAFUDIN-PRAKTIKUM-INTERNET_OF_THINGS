#include <DHT.h>



#define DHTPIN 4

#define DHTTYPE DHT22

#define RELAYPIN 26



DHT dht(DHTPIN, DHTTYPE);



// Definisikan dua ambang batas (Histerisis)

const float thresholdAtas = 30.0; // Aktuator menyala di atas 30 °C

const float thresholdBawah = 28.0; // Aktuator mati di bawah 28 °C



bool statusRelay = false; // Menyimpan state status relay saat ini



void setup() {

  Serial.begin(115200);

  dht.begin();

  pinMode(RELAYPIN, OUTPUT);

  digitalWrite(RELAYPIN, LOW); // Kondisi awal OFF

}



void loop() {

  float suhu = dht.readTemperature();



  if (isnan(suhu)) {

    Serial.println("Gagal membaca data sensor!");

  } else {

    Serial.print("Suhu: ");

    Serial.print(suhu);

    Serial.print(" °C -> ");



    // Logika Histerisis

    if (suhu > thresholdAtas) {

      statusRelay = true;

    } else if (suhu < thresholdBawah) {

      statusRelay = false;

    }

    // Jika suhu berada di antara 28.0 dan 30.0 °C, statusRelay TIDAK berubah (mempertahankan state terakhir)



    // Penerapan hasil kendali ke pin hardware

    if (statusRelay) {

      digitalWrite(RELAYPIN, HIGH);

      Serial.println("Aktuator: ON");

    } else {

      digitalWrite(RELAYPIN, LOW);

      Serial.println("Aktuator: OFF");

    }

  }



  delay(2000);

}