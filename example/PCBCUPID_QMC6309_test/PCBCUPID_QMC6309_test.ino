#include "PCBCUPID_QMC6309.h"
#include <Wire.h>

PCBCUPID_QMC6309 mag(Wire);

void setup() {
  Serial.begin(115200);
  delay(100);

  Wire.begin(4, 5);

  uint8_t chipId = mag.readChipID();
  Serial.print("Chip ID: 0x");
  Serial.println(chipId, HEX);
  if (chipId == 0x90) {
    Serial.println("Chip ID found");
    mag.reset();
    Serial.println("Soft reset complete");
    mag.setMode(QMC6309_MODE_CONTINUOUS, QMC6309_ODR_50HZ);
  } else {
    Serial.println("Chip ID not found");
  }
}

void loop() {
}