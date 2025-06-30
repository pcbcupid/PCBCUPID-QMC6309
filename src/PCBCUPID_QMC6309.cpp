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
    return (chipID == 0x90);  // as per the datasheet the chipId address is 0x00 and value is 0x90

}

PCBCUPID_QMC6309::reset()
{
    writeRegister(QMC6309_CONTROL_REG_2, 0x80 ); //soft reset
    delay(10);
    writeRegister(QMC6309_CONTROL_REG_2, 0x00); //clear reset
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
    uint16_t id;
    if(readRegister(QMC6309_REG_CHIP_ID, id))
    {
        return id;
    }
    else 
    {
        return 0xFFFF; // invalid or failed to read device chip id
    }
}

bool PCBCUPID_QMC6309::readRaw(uint16_t*  x, uint16_t* y, uint16_t* z)
{

}

PCBCUPID_QMC6309::writeRegister(uint8_t reg, uint8_t val)
{
    _wire->beginTransmission(QMC6309_I2C_ADDR);
    _wire->write((reg >> 8) & 0xFF) ;
    _wire->write(reg & 0xFF);
    _wire->write((val >> 8) & 0xFF);
    _wire->write(val & 0xFF);
    return (_wire->endTransmission() == 0);

}

PCBCUPID_QMC6309::readRegister(uint8_t reg)
{

    _wire->beginTransmission(QMC6309_I2C_ADDR);
    _wire->write((reg >> 8) & 0xFF);
    _wire->write(reg & 0xFF);
    if(_wire->endTransmission(false) != 0)
    {
        return false;
    }

    if(_wire->requestFrom(QMC6309_I2C_ADDR, (uint8_t)2) != 2)
    {
        return false;
    }



}