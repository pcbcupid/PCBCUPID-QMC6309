#include "PCBCUPID_QMC6309.h"

PCBCUPID_QMC6309::PCBCUPID_QMC6309(TwoWire &w)
{
  _wire = &w;
}

uint8_t PCBCUPID_QMC6309::readChipID()
{
  _wire->beginTransmission(QMC6309_I2C_ADDR);
  _wire->write(QMC6309_REG_CHIP_ID);
  if (_wire->endTransmission(false) != 0) {
    Serial.println("I2C write failed");
    return 0xFF;
  }

  if (_wire->requestFrom(QMC6309_I2C_ADDR, (uint8_t)1) != 1) {
    Serial.println("I2C read failed");
    return 0xFF;
  }

  uint8_t chipID = _wire->read();
  Serial.print("Read Chip ID: 0x");
  Serial.println(chipID, HEX);
  return chipID;
}

void PCBCUPID_QMC6309::reset()
{
  writeRegister(QMC6309_CONTROL_REG_2, 0x80); //soft reset
  delay(20);
  writeRegister(QMC6309_CONTROL_REG_2, 0x00); //clear reset
  delay(20);
}

void PCBCUPID_QMC6309::setMode(uint8_t mode, uint8_t odr)
{
  uint8_t control1 = (0x04 << 5) | (0x02 << 3) | (mode & 0x03);
  writeRegister(QMC6309_CONTROL_REG_1, control1);
  Serial.print("Control Reg 1 set to 0x");
  Serial.println(control1, HEX);

  uint8_t control2 = (odr << 4) | (0x02 << 2) | 0x00;
  writeRegister(QMC6309_CONTROL_REG_2, control2);
  Serial.print("Control Reg 2 set to 0x");
  Serial.println(control2, HEX);
}

/*Helper Function*/
void PCBCUPID_QMC6309::writeRegister(uint8_t reg, uint8_t val)
{
  _wire->beginTransmission(QMC6309_I2C_ADDR);
  _wire->write(reg);
  _wire->write(val);
  _wire->endTransmission();

}
