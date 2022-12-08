/**
 * @file AXP173_U8G2.ino
 * @author By mondraker (691806052@qq.com) (qq:735791683)
 * @brief  This is AXP173 PMU Simple Test
 *        ! 你需要先下载 "Adafruit_GFX.h" 与 "Adafruit_SSD1306.h" 库
 *        ! 该例程兼容小内存单片机
 * IRQ_PIN 12
 * SDA&SCA hardwareIIC
 * @version 0.1
 * @date 2022-12-09
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "AXP173.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define WIRE Wire
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &WIRE);
/* IIC pin(ESP8266) */
// SCL  5   D1
// SDA  4   D2

void setup()
{

    /* Init Serial */
    Serial.begin(115200); //设置波特率为 115200

    /* Init IIC */
    Wire.begin(); // IIC初始化

    /* Init PMU */
    pmu.begin(&Wire); // AXP173 iic初始化(不包含ADC以及电源)
                      // iic.I2C_dev_scan();     //扫描IIC设备地址

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
}

void KeyPressIRQEvent()
{
    if (pmu.powerState())
    {
        if (pmu.getShortPressIRQState())
        { //获取对应位IRQ状态信息 true or false
            Serial.println("PEK Short Press\n");
            pmu.setShortPressIRQDisabale(); //对应位写1结束中断
            static bool LOCK_STATE = 1;     //初始化屏幕状态
            if (LOCK_STATE)
            { // = 1 睡眠
                // pmu.prepareToSleep();
                // u8g2.setPowerSave(1);
                LOCK_STATE = 0;
                Serial.println("Screen Lock\n");
            }
            else
            { // = 0 唤醒
                // pmu.RestoreFromLightSleep();
                // u8g2.setPowerSave(0);
                LOCK_STATE = 1;
                Serial.println("Screen UnLock\n");
            }
            /* 息屏时设置lightsleep或者deepsleep模式 */
        }
        else if (pmu.getLongPressIRQState())
        { //获取对应位IRQ状态信息 true or false
            Serial.println("PEK Long Press\n");
            pmu.setLongPressIRQDisabale(); //对应位写1结束中断
        }
    }
    else
    {
        display.println("AXP173 ERROR!");
        delay(1000);
    }
}

void loop()
{

    printPmuInfo();

    pmu.initKeyPressIRQ(LPRESS_1_5S); //初始化按键中断事件 （设置长按键触发时间）
    KeyPressIRQEvent();               //按键中断事件判断
}

void printPmuInfo()
{ //需要打印在屏幕上的芯片信息
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    /* Get PMU temp info */
    display.setCursor(0, 64/8*0);
    display.print("CoreTemp :");
    display.print(pmu.getAXP173Temp()); //芯片温度
    display.println(" 'C");

    /* Get VBUS info */
    //  display.println("VBUS_voltage :%.2f V\n", pmu.getVBUSVoltage());         //VBUS输入电压
    //  display.println("VBUS_current :%.2f mA\n", pmu.getVBUSCurrent());        //VBUS输入电流

    if (pmu.isBatExist())
    {
        //  display.println("Battery :Battery Exist");                         //电池接入状态
        /* Get Battery info */
        display.setCursor(0, 64/8*1);
        pmu.isCharging() ? display.println("State:Is Charging s") : display.println("State:NO Charging"); //充电状态

        display.setCursor(0, 64/8*2);
        display.print("Bat_v:");
        display.print(pmu.getBatVoltage()); //电池电压
        display.println(" V");

        display.setCursor(0, 64/8*3);
        display.print("Bat_C:");
        display.print(pmu.getBatCurrent()/1000); //电池电流  正为充电，负为放电
        display.print(" mA");

        display.setCursor(0, 64/8*4);
        display.print("Bat_Level:");
        display.print(pmu.getBatLevel()); //电池电量百分比显示 （电压检测法，非库仑计算法，充电时会显示100%）
        display.println(" %");

        //  display.print("Bat_BatPower :");
        //  display.print(pmu.getBatPower());            //电池瞬时功率
        //  display.print(" W");

        display.setCursor(0, 64/8*5);
        display.print("ColInput:");
        display.print(pmu.GetBatCoulombInput()); // Get Coulomb charge Data
        display.println(" C");

        display.setCursor(0, 64/8*6);
        display.print("ColOutput:");
        display.print(pmu.GetBatCoulombOutput());       //Get Coulomb Discharge Data
        display.println(" C");

        display.setCursor(0, 64/8*7);
        display.print("ColData:");
        display.print(pmu.getCoulometerData()); // get coulomb val affter calculation
        display.println(" C");

        display.display();
    }
    else
    {
        display.println("Battery :NO Battery"); //没电池就输出这
        display.display();
    }
}
