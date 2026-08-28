#define BLYNK_TEMPLATE_ID "TMPL3u-kNvlep"
#define BLYNK_TEMPLATE_NAME "temperature controlled fan"
#define BLYNK_AUTH_TOKEN "D09kqhXxrDJNwhHmrFa6oPZR-T0orkZF"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

char ssid[] = "cromastone";
char pass[] = "k12345678";

#define DHTPIN 4
#define DHTTYPE DHT11
#define RELAY_PIN 5

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

float threshold = 33.0;
int manualOverride = 0; // Tracks if manual switch is toggled

void readSensor()
{
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("Failed to read DHT11!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C | Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Send sensor data to CORRECT Blynk Datastreams matching your web setup
  Blynk.virtualWrite(V1, temperature); // V1 is Temperature
  Blynk.virtualWrite(V2, humidity);    // V2 is Humidity

  // If manual override is NOT active, use automated temperature logic
  if (manualOverride == 0)
  {
    if (temperature >= threshold)
    {
      digitalWrite(RELAY_PIN, LOW);   // Turn ON Fan (Active-Low)
      Serial.println("Fan ON (Auto)");
    }
    else
    {
      digitalWrite(RELAY_PIN, HIGH);  // Turn OFF Fan
      Serial.println("Fan OFF (Auto)");
    }
  }

  Serial.println("----------------------");
}

// Handles Manual Switch Override from Blynk app (Virtual Pin V0)
BLYNK_WRITE(V0)
{
  manualOverride = param.asInt();

  if (manualOverride == 1)
  {
    digitalWrite(RELAY_PIN, LOW); // Force Fan ON
    Serial.println("Manual Override: Fan ON");
  }
  else
  {
    digitalWrite(RELAY_PIN, HIGH); // Force Fan OFF / Return to Auto
    Serial.println("Manual Override: Fan OFF");
  }
}

// Handles incoming slider value from Blynk app (Virtual Pin V3)
BLYNK_WRITE(V3)
{
  threshold = param.asFloat();
  Serial.print("Temperature threshold updated: ");
  Serial.print(threshold);
  Serial.println(" °C");
}

void setup()
{
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Fan OFF initially

  dht.begin();

  // Connect to Wi-Fi and Blynk Cloud
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Sync state variables from server on startup
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V3);

  // Setup sensor reading interval (every 2 seconds)
  timer.setInterval(2000L, readSensor);

  Serial.println("Temperature Controlled Fan Started");
}

void loop()
{
  Blynk.run();
  timer.run();
}