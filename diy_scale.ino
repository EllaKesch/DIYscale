#include <Wire.h>
#include <Adafruit_BME280.h>
#include "HX711.h"

// HX711 pins
#define DT_5KG   26
#define SCK_5KG  27
#define DT_1KG   25
#define SCK_1KG  33

// BME280 pins
#define I2C_SDA  13
#define I2C_SCL  14

HX711 scale5kg;
HX711 scale1kg;
Adafruit_BME280 bme;

const int PRINT_MS = 500;
const int N_SAMPLES = 5;

// Try to wait for HX711 ready, but do not block forever
bool waitReady(HX711 &s, uint32_t timeout_ms) {
  uint32_t start = millis();
  while (!s.is_ready()) {
    if (millis() - start > timeout_ms) return false;
    delay(5);
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(200);

  // BME280
  Wire.begin(I2C_SDA, I2C_SCL);
  if (bme.begin(0x77)) Serial.println("BME280 ready.");
  else Serial.println("BME280 not found.");

  // HX711 init
  scale5kg.begin(DT_5KG, SCK_5KG);
  scale1kg.begin(DT_1KG, SCK_1KG);

  Serial.println("Checking HX711 readiness...");

  bool r5 = waitReady(scale5kg, 1500);
  bool r1 = waitReady(scale1kg, 1500);

  Serial.print("HX711 5kg ready: "); Serial.println(r5 ? "YES" : "NO");
  Serial.print("HX711 1kg ready: "); Serial.println(r1 ? "YES" : "NO");

  // Tare only the ones that are ready
  Serial.println("Taring (only ready scales)...");
  if (r5) { scale5kg.tare(); Serial.println("Tared 5kg"); }
  else Serial.println("Skip tare 5kg (not ready)");

  if (r1) { scale1kg.tare(); Serial.println("Tared 1kg"); }
  else Serial.println("Skip tare 1kg (not ready)");

  Serial.println("Loop starting. You should see lines every 500 ms.");
}

void loop() {
  // Check readiness each loop so it never freezes
  bool r5 = scale5kg.is_ready();
  bool r1 = scale1kg.is_ready();

  long raw5 = 0, raw1 = 0;
  if (r5) raw5 = scale5kg.read_average(N_SAMPLES);
  if (r1) raw1 = scale1kg.read_average(N_SAMPLES);

  float t = bme.readTemperature();
  float h = bme.readHumidity();
  float p = bme.readPressure() / 100.0f;

  Serial.print("HX5: "); Serial.print(r5 ? "OK" : "NO");
  Serial.print(" raw="); Serial.print(raw5);

  Serial.print(" | HX1: "); Serial.print(r1 ? "OK" : "NO");
  Serial.print(" raw="); Serial.print(raw1);

  Serial.print(" || T="); Serial.print(t, 2);
  Serial.print("C H="); Serial.print(h, 2);
  Serial.print("% P="); Serial.print(p, 2);
  Serial.println("hPa");

  delay(PRINT_MS);
}
