#include <Wire.h>
#include "PCBCUPID_QMC6309.h"

PCBCUPID_QMC6309 mag;

void setup() {
  Serial.begin(115200);
  delay(100);

  if (!mag.begin()) {
    Serial.println("PCBCUPID_QMC6309 not found!");
    while (1);
  }

  Serial.println("PCBCUPID_QMC6309 initialized.");
  mag.reset();
  mag.setMode(QMC6309_MODE_CONTINUOUS, QMC6309_ODR_50HZ);
}

void loop() {
  int16_t x, y, z;

  if (mag.readRaw(x, y, z)) {
    Serial.print("X: "); Serial.print(x);
    Serial.print("  Y: "); Serial.print(y);
    Serial.print("  Z: "); Serial.println(z);
  } else {
    Serial.println("Read error");
  }

  delay(100);
}
