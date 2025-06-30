#ifndef QMC6309_H
#define QMC6309_H

#include <Arduino.h>
#include <Wire.h>

#define QMC6309_I2C_ADDR 0x3E // 7-bit I2C Address

// Register Mapping Address

#define QMC6309_REG_CHIP_ID 0x00
#define QMC6309_REG_X_LSB 0X01
#define QMC6309_REG_X_MSB 0x02
#define QMC6309_REG_Y_LSB 0x03
#define QMC6309_REG_Y_MSB 0x04
#define QMC6309_REG_Z_LSB 0x05
#define QMC6309_REG_Z_MSB 0x06

#define QMC6309_STATUS_REG 0x09
#define QMC6309_CONTROL_REG_1 0x0A
#define QMC6309_CONTROL_REG_2 0x0B
#define QMC6309_RESET_REG 0x0B
#define QMC6309_SELF_RESET_REG 0x0E

// Modes
#define QMC6309_MODE_SUSPEND 0x00
#define QMC6309_MODE_NORMAL 0x01
#define QMC6309_MODE_SINGLE 0x02
#define QMC6309_MODE_CONTINUOUS 0x03

// Output Data Rate
#define QMC6309_ODR_10HZ 0x01
#define QMC6309_ODR_50HZ 0x02
#define QMC6309_ODR_100HZ 0x03
#define QMC6309_ODR_200HZ 0x04

// RNG Full scale
#define QMC6309_RANGE_2G 0x00 // Actually 32 Gauss max
#define QMC6309_RANGE_8G 0x02

class PCBCUPID_QMC6309
{
public:
    QMC6309(TwoWire &w = Wire);

    bool begin();
    void reset();
    void setMode(uint8_t mode, uint8_t odr = QMC6309_ODR_200HZ);
    bool readRaw(int16_t &x, int16_t &y, int16_t &z);
    uint8_t readChipID();

private:
    TwoWire *_wire;
    void writeRegister(uint8_t reg, uint8_t val);
    uint8_t readRegister(uint8_t reg);
};

#endif