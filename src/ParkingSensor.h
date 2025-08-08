// ParkingSensor.h
#ifndef PARKING_SENSOR_H
#define PARKING_SENSOR_H

#include <Arduino.h>

class ParkingSensor
{
public:
    ParkingSensor();

    // Initialize sensor with input pin
    void begin(uint8_t pin);

    // Call frequently in loop() to handle updates
    void update();

    // Returns true if new data is available since last update()
    bool available() const;

    // Get raw 8-bit sensor value (0-255) for sensor 0..3
    uint8_t getRaw(uint8_t sensorIndex) const;

    // Convert raw sensor value to meters (user can modify this)
    float getDistance(uint8_t sensorIndex) const;

    // Optional: adjust LOW pulse threshold in microseconds (default 130)
    void setThreshold(unsigned int thresholdMicros);

private:
    static void ISR();

    enum State
    {
        WAIT_START_HIGH,
        WAIT_FIRST_LOW,
        READ_BITS
    };

    // Interrupt-driven state machine variables
    static volatile State state;
    static volatile uint32_t rawData;
    static volatile uint8_t bitIndex;
    static volatile unsigned long lastEdgeTime;
    static volatile bool currentLevel;
    static volatile unsigned long lowPulseLength;
    static volatile bool dataReady;

    static uint8_t inputPin;
    static unsigned int threshold;

    static uint8_t sensorValues[4];

    // For thread-safe copying
    uint8_t lastSensorValues[4];
    bool hasNewData;

    // Detach ISR and reattach for new pin
    void attachISR();
    void detachISR();
};

#endif
