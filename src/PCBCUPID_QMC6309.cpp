#include "PCBCUPID_QMC6309.h"

PCBCUPID_QMC6309::PCBCUPID_QMC6309(TwoWire &w) {
  _wire = &w;
}

bool PCBCUPID_QMC6309::begin() {
  _wire->begin();
  delay(10);

  uint8_t chipID = readChipID();
  Serial.print("Read Chip ID: 0x");
  Serial.println(chipID, HEX);
  if (chipID != 0x90) {
    Serial.println("Chip ID not found");
    return false;
  }

  Serial.println("Chip ID found");

  reset();
  delay(50);

  // Startup sequence as per datasheet:
  // Suspend mode, then normal, suspend, continuous
  setMode(QMC6309_MODE_SUSPEND, QMC6309_ODR_50HZ);
  delay(10);
  setMode(QMC6309_MODE_NORMAL, QMC6309_ODR_50HZ);
  delay(10);
  setMode(QMC6309_MODE_SUSPEND, QMC6309_ODR_50HZ);
  delay(10);
  setMode(QMC6309_MODE_CONTINUOUS, QMC6309_ODR_50HZ);
  delay(500);  // Let sensor start measuring

  return true;
}

uint8_t PCBCUPID_QMC6309::readChipID() {
  _wire->beginTransmission(QMC6309_I2C_ADDR);
  _wire->write(QMC6309_REG_CHIP_ID);
  uint8_t err = _wire->endTransmission(false);
  if (err != 0) {
    Serial.print("I2C write failed, error: ");
    Serial.println(err);
    return 0xFF;
  }

  uint8_t count = _wire->requestFrom(QMC6309_I2C_ADDR, (uint8_t)1);
  if (count != 1) {
    Serial.println("I2C read failed");
    return 0xFF;
  }

  return _wire->read();
}

void PCBCUPID_QMC6309::reset() {
  writeRegister(QMC6309_CONTROL_REG_2, 0x80);  // Soft reset
  Serial.println("Soft reset done");
  delay(20);
  writeRegister(QMC6309_CONTROL_REG_2, 0x00);  // Clear reset bit
  Serial.println("Clear reset done");
  delay(20);
}

void PCBCUPID_QMC6309::setMode(uint8_t mode, uint8_t odr) {
  // OSR=512 (0b11), RNG=8G (0b01)
  uint8_t osr = 0b11;
  uint8_t rng = 0b01;
  uint8_t ctrl1 = (osr << 6) | (rng << 4) | ((odr & 0x03) << 2) | (mode & 0x03);
  writeRegister(QMC6309_CONTROL_REG_1, ctrl1);
  Serial.print("Control Reg 1 set to: 0x");
  Serial.println(ctrl1, HEX);

  // CTRL2: Enable Set/Reset (bit0), enable interrupt latch (bit1) — try enabling interrupts to check
  uint8_t ctrl2 = 0x03; // bit0 = Set/Reset enable, bit1 = Interrupt latch enable
  writeRegister(QMC6309_CONTROL_REG_2, ctrl2);
  Serial.print("Control Reg 2 set to: 0x");
  Serial.println(ctrl2, HEX);

  delay(500); // Allow sensor to stabilize after mode change
}

bool PCBCUPID_QMC6309::readRaw(int16_t &x, int16_t &y, int16_t &z) {
  uint32_t start = millis();
  while (!(readRegister(QMC6309_STATUS_REG) & QMC6309_STATUS_DRDY)) {
    if (millis() - start > 2000) { // 2 seconds timeout
      Serial.println("Timeout waiting for data ready");
      return false;
    }
    delay(20);
  }

  uint8_t status = readRegister(QMC6309_STATUS_REG);
  Serial.print("Status Reg: 0x");
  Serial.println(status, HEX);

  if (status & QMC6309_STATUS_OVL) {
    Serial.println("Data overflow detected!");
    return false;
  }

  _wire->beginTransmission(QMC6309_I2C_ADDR);
  _wire->write(QMC6309_REG_X_LSB);
  if (_wire->endTransmission(false) != 0) {
    Serial.println("I2C write failed during data read");
    return false;
  }

  if (_wire->requestFrom(QMC6309_I2C_ADDR, (uint8_t)6) != 6) {
    Serial.println("I2C read failed during data read");
    return false;
  }

  uint8_t xl = _wire->read();
  uint8_t xh = _wire->read();
  uint8_t yl = _wire->read();
  uint8_t yh = _wire->read();
  uint8_t zl = _wire->read();
  uint8_t zh = _wire->read();

  x = (int16_t)((xh << 8) | xl);
  y = (int16_t)((yh << 8) | yl);
  z = (int16_t)((zh << 8) | zl);

  Serial.print("X: "); Serial.print(x);
  Serial.print(" Y: "); Serial.print(y);
  Serial.print(" Z: "); Serial.println(z);

  return true;
}


void PCBCUPID_QMC6309::writeRegister(uint8_t reg, uint8_t val) {
  _wire->beginTransmission(QMC6309_I2C_ADDR);
  _wire->write(reg);
  _wire->write(val);
  _wire->endTransmission();
}

uint8_t PCBCUPID_QMC6309::readRegister(uint8_t reg) {
  _wire->beginTransmission(QMC6309_I2C_ADDR);
  _wire->write(reg);
  if (_wire->endTransmission(false) != 0) {
    Serial.println("I2C readRegister: endTransmission failed");
    return 0xFF;
  }

  if (_wire->requestFrom(QMC6309_I2C_ADDR, (uint8_t)1) != 1) {
    Serial.println("I2C readRegister: requestFrom failed");
    return 0xFF;
  }

  return _wire->read();
}

void PCBCUPID_QMC6309::dumpRegisters() {
  for (uint8_t addr = 0x00; addr <= 0x0F; addr++) {
    uint8_t val = readRegister(addr);
    Serial.print("Reg 0x");
    Serial.print(addr, HEX);
    Serial.print(": 0x");
    Serial.println(val, HEX);
  }
}




