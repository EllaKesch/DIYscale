#include <Wire.h>
#include <Adafruit_BME280.h>
#include "HX711.h"

// -------------------- PINS --------------------
// HX711 pins
#define DT_5KG   26
#define SCK_5KG  27
#define DT_1KG   25
#define SCK_1KG  33

// BME280 I2C pins
#define I2C_SDA  13
#define I2C_SCL  14

// -------------------- OBJECTS --------------------
HX711 scale5kg;
HX711 scale1kg;
Adafruit_BME280 bme;

// -------------------- SETTINGS --------------------
const int PRINT_MS  = 500;
const int N_SAMPLES = 10;

// -------------------- CALIBRATION --------------------
// Counts per gram (replace these after calibration)
float CAL_5KG = 2150.0;   // example placeholder
float CAL_1KG = 8300.0;   // example placeholder

// Raw tare offsets (captured at tare time)
long tareRaw5 = 0;
long tareRaw1 = 0;

// -------------------- HELPERS --------------------
bool waitReady(HX711 &s, uint32_t timeout_ms) {
  uint32_t start = millis();
  while (!s.is_ready()) {
    if (millis() - start > timeout_ms) return false;
    delay(5);
  }
  return true;
}

long readRawSafe(HX711 &s, bool isReady) {
  if (!isReady) return 0;
  return s.read_average(N_SAMPLES);
}

void tareBoth() {
  Serial.println("Taring... keep platform empty");
  delay(800);

  bool r5 = waitReady(scale5kg, 1500);
  bool r1 = waitReady(scale1kg, 1500);

  if (r5) {
    tareRaw5 = scale5kg.read_average(N_SAMPLES);
    Serial.print("Tare raw 5kg: "); Serial.println(tareRaw5);
  } else {
    Serial.println("Skip tare 5kg (not ready)");
  }

  if (r1) {
    tareRaw1 = scale1kg.read_average(N_SAMPLES);
    Serial.print("Tare raw 1kg: "); Serial.println(tareRaw1);
  } else {
    Serial.println("Skip tare 1kg (not ready)");
  }

  Serial.println("Tare done.");
}

void setup() {
  Serial.begin(115200);
  delay(200);

  // BME280
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.println("BME280 not found at 0x76. Trying 0x77...");
  if (bme.begin(0x77)) Serial.println("BME280 ready.");
  else Serial.println("BME280 not found. Check wiring and address.");

  // HX711 init
  scale5kg.begin(DT_5KG, SCK_5KG);
  scale1kg.begin(DT_1KG, SCK_1KG);

  Serial.println("Checking HX711 readiness...");
  Serial.print("HX711 5kg ready: "); Serial.println(waitReady(scale5kg, 1500) ? "YES" : "NO");
  Serial.print("HX711 1kg ready: "); Serial.println(waitReady(scale1kg, 1500) ? "YES" : "NO");

  // Capture tare offsets
  tareBoth();

  Serial.println("Loop starting. Output format:");
  Serial.println("5kg_g, 1kg_g, total_g, raw5, raw1, T_C, H_pct, P_hPa");
  Serial.println("Tip: if baseline drifts, reset board to tare again.");
}

void loop() {
  bool r5 = scale5kg.is_ready();
  bool r1 = scale1kg.is_ready();

  long raw5 = readRawSafe(scale5kg, r5);
  long raw1 = readRawSafe(scale1kg, r1);

  // Convert to grams using stored tare offsets and calibration factors
  float grams5 = 0.0f;
  float grams1 = 0.0f;

  if (r5 && CAL_5KG != 0.0f) grams5 = (float)(raw5 - tareRaw5) / CAL_5KG;
  if (r1 && CAL_1KG != 0.0f) grams1 = (float)(raw1 - tareRaw1) / CAL_1KG;

  float total = grams5 + grams1;

  // BME280 readings
  float t = bme.readTemperature();
  float h = bme.readHumidity();
  float p = bme.readPressure() / 100.0f;

  // CSV-style output (easy to log)
  Serial.print(grams5, 1); Serial.print(", ");
  Serial.print(grams1, 1); Serial.print(", ");
  Serial.print(total, 1);  Serial.print(", ");
  Serial.print(raw5);      Serial.print(", ");
  Serial.print(raw1);      Serial.print(", ");
  Serial.print(t, 2);      Serial.print(", ");
  Serial.print(h, 2);      Serial.print(", ");
  Serial.println(p, 2);

  delay(PRINT_MS);
}
