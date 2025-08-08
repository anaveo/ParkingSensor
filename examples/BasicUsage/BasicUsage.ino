#include "ParkingSensor.h"

ParkingSensor ps;

void setup()
{
    Serial.begin(115200);
    ps.begin(2); // Use your sensor pin
}

void loop()
{
    ps.update();

    if (ps.available())
    {
        for (uint8_t i = 0; i < 4; i++)
        {
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
