/**
 * @file AXP173.h
 * @author By mondraker (691806052@qq.com) (qq:735791683)
 * @brief
 * @version 0.1
 * @date 2022-09-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _AXP173_H_
#define _AXP173_H_

#include "I2C_PORT.h"

/* HOST MODE(TWSI) */
/* AXP173 device address */
#define AXP173_ADDR 0x34 //设备地址
/* AXP173 equipment address */
#define AXP173_ADDR_READ 0x69  //设备读地址 ((0x34 << 1) + 1)
#define AXP173_ADDR_WRITE 0x68 //设备写地址 (0x34 << 1)
/* AXP173 equipment address */
#define IRQ_PIN 16
#define IRQ_STATE digitalRead(IRQ_PIN) // IRQ中断接收引脚 D0

typedef enum
{                 //可调电压输出通道（ldo1为RTC电源，电压不可调）
    OP_DCDC1 = 0, // 0
    OP_LDO4,      // 1
    OP_LDO2,      // 2
    OP_LDO3,      // 3
    OP_DCDC2,     // 4
} OUTPUT_CHANNEL;

typedef enum
{               // ADC采集电压与电流参数 （ADC使能地址：0x82;默认值：0x83）
    ADC_TS = 0, //温敏电阻管脚ADC
    ADC_APS_V,  // APS电压
    ADC_VBUS_C, //直流输入电流
    ADC_VBUS_V, //直流输入电压
    ADC_ACIN_C, //交流输入电流
    ADC_ACIN_V, //交流输入电压
    ADC_BAT_C,  //电池输入电流
    ADC_BAT_V,  //电池输入电压
} ADC_CHANNEL;

typedef enum
{                  //电池充电电流设置 （地址：0x33;初值：0xC8）
    CHG_100mA = 0, // 0000
    CHG_190mA,     // 0001
    CHG_280mA,     // 0010
    CHG_360mA,     // 0011
    CHG_450mA,     // 0100
    CHG_550mA,     // 0101
    CHG_630mA,     // 0110
    CHG_700mA,     // 0111
    CHG_780mA,     // 1000
    CHG_880mA,     // 1001
    CHG_960mA,     // 1010
    CHG_1000mA,    // 1011
    CHG_1080mA,    // 1100
    CHG_1160mA,    // 1101
    CHG_1240mA,    // 1110
    CHG_1320mA,    // 1111
} CHARGE_CURRENT;

typedef enum
{ //库仑计控制 （地址：0xB8;默认值：0x00）
    COULOMETER_RESET = 5,
    COULOMETER_PAUSE,
    COULOMETER_ENABLE,
} COULOMETER_CTRL;

typedef enum
{                    //关机时长设置 （地址：0x36;默认值【只操作最后两位】：0x5D）
    POWEROFF_4S = 0, // 00
    POWEROFF_6S,     // 01
    POWEROFF_8S,     // 10
    POWEROFF_10S,    // 11
} POWEROFF_TIME;

typedef enum
{                      //开机时长设置
    POWERON_128mS = 0, // 00
    POWERON_512mS,     // 01
    POWERON_1S,        // 10
    POWERON_2S,        // 11
} POWERON_TIME;

typedef enum
{                  //长按键PEK多长时间触发开关机事件（地址：0x36）
    LPRESS_1S = 0, // 00
    LPRESS_1_5S,   // 01
    LPRESS_2S,     // 10
    LPRESS_2_5S,   // 11
} LONG_PRESS_TIME;

class AXP173 : public I2C_PORT
{
    private:
        uint16_t _getMin(uint16_t a, uint16_t b);
        uint16_t _getMax(uint16_t a, uint16_t b);
        uint16_t _getMid(uint16_t input, uint16_t min, uint16_t max);

        public:
        /* Init（芯片初始化）*/
        #ifdef ARDUINO // AXP173未指名IIC地址
            bool begin(TwoWire *wire);
        #else // AXP173已经指名IIC地址（arduino）
            void begin();
        #endif

        /* Power input state（输入电源状态检测） */
        //地址：0x00
        bool isACINExist();      // ACIN存在指示
        bool isACINAvl();        // ACIN是否可用
        bool isVBUSExist();      // VBUS存在指示
        bool isVBUSAvl();        // VBUS是否可用
        bool getBatCurrentDir(); //获取电池电流方向（0：在放电；1：在充电）
        //地址：0x01
        bool isAXP173OverTemp(); //指示AXP173是否过温
        bool isCharging();       //充电指示（0：未充电或已充电完成；1：正在充电）
        bool isBatExist();       //电池存在状态指示
        bool isChargeCsmaller(); //指示充电电流是否小于期望电流（0：实际充电电流等于期望电流；1：实际充电电流小于期望电流）

        /* Power output control （电源输出控制）*/
        void setOutputEnable(OUTPUT_CHANNEL channel, bool state);        // channel：设置电源输出通道（OUTPUT_CHANNEL）；state：设置是否输出
        void setOutputVoltage(OUTPUT_CHANNEL channel, uint16_t voltage); // channel：设置电源输出通道（OUTPUT_CHANNEL）；voltage：设置输出电压
                                                                        // DCDC1 & LDO4: 700~3500(mV), DCDC2: 700~2275(mV), LDO2 & LDO3: 1800~3300(mV)
        /* Basic control (开关芯片控制) */
        void powerOFF(void);                         //调用直接关机
        bool powerState(void);                       //若关机则返回false
        void setPowerOffTime(POWEROFF_TIME offTime); //设置关机时间（输入参数见POWEROFF_TIME枚举体）
        void setPowerOnTime(POWERON_TIME onTime);    //设置开机时间（输入参数见POWERON_TIME枚举体）

        /* Charge control (电池充电设置) */
        void setChargeEnable(bool state);              //充电功能使能控制位7bit（上电默认开启）
        void setChargeCurrent(CHARGE_CURRENT current); //充电电流设置0-3bit，写入电流见CHARGE_CURRENT枚举体

        /* ADC control (ADC设置) */
        void setADCEnable(ADC_CHANNEL channel, bool state); // ADC使能1 channel：设置ADC使能通道 参数见ADC_CHANNEL枚举体 state:设置是否输出
        void setChipTempEnable(bool state);                 // ADC使能2 设置芯片温度检测ADC使能 state:设置是否输出 默认输出

        /* Coulometer control (库仑计模式设置) */
        void setCoulometer(COULOMETER_CTRL option, bool state); //设置库仑计状态（开关ENABLE，暂停PAUSE，清零RESET）

        /* Coulometer data (库仑计数据) */
        uint32_t getCoulometerChargeData(void);    //电池充电库仑计数据寄存器3
        uint32_t getCoulometerDischargeData(void); //电池放电库仑计数据寄存器3
        float GetBatCoulombInput(void);            //库仑计输入计数
        float GetBatCoulombOutput(void);           //库仑计输出计数
        float getCoulometerData(void);             //计算后返回的值 get coulomb val affter calculation

        /* BAT data (电池状态数据) */
        float getBatVoltage(); //返回高八位 + 低四位电池电压   地址：高0x78 低0x79
        float getBatCurrent(); //返回高八位 + 低五位电池电流   地址：充电电流（高0x7A 低0x7B） & 放电电流（高0x7C 低0x7D）
        float getBatLevel();   //返回电池电量等级（%）
        float getBatPower();   //返回高八位 + 中八位 + 低八位电池瞬时功率  地址：高0x70 中0x71 低0x72
        // uint32_t getChargeTimeMS();

        /* VBUS data (外部输入电压状态数据) */
        float getVBUSVoltage(); //返回高八位 + 低四位USB输入电压   地址：高0x5A 低0x5B
        float getVBUSCurrent(); //返回高八位 + 低四位USB输入电流   地址：高0x5C 低0x5D

        /* Temperature data (温度监控数据) */
        float getAXP173Temp(); //返回高八位 + 低四位芯片内置温度传感器温度 地址：高0x5E 低0x5F
        float getTSTemp();     //返回高八位 + 低四位芯片TS脚热敏电阻检测到的电池温度  地址：高0x62 低0x63

        /* Read IRQ enable and state REG get PEK Long and Short Press state(读取与操作IRQ使能与状态寄存器获取长按键与短按键状态) */
        void aoToPowerOFFEnabale();                       //按键时长大于关机时长自动关机使能
        void setShortPressEnabale();                      //短按键使能REG31H[3] 调用后立刻导致短按键中断发生
        bool getShortPressIRQState();                     //读取短按键IRQ中断状态
        void initIRQState();                              //所有IRQ中断使能置零 REG40H 41H 42H 43H 4AH
        void setLongPressTime(LONG_PRESS_TIME pressTime); //设置长按键触发时间
        bool getLongPressIRQState();                      //读取长按键IRQ中断状态
        void initKeyPressIRQ(LONG_PRESS_TIME pressTime);  //初始化IRQ中断使能，设置长按键触发时间
        void setShortPressIRQDisabale();                  //对应位写1结束中断
        void setLongPressIRQDisabale();                   //对应位写1结束中断

        void prepareToSleep(void);
        void lightSleep(uint64_t time_in_us);
        void deepSleep(uint64_t time_in_us);
        void RestoreFromLightSleep(void);
};

extern AXP173 pmu;
#endif