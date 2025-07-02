#include <Wire.h>
#include "PCBCUPID_QMC6309.h"

PCBCUPID_QMC6309 mag(Wire);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("QMC6309 Test");

  if (!mag.begin()) {
    Serial.println("QMC6309 initialization failed!");
    while (1);
  }

  Serial.println("QMC6309 initialized successfully.");
}

void loop() {
  int16_t x, y, z;
  if (mag.readRaw(x, y, z)) {
    // Already printed inside readRaw
  } else {
    Serial.println("Failed to read raw data");
  }
  delay(500);
}
