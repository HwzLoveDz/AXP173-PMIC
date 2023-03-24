#ifndef _I2C_PORT_H_
#define _I2C_PORT_H_

#ifdef ARDUINO
#include <Arduino.h>
#include "Wire.h"
#endif

/* I2C device scan function */
#define ENABLE_I2C_DEV_SCAN 0
// 1000ms default read timeout (modify with "I2Cdev::readTimeout = [ms];")
#define I2CDEV_DEFAULT_READ_TIMEOUT     1000

/* I2C_PORT base class */
class I2C_PORT {
    private:
        uint8_t _dev_addr;
        //uint8_t devAddr;
        #ifdef ARDUINO
        TwoWire * _wire;
        #endif
    protected:
        /* Port interface */
        #ifdef ARDUINO
        void     _I2C_init(TwoWire * wire, uint8_t dev_addr);
        bool     _I2C_checkDevAvl();    //检测有无设备存在
        #else
        void     _I2C_init(uint8_t dev_addr);
        void     _I2C_checkDevAvl();
        #endif
        void     _I2C_write1Byte(uint8_t addr, uint8_t data);
        void     _I2C_write16Bit(uint8_t addr, uint8_t data_1, uint8_t data_2);
        uint8_t  _I2C_read8Bit(uint8_t addr);
        uint16_t _I2C_read12Bit(uint8_t addr);
        uint16_t _I2C_read13Bit(uint8_t addr);
        uint16_t _I2C_read16Bit(uint8_t addr);
        uint16_t _I2C_read16Bit_lowFirst(uint8_t addr);
        uint32_t _I2C_read24Bit(uint8_t addr);
        uint32_t _I2C_read32Bit(uint8_t addr);
        void     _I2C_readBuff(uint8_t addr, int size, uint8_t buff[]);
        // ! 以下为移植的更好的写法
        // ^        read function
        int8_t  _I2C_readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint16_t timeout=I2C_PORT::readTimeout);
        int8_t  _I2C_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout=I2C_PORT::readTimeout);
        int8_t  _I2C_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint16_t timeout=I2C_PORT::readTimeout);
        int8_t  _I2C_readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint16_t timeout=I2C_PORT::readTimeout);
        int8_t  _I2C_readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data, uint16_t timeout=I2C_PORT::readTimeout);
        int8_t  _I2C_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout=I2C_PORT::readTimeout);
        // ^        write function
        bool    _I2C_writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
        bool    _I2C_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
        bool    _I2C_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
        bool    _I2C_writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
        bool    _I2C_writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);
        bool    _I2C_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
    public:
        // int I2C_dev_scan(); //扫描iic设备地址
        static uint16_t readTimeout;
};

#endif