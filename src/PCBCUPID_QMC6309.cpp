#include "PCBCUPID_QMC6309.h"

PCBCUPID_QMC6309::PCBCUPID_QMC6309(TwoWire &w) {
  _wire = &w;
  xOffset = 0;
  yOffset = 0;
}

bool PCBCUPID_QMC6309::begin() {
  _wire->begin();
  delay(10);

  uint8_t chipID = readChipID();
  if (chipID != 0x90) {
    return false;
  }

  reset();
  delay(50);

  setMode(QMC6309_MODE_SUSPEND, QMC6309_ODR_200HZ);
  delay(10);
  setMode(QMC6309_MODE_NORMAL, QMC6309_ODR_200HZ);
  delay(10);
  setMode(QMC6309_MODE_SUSPEND, QMC6309_ODR_200HZ);
  delay(10);
  setMode(QMC6309_MODE_CONTINUOUS, QMC6309_ODR_200HZ);
  delay(500);

  return true;
}

uint8_t PCBCUPID_QMC6309::readChipID() {
  _wire->beginTransmission(QMC6309_I2C_ADDR);
  _wire->write(QMC6309_REG_CHIP_ID);
  if (_wire->endTransmission(false) != 0) return 0xFF;

  if (_wire->requestFrom(QMC6309_I2C_ADDR, (uint8_t)1) != 1) return 0xFF;

  return _wire->read();
}

void PCBCUPID_QMC6309::reset() {
  writeRegister(QMC6309_CONTROL_REG_2, 0x80);
  delay(20);
  writeRegister(QMC6309_CONTROL_REG_2, 0x00);
  delay(20);
}

void PCBCUPID_QMC6309::setMode(uint8_t mode, uint8_t odr) {
  uint8_t osr = 0b11;
  uint8_t rng = 0b01;
  uint8_t ctrl1 = (osr << 6) | (rng << 4) | ((odr & 0x03) << 2) | (mode & 0x03);
  writeRegister(QMC6309_CONTROL_REG_1, ctrl1);
  writeRegister(QMC6309_CONTROL_REG_2, 0x03);
  delay(500);
}

bool PCBCUPID_QMC6309::readRaw(int16_t &x, int16_t &y, int16_t &z) {
  uint32_t start = millis();
  while (!(readRegister(QMC6309_STATUS_REG) & QMC6309_STATUS_DRDY)) {
    if (millis() - start > 2000) return false;
    delay(20);
  }

  if (readRegister(QMC6309_STATUS_REG) & QMC6309_STATUS_OVL) return false;

  _wire->beginTransmission(QMC6309_I2C_ADDR);
  _wire->write(QMC6309_REG_X_LSB);
  if (_wire->endTransmission(false) != 0) return false;

  if (_wire->requestFrom(QMC6309_I2C_ADDR, (uint8_t)6) != 6) return false;

  uint8_t xl = _wire->read();
  uint8_t xh = _wire->read();
  uint8_t yl = _wire->read();
  uint8_t yh = _wire->read();
  uint8_t zl = _wire->read();
  uint8_t zh = _wire->read();

  x = (int16_t)((xh << 8) | xl);
  y = (int16_t)((yh << 8) | yl);
  z = (int16_t)((zh << 8) | zl);

  if (x < xMin) xMin = x;
  if (x > xMax) xMax = x;
  if (y < yMin) yMin = y;
  if (y > yMax) yMax = y;
  if (z < zMin) zMin = z;
  if (z > zMax) zMax = z;

  return true;
}

void PCBCUPID_QMC6309::setOffsets(int16_t x_off, int16_t y_off) {
  xOffset = x_off;
  yOffset = y_off;
}

void PCBCUPID_QMC6309::getCalibrated(int16_t xRaw, int16_t yRaw, int16_t zRaw, float &xCorr, float &yCorr) {
  float xCenter = (xMax + xMin) / 2.0;
  float yCenter = (yMax + yMin) / 2.0;
  float xScale = (xMax - xMin) / 2.0;
  float yScale = (yMax - yMin) / 2.0;
  float avgScale = (xScale + yScale) / 2.0;

  xCorr = (xRaw - xCenter) * (avgScale / xScale);
  yCorr = (yRaw - yCenter) * (avgScale / yScale);
}

float PCBCUPID_QMC6309::getHeading() {
  int16_t xRaw, yRaw, zRaw;
  if (!readRaw(xRaw, yRaw, zRaw)) return -1.0;

  float xCorr, yCorr;
  getCalibrated(xRaw, yRaw, zRaw, xCorr, yCorr);

  float heading = atan2(-xCorr, yCorr);
  heading += 0.22;
  if (heading < 0) heading += 2 * PI;
  if (heading > 2 * PI) heading -= 2 * PI;
  return heading * 180.0 / PI;
}

const char *PCBCUPID_QMC6309::headingToDirection(float deg) {
  if (deg >= 337.5 || deg < 22.5) return "N";
  if (deg >= 22.5 && deg < 67.5) return "NE";
  if (deg >= 67.5 && deg < 112.5) return "E";
  if (deg >= 112.5 && deg < 157.5) return "SE";
  if (deg >= 157.5 && deg < 202.5) return "S";
  if (deg >= 202.5 && deg < 247.5) return "SW";
  if (deg >= 247.5 && deg < 292.5) return "W";
  if (deg >= 292.5 && deg < 337.5) return "NW";
  return "Unknown";
}

void PCBCUPID_QMC6309::convertToMicroTesla(int16_t xRaw, int16_t yRaw, int16_t zRaw, float &x_uT, float &y_uT, float &z_uT) {
  const float scaleFactor = 0.0488;
  x_uT = xRaw * scaleFactor;
  y_uT = yRaw * scaleFactor;
  z_uT = zRaw * scaleFactor;
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
  if (_wire->endTransmission(false) != 0) return 0xFF;
  if (_wire->requestFrom(QMC6309_I2C_ADDR, (uint8_t)1) != 1) return 0xFF;
  return _wire->read();
}



