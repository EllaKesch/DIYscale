# ESP32 Dual Load-Cell Scale + BME280 Logger

This project implements a dual load-cell weighing system using an ESP32, two HX711 ADC amplifier boards (1 kg and 5 kg), and a BME280 environmental sensor. The system continuously reads weight-related signals and environmental data and outputs them via Serial.

The setup is designed for a DIY scale platform where **two load cells share the load through a single wooden plate**, allowing higher capacity while maintaining sensitivity for smaller weights.

---

## Features

- Dual load-cell support (1 kg + 5 kg)
- Independent HX711 amplifiers
- Non-blocking HX711 reads (prevents firmware freezes)
- Works **without calibration** (raw ADC values)
- Environmental sensing:
  - Temperature (°C)
  - Humidity (%)
  - Pressure (hPa)
- Serial output every 500 ms
- Designed for later calibration without firmware changes

---

## System Overview

**Microcontroller**
- ESP32 (ESP32-D0WDQ6 tested)

**Sensors**
- 1 × 5 kg beam load cell
- 1 × 1 kg beam load cell
- 2 × HX711 ADC boards
- 1 × BME280 (I2C)

**Output**
- USB Serial Monitor

---

## Hardware Components

- ESP32 development board  
- 2 × HX711 amplifier boards  
- 1 × 5 kg beam load cell  
- 1 × 1 kg beam load cell  
- 1 × BME280 breakout board (I2C, address `0x77`)  
- Breadboard + jumper wires  
- Mechanical mounting:
  - Rigid base
  - Wooden top plate

---

## Wiring

### HX711 → ESP32

| HX711 | Signal | ESP32 GPIO |
|------|--------|------------|
| 5 kg HX711 | DT (DOUT) | GPIO 26 |
| 5 kg HX711 | SCK (CLK) | GPIO 27 |
| 1 kg HX711 | DT (DOUT) | GPIO 25 |
| 1 kg HX711 | SCK (CLK) | GPIO 33 |
| Both HX711 | VCC | 3V3 |
| Both HX711 | GND | GND |

---

### BME280 → ESP32 (I2C)

| BME280 | Signal | ESP32 GPIO |
|-------|--------|------------|
| SDA | Data | GPIO 13 |
| SCL | Clock | GPIO 14 |
| VCC | Power | 3V3 |
| GND | Ground | GND |

- BME280 I2C address: **0x77**
- If your board uses `0x76`, update the code.

---

### Load Cell → HX711 Wiring

Typical 4-wire beam load cells:

| Load Cell Wire | HX711 Pin |
|---------------|-----------|
| Red | E+ |
| Black | E- |
| Green | A+ |
| White | A- |

If your wire colors differ, refer to the manufacturer documentation.

---

## Mechanical Setup

Correct mechanical mounting is **critical** for stable readings.

### Load Cell Mounting

- Each beam load cell is mounted as a **cantilever**
- One end fixed to the base
- Other end supports the platform
- Strain gauge region (center) must be free to bend

### Platform Design

- Wooden plate must rest on **both load cells**
- Load cells should be level
- Avoid twisting or side loads
- Use spacers or standoffs for consistent height

> If the platform wobbles, readings will drift or become noisy.

---

## Firmware

### Required Libraries (Arduino IDE)

Install via Library Manager:
- **HX711** by bogde
- **Adafruit BME280 Library**
- **Adafruit Unified Sensor**

---

### Firmware Behavior

- Checks HX711 readiness before reading
- Avoids blocking if a HX711 is disconnected
- Tares only sensors that respond
- Continuously prints raw HX711 and BME280 data

---

### Serial Output

Printed every 500 ms:

- HX711 readiness + raw ADC value (both channels)
- Temperature (°C)
- Humidity (%)
- Pressure (hPa)

  Example: 
  HX5: OK raw=8392213 | HX1: OK raw=8123456 || T=22.18C H=41.90% P=1012.45hPa


---

## Calibration

### Important Notes

- The system is currently **uncalibrated**
- Raw values are valid but **not real grams or kilograms**
- Calibration must be done **after final mechanical mounting**
- Each HX711 + load cell pair requires its own calibration factor

---

## Calibration Method A (Recommended – Per Load Cell)

This provides the best accuracy.

### Steps

1. Mount the full system permanently  
2. Power on and wait **2–5 minutes**  
3. Tare the empty platform  
4. Place a known weight (e.g. 1000 g)  
5. Record:
   - `raw_with_weight`
   - `raw_tare`
6. Compute:


delta = raw_with_weight - raw_tare
factor = delta / known_grams


Repeat for both load cells.

### Weight Calculation



grams = (raw - tare) / factor
total_weight = grams_5kg + grams_1kg


---

## Calibration Method B (Fast – Combined Platform)

Use this if individual load-cell accuracy is not required.

1. Tare platform  
2. Place known weight  
3. Compute:


delta_total = (raw5 + raw1) - tare_total
factor_total = delta_total / known_grams
total_weight = delta_total / factor_total


> This method hides uneven load distribution.

---

## Calibration Tips

- Always place weights in the same position
- Use at least two reference weights:
- 200–500 g
- 1000–2000 g
- Large variation when moving weight indicates mechanical issues

---

## Troubleshooting

### BME280 Not Detected
- Check I2C address (`0x77` vs `0x76`)
- Verify SDA/SCL wiring
- Ensure sensor is in I2C mode
- Tie CS high if the board supports SPI

### HX711 Not Ready
- Confirm 3.3V power and common ground
- Check DT/SCK wiring
- Verify correct HX711 pin header
- Test HX711 boards one at a time

### No Weight Change
- Load cell not bending
- Platform touching base
- Incorrect mounting orientation

---

## Future Improvements

- Serial commands for tare and calibration
- Save calibration factors to non-volatile memory
- Output in grams/kg
- Filtering (moving average, median)
- CSV logging with timestamps

---
alibration-notes.md
README.md
