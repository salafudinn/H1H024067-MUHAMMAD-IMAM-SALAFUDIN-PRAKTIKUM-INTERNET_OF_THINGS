#include <DHT.h>


#define DHTPIN 4

#define DHTTYPE DHT22



DHT dht(DHTPIN, DHTTYPE);



void setup() {

  Serial.begin(115200);

  dht.begin();

  Serial.println("Memulai akuisisi data sensor DHT22 (Mode Rata-rata)...");

}



void loop() {

  float totalSuhu = 0;

  float totalKelembaban = 0;

  int sampelValid = 0;



  // Mengambil 5 kali sampel data

  for (int i = 0; i < 5; i++) {

    float h = dht.readHumidity();

    float t = dht.readTemperature();



    if (!isnan(h) && !isnan(t)) {

      totalSuhu += t;

      totalKelembaban += h;

      sampelValid++;

    } else {

      Serial.println("Peringatan: Ada pembacaan sampel yang gagal!");

    }



    // Jeda 2 detik antar sampel pembacaan

    delay(2000); 

  }



  // Tampilkan rata-rata jika minimal ada 1 sampel valid

  if (sampelValid > 0) {

    float rataSuhu = totalSuhu / sampelValid;

    float rataKelembaban = totalKelembaban / sampelValid;



    Serial.print("Hasil Rata-Rata ");

    Serial.print(sampelValid);

    Serial.println(" sampel ");

    Serial.print("Suhu Rata-rata       : ");

    Serial.print(rataSuhu);

    Serial.println(" °C");

    Serial.print("Kelembaban Rata-rata : ");

    Serial.print(rataKelembaban);

    Serial.println(" %");

  } else {

    Serial.println("Gagal total: Tidak ada data sampel yang valid!");

  }

}