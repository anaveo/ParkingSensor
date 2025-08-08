#include "ParkingSensor.h"

ParkingSensor ps;
const char sensors[] = {'A', 'B', 'C', 'D'};

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
            Serial.print(sensors[i]);
            Serial.print(": raw=");
            Serial.print(ps.getRaw(i));
            Serial.print(", dist(m)=");
            Serial.print(ps.getDistance(i), 3);
            Serial.print(" |");
        }
        Serial.println();
    }
}
