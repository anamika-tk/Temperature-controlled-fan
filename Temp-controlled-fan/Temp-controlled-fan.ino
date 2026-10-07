#define BLYNK_TEMPLATE_ID "TMPL3u-kNvlep"
#define BLYNK_TEMPLATE_NAME "temperature controlled fan"
#define BLYNK_AUTH_TOKEN "D09kqhXxrDJNwhHmrFa6oPZR-T0orkZF"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

char ssid[] = "cromastone";
char pass[] = "k12345678";

#define DHTPIN 4
#define RELAY 5

DHT dht(DHTPIN, DHT11);
BlynkTimer timer;

float threshold = 33;
bool manual = false;

void updateSystem() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum))
    return;

  Blynk.virtualWrite(V1, temp);
  Blynk.virtualWrite(V2, hum);

  if (!manual) {
    if (temp >= threshold)
      digitalWrite(RELAY, LOW);
    else
      digitalWrite(RELAY, HIGH);
  }

  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print(" C | Humidity: ");
  Serial.print(hum);
  Serial.print(" % | Threshold: ");
  Serial.println(threshold);
}

BLYNK_WRITE(V0) {
  manual = param.asInt();

  if (manual)
    digitalWrite(RELAY, LOW);
  else
    digitalWrite(RELAY, HIGH);
}

BLYNK_WRITE(V3) {
  threshold = param.asFloat();
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);

  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Blynk.syncVirtual(V0, V3);
  timer.setInterval(2000L, updateSystem);
}

void loop() {
  Blynk.run();
  timer.run();
}
