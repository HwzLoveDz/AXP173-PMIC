/**
 * @file AXP173.cpp
 * @author By mondraker (691806052@qq.com) (qq:735791683)
 * @brief The base library comes from m5stack,They open-sourced
 * the AXP192 library,Thanks in advance!
 * 
 * https://docs.m5stack.com/en/products
 * 
 * @version 0.2
 * @date 2022-12-05
 * @copyright Copyright (c) 2022
 */
#include "AXP173.h"

// "&" 与位运算置位，"|" 或位运算写入

/* Private functions （在类内判断数字大中小。主要是为了限制设置电压时输入超出范围，incline定义快速读取）*/
inline uint16_t AXP173::_getMin(uint16_t a, uint16_t b) {
    return ((a)<(b)?(a):(b));
}

inline uint16_t AXP173::_getMax(uint16_t a, uint16_t b) {
    return ((a)>(b)?(a):(b));
}

inline uint16_t AXP173::_getMid(uint16_t input, uint16_t min, uint16_t max) {
    return _getMax(_getMin(input, max), min);
}

/* Public functions (包含IIC的初始化以及是否初始化的判断)*/
#ifdef ARDUINO
bool AXP173::begin(TwoWire * wire) {
    _I2C_init(wire, AXP173_ADDR);

    /* Set PMU Voltage */
    setPmuPower();

    /* Set PMU Config */
    setPmuConfig();
    return _I2C_checkDevAvl();
}
#else
void AXP173::begin() {
/* 各种电压设置与ADC使能一类可以写在这里 */

    /* Set PMU Voltage */
    setPmuPower();

    /* Set PMU Config */
    setPmuConfig();
}
#endif

//写在一切IIC设备初始化前面，电源芯片必须第一个初始化，并且在其他设备iic初始化之前设置好电压，否则其他设备程序初始化完结果没供电。
void AXP173::setPmuPower() {    //电源通道电压输出设置，交换位置可以设置上电时序，中间加delay可以延迟上电
    /* Enable and set LDO2 voltage */
    setOutputEnable(OP_LDO2, true);     //LDO2设置为输出
    setOutputVoltage(OP_LDO2, 3300);    //LDO2电压设置为3.000V

    /* Enable and set LDO3 voltage */
    setOutputEnable(OP_LDO3, true);     //LDO3设置为输出
    setOutputVoltage(OP_LDO3, 3300);    //LDO3电压设置为3.300V

    /* Enable and set LDO4 voltage */
    setOutputEnable(OP_LDO4, true);     //LDO4设置为输出
    setOutputVoltage(OP_LDO4, 3300);    //LDO4电压设置为3.300V

    /* Enable and set DCDC1 voltage */
    setOutputEnable(OP_DCDC1, true);    //DCDC1设置为输出
    setOutputVoltage(OP_DCDC1, 3300);   //DCDC1电压设置为3.300V

    /* Enable and set DCDC2 voltage */
    setOutputEnable(OP_DCDC2, true);    //DCDC2设置为输出
    setOutputVoltage(OP_DCDC2, 2275);   //DCDC2电压设置为2.275V

    /* Enable Battery Charging */
    setChargeEnable(true);                      //充电功能使能
    setChargeCurrent(CHG_450mA);        //设置充电电流为450mA
}
void AXP173::setPmuConfig() {   //电源芯片ADC，库仑计等功能设置
    /* Clear IRQ */
    initIRQState();

    /* Set on time */
    setPowerOnTime(POWERON_1S);         //设置PEK开机时长为1S 

    /* Set off time */
    setPowerOffTime(POWEROFF_4S);       //设置PEK关机时长为4S（我这个芯片因为定制好像只能设置6，8，10s）

    /* Set PEKLongPress time */
    setLongPressTime(LPRESS_1_5S);      //设置PEK长按键时长为1.5S

    /* Enable VBUS ADC */
    setADCEnable(ADC_VBUS_V, true);     //VBUS ADC 电压使能
    setADCEnable(ADC_VBUS_C, true);     //VBUS ADC 电流使能

    /* Enable Battery ADC */
    setADCEnable(ADC_BAT_V, true);      //Battery ADC 电压使能
    setADCEnable(ADC_BAT_C, true);      //Battery ADC 电流使能

    /* Enable Coulometer and set COULOMETER_ENABLE*/
    setCoulometer(COULOMETER_ENABLE, true); //库仑计使能
}




/* 输入电源状态寄存器（地址：0x00）
 * 函数返回值：0 or 1
 * 函数作用：（具体见，h文件）
 * 函数写法功能解析：
 *  首先判断从输入电源状态寄存器读取的8bit值（由高到低读取7-->0）与0Bxxxxxxxx进行与运算，若两者相同则输出则为true反之为false
 * 再判断如下语句：  ("true" or" false") ? true ：false;   意为：若条件为true则返回1,反之返回0
 * 优点：直观简洁，避免使用if...else...语句拖慢处理效率
 */
bool AXP173::isACINExist() {
    return ( _I2C_read8Bit(0x00) & 0B10000000 ) ? true : false;
}

bool AXP173::isACINAvl() {
    return ( _I2C_read8Bit(0x00) & 0B01000000 ) ? true : false;
}

bool AXP173::isVBUSExist() {
    return ( _I2C_read8Bit(0x00) & 0B00100000 ) ? true : false;
}

bool AXP173::isVBUSAvl() {
    return ( _I2C_read8Bit(0x00) & 0B00010000 ) ? true : false;
}

bool AXP173::getBatCurrentDir() {
    return ( _I2C_read8Bit(0x00) & 0B00000100 ) ? true : false;
}

/* 电源工作模式以及充电状态指示寄存器（地址：0x01）
 * 函数返回值：0 or 1
 * 函数作用：（具体见，h文件）
 * 函数写法功能解析：（同上）
 */
bool AXP173::isAXP173OverTemp() {
    return ( _I2C_read8Bit(0x01) & 0B10000000 ) ? true : false;
}

bool AXP173::isCharging() {
    return ( _I2C_read8Bit(0x01) & 0B01000000 ) ? true : false;
}

bool AXP173::isBatExist() {
    return ( _I2C_read8Bit(0x01) & 0B00100000 ) ? true : false;
}

bool AXP173::isChargeCsmaller() {
    return ( _I2C_read8Bit(0x01) & 0B00000100 ) ? true : false;
}




/* 电源输出控制寄存器（地址：0x12）
 * 函数返回值：None
 * 函数作用：开关某一通道电源输出（具体见，h文件）
 * 函数写法功能解析：(见附件：pmu_outPutState_test.c)
 */
void AXP173::setOutputEnable(OUTPUT_CHANNEL channel, bool state) {
    uint8_t buff = _I2C_read8Bit(0x12); //buff类型为unsigned char
    buff = state ? ( buff | (1U << channel) ) : ( buff & ~(1U << channel) );
    _I2C_write1Byte(0x12, buff);        //对0x12寄存器写入1字节也就是8位二进制buff
}

/* 外部升压芯片使能（EN脚高低电平控制） */
void AXP173::setEnPinEnable(bool state) {
    uint8_t buff = _I2C_read8Bit(0x12); //buff类型为unsigned char
    buff = state ? ( (buff & 0B10111111) | 0x01000000 ) : ( buff & ~0x01000000 );
    _I2C_write1Byte(0x12, buff);        //对0x12寄存器写入1字节也就是8位二进制buff
}

/* 电源输出电压配置寄存器
 * DC-DC2     0x23  25mV/step   7-6bit(stable)   5-0bit(usage)
 * DC-DC1     0x26  25mV/step   7bit(stable)     6-0bit(usage)
 * LDO4       0x27  25mV/step   7bit(stable)     6-0bit(usage)
 * LDO2&LDO3  0x28  100mV/step  None(stable)     7-4bit&3-0bit(usage)
 * 函数返回值：None
 * 函数作用：控制某一通道电源电压输出大小（具体见，h文件）
 * 
 * 函数写法功能解析：_getMid()可以避免用户输入过大或者过小的值导致程序意外错误，输入过小值直接
 * 输出最小电压，过大值直接输出最大电压。然后将用户输入值转化为对应的电压乘数（step）
 * (buff & 0B10000000)：将buff转为八位二进制,重置电压设置位0-6bit,"维持保留位7bit" !!!
 * (voltage & 0B01111111)：将voltage转为八位二进制,维持待写入电压位0-6bit，"重置保留位7bit" !!!
 * (buff & 0B10000000) | (voltage & 0B01111111)：最后的或运算结束后，只有0-6bit改变了，7bit还仍
 * 然维持读取时的状态，起到了不更改保留位但却更改了电压位的作用！！！
 */
void AXP173::setOutputVoltage(OUTPUT_CHANNEL channel, uint16_t voltage) {
    uint8_t buff = 0;
    switch (channel) {
        case OP_DCDC1:
            voltage = (_getMid(voltage, 700, 3500) - 700) / 25; //0 - 112(step)
            buff = _I2C_read8Bit(0x26);
            buff = (buff & 0B10000000) | (voltage & 0B01111111);
            _I2C_write1Byte(0x26, buff);
            break;
        case OP_DCDC2:
            voltage = (_getMid(voltage, 700, 2275) - 700) / 25;
            buff = _I2C_read8Bit(0x23);
            buff = (buff & 0B11000000) | (voltage & 0B00111111);
            _I2C_write1Byte(0x23, buff);
            break;
        case OP_LDO2:
            voltage = (_getMid(voltage, 1800, 3300) - 1800) / 100;
            buff = _I2C_read8Bit(0x28);
            buff = (buff & 0B00001111) | (voltage << 4);
            _I2C_write1Byte(0x28, buff);
            break;
        case OP_LDO3:
            voltage = (_getMid(voltage, 1800, 3300) - 1800) / 100;
            buff = _I2C_read8Bit(0x28);
            buff = (buff & 0B11110000) | (voltage);
            _I2C_write1Byte(0x28, buff);
            break;
        case OP_LDO4:
            voltage = (_getMid(voltage, 700, 3500) - 700) / 25;
            buff = _I2C_read8Bit(0x27);
            buff = (buff & 0B10000000) | (voltage & 0B01111111);
            _I2C_write1Byte(0x27, buff);
            break;
        default:
            break;
    }
}




/* 开关芯片控制寄存器（地址：0x32）
 * 函数返回值：None
 * 函数作用：开关芯片输出（具体见，h文件）
 * 
 * 函数写法功能解析：给该寄存器7bit位写 "1" 会关闭芯片所有输出，为了不改变该寄存器的其它
 * 位设置，让二进制形式仅对7bit写一"0B1000 0000"和读取到的原寄存器状态做 "|" 或位运算即
 * 可。由于关机后该位上电会自动置 "0",因此更改时可以不使用 "&" 与位运算重置该位.
 */
void AXP173::powerOFF(void) {                   //关闭芯片所有输出
    _I2C_write1Byte(0x32, (_I2C_read8Bit(0x32) | 0B10000000));
}

bool AXP173::powerState(void) {              //若关机则返回false
    return ( _I2C_read8Bit(0x32) & 0B10000000 ) ? false : true;
}

/* 长按按键芯片开关机时间设置寄存器（地址：0x36）
 * 函数返回值：None
 * 函数作用：设置长按按键芯片开关机时间（具体见，h文件）
 * 
 * 函数写法功能解析："&" 运算先将 "4 and 5 bit" 置位，然后保留其它位已经写 "1" 的配置，
 * 最后 "|" 运算对 "4 and 5 bit" 写"入 " 0 or 1 "
 */
void AXP173::setPowerOnTime(POWERON_TIME onTime) {          //7 and 6 bit   开机时间
    _I2C_write1Byte(0x36, ((_I2C_read8Bit(0x36) & 0B00111111) | onTime));
}

void AXP173::setPowerOffTime(POWEROFF_TIME offTime) {       //0 and 1 bit   关机时间
    _I2C_write1Byte(0x36, ((_I2C_read8Bit(0x36) & 0B11111100) | offTime));
}




/* 充电控制寄存器1（地址：0x33）
 * 函数返回值：None
 * 函数作用：设置充电电流以及是否使能充电功能，充电目标电压(5 and 6 bit)默认为4.2V(1 and 0)
 * 
 * 函数写法功能解析：同上 "&" 与位运算置位，"|" 或位运算写入
 */
void AXP173::setChargeEnable(bool state) {              //充电功能使能控制位7bit
    if (state)
        _I2C_write1Byte(0x33, ((_I2C_read8Bit(0x33) | 0B10000000)));
    else
        _I2C_write1Byte(0x33, ((_I2C_read8Bit(0x33) & 0B01111111)));
}

void AXP173::setChargeCurrent(CHARGE_CURRENT current) { //写入电流见CHARGE_CURRENT枚举体
    _I2C_write1Byte(0x33, ((_I2C_read8Bit(0x33) & 0B11110000) | current));
}




/* ADC使能寄存器1（地址：0x82）
 * 函数返回值：None
 * 函数作用：设置ADC使能
 * 
 * 函数写法功能解析：同 "电源输出控制寄存器" 参数见ADC_CHANNEL枚举体
 */
void AXP173::setADCEnable(ADC_CHANNEL channel, bool state) {
    uint8_t buff = _I2C_read8Bit(0x82);
    buff = state ? ( buff | (1U << channel) ) : ( buff & ~(1U << channel) );
    _I2C_write1Byte(0x82, buff);
}

/* ADC使能寄存器2（地址：0x83）
 * 函数返回值：None
 * 函数作用：设置芯片温度检测ADC使能（默认开启）
 * 
 * 函数写法功能解析：同上
 */
void AXP173::setChipTempEnable(bool state) {
    uint8_t buff = _I2C_read8Bit(0x83);
    buff = state ? ( buff | 0B10000000 ) : ( buff & 0B01111111 );
    _I2C_write1Byte(0x83, buff);
}




/* 库仑计控制寄存器（地址：0xB8）
 * 函数返回值：None
 * 函数作用：设置库仑计（开关ENABLE 7bit，暂停PAUSE 6bit，清零RESET 5bit。5 and 6 bit 操作结束后会自动置零，0-4bit为保留位）
 * 
 * 函数写法功能解析：同上
 */
void AXP173::setCoulometer(COULOMETER_CTRL option, bool state) {
    uint8_t buff = _I2C_read8Bit(0xB8);
    buff = state ? ( buff | (1U << option) ) : ( buff & ~(1U << option) );
    _I2C_write1Byte(0xB8, buff);
}




/* 以上为设置寄存器数据 */
/* 以下为读取寄存器数据 */




/* IRQ引脚中断寄存器（地址：0xB8）
 * 函数返回值：None
 * 函数作用：设置库仑计（开关ENABLE 7bit，暂停PAUSE 6bit，清零RESET 5bit。5 and 6 bit 操作结束后会自动置零，0-4bit为保留位）
 * 
 * 函数写法功能解析：同上
 */





/* 库仑计数据读取寄存器（地址：0xB0、0xB4）(待理解)
 * 函数返回值：float
 * 函数作用：读取库仑计数值并且计算电池电量
 * 
 * 函数写法功能解析：(2^16) * 0.5 * (int32_t)(CCR - DCR) / 3600.0 / 25.0;
 * 
 * SOC = RM / FCC：荷电状态% = 剩余电容量 / 完全充电容量
 * RM = CCR - DCR：剩余电容量 = 充电计数器寄存器 - 放电计数器寄存器
 * 
 * (2^16)：65536
 * 0.5：current_LSB（数据手册说库仑计精度为0.5%）
 * (int32_t)：将结果强制转换为正数
 * (CCR - DCR)：剩余电容量mAh
 * 3600.0：1mAh = 0.001A * 3600s = 3.6库仑，将剩余电容量转化为电流
 * 25.0：ADC rate (0x84寄存器 "6 and 7 bit" 默认为 "00",使得ADC速率为"25*(2^0)"Hz)
 */
inline uint32_t AXP173::getCoulometerChargeData(void) {     //电池充电库仑计数据寄存器3（积分后数据）
    return _I2C_read32Bit(0xB0);
}

inline uint32_t AXP173::getCoulometerDischargeData(void) {  //电池放电库仑计数据寄存器3（积分后数据）
    return _I2C_read32Bit(0xB4);
}

float AXP173::GetBatCoulombInput(void) {
  uint32_t ReData = _I2C_read32Bit(0xB0);
  return ReData * 65536 * 0.5 / 3600 / 25.0;
}

float AXP173::GetBatCoulombOutput(void) {
  uint32_t ReData = _I2C_read32Bit(0xB4);
  return ReData * 65536 * 0.5 / 3600 / 25.0;
}

float AXP173::getCoulometerData(void) {                     //返回库仑计计算数据
    uint32_t coin = getCoulometerChargeData();
    uint32_t coout = getCoulometerDischargeData();
    // data = 65536 * current_LSB（电池电流ADC精度为0.5mA） * (coin - coout) / 3600（单位换算） / ADC rate (0x84寄存器 "6 and 7 bit" 默认为 "00",使得ADC速率为"25*(2^0)"Hz)
    float CCC = 65536 * 0.5 * (int32_t)(coin - coout) / 3600.0 / 25.0;
    return CCC;
}




//返回高八位 + 低四位电池电压   地址：高0x78 低0x79 精度：1.1mV
float AXP173::getBatVoltage() {
    float ADCLSB = 1.1 / 1000.0;
    return _I2C_read12Bit(0x78) * ADCLSB;
}

//返回高八位 + 低五位电池电流   地址：充电电流（高0x7A 低0x7B） & 放电电流（高0x7C 低0x7D） 精度：0.5mA
float AXP173::getBatCurrent() {
    float ADCLSB = 0.5;
    uint16_t CurrentIn = _I2C_read13Bit(0x7A);
    uint16_t CurrentOut = _I2C_read13Bit(0x7C);
    return (CurrentIn - CurrentOut) * ADCLSB;
}

//返回电池电量等级（%）
float AXP173::getBatLevel() {
    const float batVoltage = getBatVoltage();
    const float batPercentage = (batVoltage < 3.248088) ? (0) : (batVoltage - 3.120712) * 100;       
    return (batPercentage <= 100) ? batPercentage : 100;    
}

//返回高八位 + 中八位 + 低八位电池瞬时功率  地址：高0x70 中0x71 低0x72 数据乘以精度减小误差
float AXP173::getBatPower() {
    float VoltageLSB = 1.1;
    float CurrentLCS = 0.5;
    uint32_t ReData = _I2C_read24Bit(0x70);
    return VoltageLSB * CurrentLCS * ReData / 1000.0;    
}

/* uint32_t AXP173::getChargeTimeMS() {

    static uint32_t chargeTime = 0;
    static uint32_t startTime = 0;
    uint32_t nowTime = millis();

    if (isCharging())
    {
        chargeTime = nowTime - startTime;
        return chargeTime;
    }
    else
    {
        startTime = nowTime;
        return chargeTime;
    }
    
} */



//返回高八位 + 低四位USB输入电压   地址：高0x5A 低0x5B  精度：1.7mV
float AXP173::getVBUSVoltage() {
    float ADCLSB = 1.7 / 1000.0; 
    uint16_t ReData = _I2C_read12Bit(0x5A);
    return ReData * ADCLSB;
}

//返回高八位 + 低四位USB输入电流   地址：高0x5C 低0x5D  精度：0.375mA
float AXP173::getVBUSCurrent() {
    float ADCLSB = 0.375;
    uint16_t ReData = _I2C_read12Bit(0x5C);
    return ReData * ADCLSB;
}



//返回高八位 + 低四位芯片内置温度传感器温度 地址：高0x5E 低0x5F 精度：0.1℃  最小值-144.7℃
float AXP173::getAXP173Temp() {
    float ADCLSB = 0.1;
    const float OFFSET_DEG_C = -144.7;
    uint16_t ReData = _I2C_read12Bit(0x5E);
    return OFFSET_DEG_C + ReData * ADCLSB;
}

//返回高八位 + 低四位芯片TS脚热敏电阻检测到的电池温度  地址：高0x62 低0x63 精度：0.1℃  最小值-144.7℃
float AXP173::getTSTemp() {
    float ADCLSB = 0.1;
    const float OFFSET_DEG_C = -144.7;
    uint16_t ReData = _I2C_read12Bit(0x62);
    return OFFSET_DEG_C + ReData * ADCLSB;
}




/* 按键状态检测 */
void AXP173::aoToPowerOFFEnabale(void) {        //按键时长大于关机时长自动关机  
    _I2C_write1Byte(0x36, (_I2C_read8Bit(0x36) | 0B00001000));
}
void AXP173::initIRQState(void) {               //所有IRQ中断使能置零REG40H 41H 42H 43H 4AH
    _I2C_write1Byte(0x40, ((_I2C_read8Bit(0x40) & 0B00000001) | 0B00000000));
    _I2C_write1Byte(0x41, ((_I2C_read8Bit(0x41) & 0B00000000) | 0B00000000));
    _I2C_write1Byte(0x42, ((_I2C_read8Bit(0x42) & 0B00000100) | 0B00000000));
    _I2C_write1Byte(0x43, ((_I2C_read8Bit(0x43) & 0B11000010) | 0B00000000));
    _I2C_write1Byte(0x4A, ((_I2C_read8Bit(0x4A) & 0B01111111) | 0B00000000));
}

void AXP173::setShortPressEnabale(void) {       //短按键使能REG31H[3] 调用后立刻导致短按键中断发生
    _I2C_write1Byte(0x31, (_I2C_read8Bit(0x31) | 0B00001000));
}
bool AXP173::getShortPressIRQState(void) {      //读取短按键IRQ中断状态
    return ( _I2C_read8Bit(0x46) & 0B00000010 ) ? true : false;
}
void AXP173::setShortPressIRQDisabale(void) {   //短按键对应位写1结束中断
    _I2C_write1Byte(0x46, (_I2C_read8Bit(0x46) | 0B00000010));
}


void AXP173::setLongPressTime(LONG_PRESS_TIME pressTime) {  //设置长按键触发时间 5 and 4 bit
    _I2C_write1Byte(0x36, ((_I2C_read8Bit(0x36) & 0B11001111) | pressTime));
}
bool AXP173::getLongPressIRQState(void) {       //读取长按键IRQ中断状态
    return ( _I2C_read8Bit(0x46) & 0B00000001 ) ? true : false;
}
void AXP173::setLongPressIRQDisabale(void) {    //长按键对应位写1结束中断
    _I2C_write1Byte(0x46, (_I2C_read8Bit(0x46) | 0B00000001));
}




/* 按键与睡眠 */
void AXP173::prepareToSleep(void) {             //ldo断电

    //setOutputEnable(AXP173::OP_LDO3, false);     //LDO3关闭输出

}

void AXP173::lightSleep(uint64_t time_in_us) {  //类似于锁屏，需要打开REG31[3]

    //prepareToSleep();

}

void AXP173::deepSleep(uint64_t time_in_us) {   //ldo断电加MCU低功耗模式
    // prepareToSleep();

    // /* nnn */
    // setSleepMode (WiFiSleepType_t type, int listenInterval=0);  //WiFiSleepType_t type, int listenInterval=0


    // RestoreFromLightSleep();
}

void AXP173::RestoreFromLightSleep(void) {      //ldo重启输出

    //setOutputEnable(AXP173::OP_LDO3, true);     //LDO3设置为输出
    //setOutputVoltage(AXP173::OP_LDO3, 3300);    //LDO3电压设置为3.300V

}

AXP173 pmu;