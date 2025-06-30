#include "PCBCUPID_QMC6309.h"

PCBCUPID_QMC6309::QMC6309(TwoWire &w)
{
    _wire = &w;
}

PCBCUPID_QMC6309::begin()
{
    _wire->begin();
    delay(10);

    uint16_t chipID = readChipID();
    return (chipID == 0x90); // as per the datasheet the chipId address is 0x00 and value is 0x90
}

PCBCUPID_QMC6309::reset()
{
    writeRegister(QMC6309_CONTROL_REG_2, 0x80); // soft reset
    delay(10);
    writeRegister(QMC6309_CONTROL_REG_2, 0x00); // clear reset
    delay(10);
}

PCBCUPID_QMC6309::setMode(uint8_t mode, uint8_t odr)
{
    uint8_t control1 = 0x00;

    control1 |= (0x02 << 5);
    control1 |= (0x00 << 3);
    control1 = (mode & 0x03);

    writeRegister(QMC6309_CONTROL_REG_1, control1);

    uint8_t control2 = ((odr & 0x07) << 5) | (0x00 << 3) | (0x00);
    writeRegister(QMC6309_CONTROL_REG_2, control2);
}

PCBCUPID_QMC6309::readChipID()
{
  
    return readRegister(QMC6309_REG_CHIP_ID);
   
}

bool PCBCUPID_QMC6309::readRaw(int16_t &x, int16_t &y, int16_t &z)
{
    _wire->beginTransmission(QMC6309_I2C_ADDR);
    _wire->write(QMC6309_REG_X_LSB);
    if (_wire->endTransmission(false) != 0)
        return false;

    _wire->requestFrom(QMC6309_I2C_ADDR, (uint8_t)6);
    if (_wire->available() < 6)
        return false;

    uint8_t xl = _wire->read();
    uint8_t xh = _wire->read();
    uint8_t yl = _wire->read();
    uint8_t yh = _wire->read();
    uint8_t zl = _wire->read();
    uint8_t zh = _wire->read();

    x = (int16_t)((xh << 8) | xl);
    y = (int16_t)((yh << 8) | yl);
    z = (int16_t)((zh << 8) | zl);

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
    _wire->endTransmission(false);
    _wire->requestFrom(QMC6309_I2C_ADDR, (uint8_t)1);
    return _wire->available() ? _wire->read() : 0;
}