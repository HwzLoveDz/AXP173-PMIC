#include "I2C_PORT.h"

/* Port interface */
#ifdef ARDUINO
void I2C_PORT::_I2C_init(TwoWire *wire, uint8_t dev_addr)
{
    _wire = wire;
    _dev_addr = dev_addr;
    //devAddr = dev_addr;
}

bool I2C_PORT::_I2C_checkDevAvl()
{
    _wire->beginTransmission(_dev_addr);
    return (_wire->endTransmission() ? true : false);
}

void I2C_PORT::_I2C_write1Byte(uint8_t addr, uint8_t data)
{
    _wire->beginTransmission(_dev_addr);
    _wire->write(addr);
    _wire->write(data);
    _wire->endTransmission();
}

uint8_t I2C_PORT::_I2C_read8Bit(uint8_t addr)
{
    _wire->beginTransmission(_dev_addr);
    _wire->write(addr);
    _wire->endTransmission();
    _wire->requestFrom(_dev_addr, (size_t)1);
    return _wire->read();
}

uint16_t I2C_PORT::_I2C_read12Bit(uint8_t addr)
{
    uint8_t buff[2];
    _I2C_readBuff(addr, 2, buff);
    return (buff[0] << 4) + buff[1];
}

uint16_t I2C_PORT::_I2C_read13Bit(uint8_t addr)
{
    uint8_t buff[2];
    _I2C_readBuff(addr, 2, buff);
    return (buff[0] << 5) + buff[1];
}

uint16_t I2C_PORT::_I2C_read16Bit(uint8_t addr)
{
    uint8_t buff[2];
    _I2C_readBuff(addr, 2, buff);
    return (buff[0] << 8) + buff[1];
}

uint32_t I2C_PORT::_I2C_read24Bit(uint8_t addr)
{
    uint8_t buff[4];
    _I2C_readBuff(addr, 3, buff);
    return (buff[0] << 16) + (buff[1] << 8) + buff[2];
}

uint32_t I2C_PORT::_I2C_read32Bit(uint8_t addr)
{
    uint8_t buff[4];
    _I2C_readBuff(addr, 4, buff);
    return (buff[0] << 24) + (buff[1] << 16) + (buff[2] << 8) + buff[3];
}

void I2C_PORT::_I2C_readBuff(uint8_t addr, int size, uint8_t buff[])
{
    _wire->beginTransmission(_dev_addr);
    _wire->write(addr);
    _wire->endTransmission();
    _wire->requestFrom(_dev_addr, (size_t)size);
    for (int i = 0; i < size; i++)
    {
        buff[i] = _wire->read();
    }
}

uint16_t I2C_PORT::_I2C_read16Bit_lowFirst(uint8_t addr)
{
    uint8_t buff[2];
    _I2C_readBuff(addr, 2, buff);
    return buff[0] + (buff[1] << 8);
}

void I2C_PORT::_I2C_write16Bit(uint8_t addr, uint8_t data_1, uint8_t data_2)
{
    _wire->beginTransmission(_dev_addr);
    _wire->write(addr);
    _wire->write(data_1);
    _wire->write(data_2);
    _wire->endTransmission();
}

// ! 以下为移植的更好的写法

/** Default timeout value for read operations.
 * Set this to 0 to disable timeout detection.
 */
uint16_t I2C_PORT::readTimeout = I2CDEV_DEFAULT_READ_TIMEOUT;

// ^        read function

/** Read a single bit from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2C_PORT::_I2C_readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint16_t timeout)
{
    uint8_t b;
    uint8_t count = _I2C_readByte(devAddr, regAddr, &b, timeout);
    *data = b & (1 << bitNum);
    return count;
}

/** Read single byte from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for byte value read from device
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2C_PORT::_I2C_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout)
{
    return _I2C_readBytes(devAddr, regAddr, 1, data, timeout);
}

/** Read multiple bits from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2C_PORT::_I2C_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint16_t timeout)
{
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t count, b;
    if ((count = _I2C_readByte(devAddr, regAddr, &b, timeout)) != 0)
    {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        b &= mask;
        b >>= (bitStart - length + 1);
        *data = b;
    }
    return count;
}

/** Read single word from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for word value read from device
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2C_PORT::_I2C_readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint16_t timeout)
{
    return _I2C_readWords(devAddr, regAddr, 1, data, timeout);
}

// & read主要移植的这个：readWords
/** Read multiple words from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of words to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of words read (0 indicates failure)
 */
int8_t I2C_PORT::_I2C_readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data, uint16_t timeout)
{
    int8_t count = 0;

    _wire->beginTransmission(devAddr);
    _wire->write(regAddr);
    _wire->endTransmission();

    _wire->beginTransmission(devAddr);
    _wire->requestFrom(devAddr, (uint8_t)(length * 2)); // length=words, this wants bytes

    uint32_t t1 = millis();
    bool msb = true; // starts with MSB, then LSB
    for (; _wire->available() && count < length && (timeout == 0 || millis() - t1 < timeout);)
    {
        if (msb)
        {
            // first byte is bits 15-8 (MSb=15)

            data[count] = _wire->read() << 8;
        }
        else
        {
            // second byte is bits 7-0 (LSb=0)
            data[count] |= _wire->read();
            count++;
        }
        msb = !msb;
    }
    if (timeout > 0 && millis() - t1 >= timeout && count < length)
        count = -1; // timeout

    _wire->endTransmission();

    return count;
}
// & read主要移植的这个：readBytes
/** Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of bytes read (0 indicates failure)
 */
int8_t I2C_PORT::_I2C_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout)
{

    int8_t count = 0;

    _wire->beginTransmission(devAddr);
    _wire->write(regAddr);

    _wire->endTransmission();

    _wire->beginTransmission(devAddr);
    _wire->requestFrom(devAddr, length);

    uint32_t t1 = millis();
    for (; _wire->available() && (timeout == 0 || millis() - t1 < timeout); count++)
    {
        data[count] = _wire->read();
    }
    if (timeout > 0 && millis() - t1 >= timeout && count < length)
        count = -1; // timeout

    _wire->endTransmission();

    return count;
}

// ^        write function

/** write a single bit in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
bool I2C_PORT::_I2C_writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data)
{
    uint8_t b;
    _I2C_readByte(devAddr, regAddr, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return _I2C_writeByte(devAddr, regAddr, b);
}

/** Write single byte to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New byte value to write
 * @return Status of operation (true = success)
 */
bool I2C_PORT::_I2C_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data)
{
    return _I2C_writeBytes(devAddr, regAddr, 1, &data);
}

/** Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
bool I2C_PORT::_I2C_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data)
{
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t b;
    if (_I2C_readByte(devAddr, regAddr, &b) != 0)
    {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask;                     // zero all non-important bits in data
        b &= ~(mask);                     // zero all important bits in existing byte
        b |= data;                        // combine data with existing byte
        return _I2C_writeByte(devAddr, regAddr, b);
    }
    else
    {
        return false;
    }
}

/** Write single word to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New word value to write
 * @return Status of operation (true = success)
 */
bool I2C_PORT::_I2C_writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data)
{
    return _I2C_writeWords(devAddr, regAddr, 1, &data);
}

// & write主要移植的这个：writeWords
/** Write multiple words to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of words to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
bool I2C_PORT::_I2C_writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data)
{
    uint8_t status = 0;
    _wire->beginTransmission(devAddr);
    _wire->write(regAddr); // send address
    for (uint8_t i = 0; i < length * 2; i++)
    {
        _wire->write((uint8_t)(data[i++] >> 8)); // send MSB
        _wire->write((uint8_t)data[i]);          // send LSB
    }
    status = _wire->endTransmission();
    return status == 0;
}
// & write主要移植的这个：writeBytes
/** Write multiple bytes to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of bytes to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
bool I2C_PORT::_I2C_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data)
{
    uint8_t status = 0;
    _wire->beginTransmission(devAddr);
    _wire->write((uint8_t)regAddr); // send address
    for (uint8_t i = 0; i < length; i++)
    {
        _wire->write((uint8_t)data[i]);
    }
    status = _wire->endTransmission();
    return status == 0;
}

// ! End


// /**
//  * @brief I2C device scan
//  *
//  * @return int I2C device number
//  */
// int I2C_PORT::I2C_dev_scan()
// {
//     uint8_t error, address;
//     int nDevices;

//     Serial.println("[I2C_SCAN] device scanning...");

//     nDevices = 0;
//     for (address = 1; address < 127; address++)
//     {
//         // The i2c_scanner uses the return value of
//         // the Write.endTransmisstion to see if
//         // a device did acknowledge to the address.
//         Wire.beginTransmission(address);
//         error = Wire.endTransmission();

//         if (error == 0)
//         {
//             Serial.print("[I2C_SCAN]: device found at address 0x");
//             if (address < 16)
//                 Serial.print("0");
//             Serial.print(address, HEX);
//             Serial.println(" !");

//             nDevices++;
//         }
//         else if (error == 4)
//         {
//             Serial.print("[I2C_SCAN]: unknow error at address 0x");
//             if (address < 16)
//                 Serial.print("0");
//             Serial.println(address, HEX);
//         }
//     }

//     Serial.print("[I2C_SCAN]:");
//     Serial.printf(" %d devices was found\r\n", nDevices);
//     return nDevices;
// }

#else
void I2C_PORT::_I2C_init(uint8_t dev_addr) {}
void I2C_PORT::_I2C_checkDevAvl() {}
#endif
