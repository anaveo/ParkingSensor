# ParkingSensor

Interrupt-driven Arduino library to decode 4 sensor readings from a proprietary parking sensor signal.

## Important Notes

- **Distance measurement scale:**  
  The sensors use a non-linear scale where higher raw values mean *shorter* distances:  
  - Raw value `240` ≈ 1.5 meters  
  - Raw value `241` ≈ 1.4 meters  
  - ...  
  - Raw value `255` ≈ 0 meters (closest)  

## Features

- Non-blocking, interrupt-based decoding of pulse train signals  
- Supports 4 sensors with 8-bit resolution each  
- Provides raw sensor values and distance in meters (with caveats above)  

## Installation

1. Download the library from [GitHub](https://github.com/anaveo/ParkingSensor)  
2. Add the library to the Arduino IDE: **Sketch > Include Library > Add .ZIP Library...**  
3. Or clone the repo into your Arduino `libraries/` folder  

## Usage

```cpp
#include <ParkingSensor.h>

ParkingSensor ps;

void setup() {
  Serial.begin(115200);
  ps.begin(2);  // Use your sensor input pin
}

void loop() {
  ps.update();

  if (ps.available()) {
    for (uint8_t i = 0; i < 4; i++) {
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.print(": raw=");
      Serial.print(ps.getRaw(i));
      Serial.print(", dist(m)=");
      Serial.println(ps.getDistance(i), 3);
    }
    Serial.println();
  }
}
```

## API
- void begin(uint8_t pin) — Initialize the sensor input pin
- void update() — Call frequently to process incoming signals
- bool available() — Check if new sensor data is available
- uint8_t getRaw(uint8_t sensorIndex) — Get raw 8-bit value for sensor 0–3
- float getDistance(uint8_t sensorIndex) — Get distance in meters 
- void setThreshold(unsigned int microseconds) — Set pulse length threshold for bit decoding
