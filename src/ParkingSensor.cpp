// ParkingSensor.cpp
#include "ParkingSensor.h"

uint8_t ParkingSensor::inputPin = 2;         // default pin
unsigned int ParkingSensor::threshold = 130; // default threshold (µs)

volatile ParkingSensor::State ParkingSensor::state = WAIT_START_HIGH;
volatile uint32_t ParkingSensor::rawData = 0;
volatile uint8_t ParkingSensor::bitIndex = 0;
volatile unsigned long ParkingSensor::lastEdgeTime = 0;
volatile bool ParkingSensor::currentLevel = LOW;
volatile unsigned long ParkingSensor::lowPulseLength = 0;
volatile bool ParkingSensor::dataReady = false;
volatile uint8_t ParkingSensor::sensorValues[4] = {0, 0, 0, 0};

ParkingSensor::ParkingSensor() : hasNewData(false)
{
    memset(lastSensorValues, 0, sizeof(lastSensorValues));
}

void ParkingSensor::begin(uint8_t pin)
{
    inputPin = pin;
    pinMode(inputPin, INPUT);
    currentLevel = digitalRead(inputPin);
    lastEdgeTime = micros();
    attachISR();
}

void ParkingSensor::attachISR()
{
    attachInterrupt(digitalPinToInterrupt(inputPin), ISR, CHANGE);
}

void ParkingSensor::detachISR()
{
    detachInterrupt(digitalPinToInterrupt(inputPin));
}

void ParkingSensor::update()
{
    if (dataReady)
    {
        noInterrupts();
        for (int i = 0; i < 4; i++)
        {
            lastSensorValues[i] = sensorValues[i];
        }
        dataReady = false;
        interrupts();
        hasNewData = true;
    }
}

bool ParkingSensor::available() const
{
    return hasNewData;
}

uint8_t ParkingSensor::getRaw(uint8_t sensorIndex) const
{
    if (sensorIndex > 3)
        return 0;
    return lastSensorValues[sensorIndex];
}

float ParkingSensor::getDistance(uint8_t sensorIndex) const
{
    // Example conversion: raw value 0 = 0m, 255 = 3m (adjust for your sensor)
    if (sensorIndex > 3)
        return 0.0f;
    float meters = (lastSensorValues[sensorIndex] / 255.0f) * 3.0f;
    return meters;
}

void ParkingSensor::setThreshold(unsigned int thresholdMicros)
{
    threshold = thresholdMicros;
}

// Static ISR method
void ParkingSensor::ISR()
{
    bool level = digitalRead(inputPin);
    unsigned long now = micros();
    unsigned long pulseLength = (now >= lastEdgeTime) ? (now - lastEdgeTime) : (ULONG_MAX - lastEdgeTime + now);
    lastEdgeTime = now;
    currentLevel = level;

    if (pulseLength < 50)
        return; // debounce noise filter

    switch (state)
    {
    case WAIT_START_HIGH:
        if (!level && pulseLength > 1500 && pulseLength < 2500)
        {
            state = WAIT_FIRST_LOW;
        }
        break;

    case WAIT_FIRST_LOW:
        if (!level)
        {
            bitIndex = 0;
            rawData = 0;
            state = READ_BITS;
            lowPulseLength = 0;
        }
        break;

    case READ_BITS:
        if (level)
        {
            lowPulseLength = pulseLength; // rising edge ends LOW pulse
        }
        else
        {
            if (lowPulseLength > 0)
            {
                rawData <<= 1;
                if (lowPulseLength > threshold)
                {
                    rawData |= 1;
                }
                bitIndex++;
                lowPulseLength = 0;

                if (bitIndex >= 32)
                {
                    sensorValues[0] = (rawData >> 24) & 0xFF; // A
                    sensorValues[1] = (rawData >> 0) & 0xFF;  // D
                    sensorValues[2] = (rawData >> 16) & 0xFF; // B
                    sensorValues[3] = (rawData >> 8) & 0xFF;  // C
                    dataReady = true;
                    state = WAIT_START_HIGH;
                }
            }
        }
        break;
    }
}
