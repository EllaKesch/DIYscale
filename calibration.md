## Calibration

### Overview

The HX711 amplifiers output raw ADC counts. Calibration converts these raw values into real weight units (grams).

Each load cell + HX711 pair has its own sensitivity and therefore **requires its own calibration factor**.

> Calibration must be performed **after the final mechanical mounting** of the load cells and platform.

---

### What You Need

- Fully assembled scale (final mounting and platform)
- ESP32 running the current firmware
- Serial Monitor at **115200 baud**
- At least **one known reference weight**
  - Recommended: **1000 g or higher**
  - Water bottles can be used (1 L ≈ 1000 g)

---

### Calibration Principle

Weight is calculated using:

weight_grams = (raw_value - tare_offset) / calibration_factor

yaml
Copy code

Where:
- `raw_value` is the HX711 ADC reading
- `tare_offset` is the zero-load reading
- `calibration_factor` is counts per gram

---

### Calibration Procedure (Recommended)

#### 1. Stabilize the System
- Power the ESP32
- Wait **2–5 minutes**
- Do not touch the platform during this time

This allows the load cells and HX711 to thermally stabilize.

---

#### 2. Tare (Zero the Scale)
- Ensure the platform is **empty**
- Reset the ESP32
- Confirm in Serial Monitor:
Taring (only ready scales)...
Tared 5kg
Tared 1kg

The system now stores the zero offset internally.

---

#### 3. Record Raw Values at Zero Load
With the platform still empty, note the raw readings:

HX5: OK raw=XXXXXXXX
HX1: OK raw=YYYYYYYY

Save these as:
- `raw5_tare`
- `raw1_tare`

---

#### 4. Apply a Known Weight
- Place the known weight **centered** on the platform
- Wait **5–10 seconds**
- Record the new raw readings:

HX5: OK raw=AAAAAAAA
HX1: OK raw=BBBBBBBB


Save these as:
- `raw5_weight`
- `raw1_weight`

---

#### 5. Compute Calibration Factors

Let:
known_weight = weight_in_grams

Compute:

delta5 = raw5_weight - raw5_tare
delta1 = raw1_weight - raw1_tare

cal_factor_5kg = delta5 / known_weight
cal_factor_1kg = delta1 / known_weight



Example:
known_weight = 1000 g
delta5 = 2,150,000
delta1 = 8,300,000

cal_factor_5kg = 2150.0
cal_factor_1kg = 8300.0


---

### Applying Calibration in Code

Add the calibration factors near the top of the firmware:

```cpp
// Calibration factors (counts per gram)
float CAL_5KG = 2150.0;   // replace with measured value
float CAL_1KG = 8300.0;   // replace with measured value

#Convert raw readings to grams in the main loop:

grams5 = raw5 / CAL_5KG;
grams1 = raw1 / CAL_1KG;
total_grams = grams5 + grams1;
```
# Verification
After calibration:

- Empty platform should read approximately 0 g
- Known weight should match expected value within tolerance
- Removing the weight should return to 0 g

Minor variation (±1–3%) is normal for DIY mechanical setups.

# When Re-Calibration Is Required
Re-calibration is required if:
- Load cells are remounted
- Platform geometry changes
- HX711 boards are replaced
- Large temperature changes occur
- Normal power cycles do not require recalibration.

# Alternative: Combined Calibration (Optional)
- If individual load-cell accuracy is not required, the system can be calibrated using the sum of both raw signals.
- This method is faster but hides uneven load distribution and is not recommended for precision measurements.
