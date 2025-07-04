#include <Wire.h>
#include "PCBCUPID_QMC6309.h"
#include "ExponentialFilter.h"

PCBCUPID_QMC6309 mag(Wire);

// Calibrated offsets
const float x_offset = 3485.0;
const float y_offset = 1290.0;
const float declination = 0.22;

// Filter object: 0.2 weight (20%)
ExponentialFilter headingFilter(0.2, 0.0);

void setup() {
  Serial.begin(115200);
  delay(1000);

  if (!mag.begin()) {
    Serial.println("Magnetometer init failed");
    while (1);
  }

  Serial.println("Heading with Exponential Filter");
}

void loop() {
  int16_t x, y, z;
  if (mag.readRaw(x, y, z)) {
    float x_cal = x - x_offset;
    float y_cal = y - y_offset;

    float heading = atan2(-x_cal, y_cal) + declination;
    if (heading < 0) heading += 2 * PI;
    if (heading > 2 * PI) heading -= 2 * PI;

    float headingDeg = heading * 180.0 / PI;

    // Apply exponential filter
    headingFilter.update(headingDeg);
    float smoothedHeading = headingFilter.get();

    Serial.print("Heading: ");
    Serial.print(smoothedHeading, 1);
    Serial.print("° (");
    Serial.print(mag.headingToDirection(smoothedHeading));
    Serial.println(")");
  }

  delay(200);
}



