/**
 * @file AXP173_U8G2.ino
 * @author By mondraker (691806052@qq.com) (qq:735791683)
 * @brief  This is AXP173 PMU multi_Test
 *         ! You need to get the "U8g2lib.h" library first
 *         ! 你需要先下载 "U8g2lib.h" 库
 *         ! Arduino not support "printf", Please replace all with "print" or "println"
 *         ! Arduino 不支持 "printf", 请全部替换为 "print" 或 "println"
 * @version 0.2
 * @date 2022-12-05
 * @copyright Copyright (c) 2022
 */

#include "AXP173.h"
#include "log.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

/* U8g2 constructor */
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0,U8X8_PIN_NONE);

/* Screen Log Set */
#define U8LOG_WIDTH 30
#define U8LOG_HEIGHT 7
uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT];

/* IIC pin(ESP8266) */
// SCL  5   D1
// SDA  4   D2

//U8G2LOG u8g2log;
//AXP173 pmu;       //声明pmu对象
//I2C_PORT iic;     //声明iic对象

void setup() {

    /* Init Serial */ 
    Serial.begin(115200);     //设置波特率为 115200

    /* Init IIC */
    Wire.begin();             //IIC初始化

    /* Init PMU */
    pmu.begin(&Wire);         //AXP173 iic初始化(包含ADC以及电源，按键时长设置等等，只要不断电源，即使关机之后这些信息也会保存)
    //iic.I2C_dev_scan();     //扫描IIC设备地址

    /* Init Screen */
    u8g2.begin();

    /* Init Screen Log */
    u8g2log.begin(U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);

    delay(100);                 //在这儿停顿
    lprintf(LOG_INFO,"This is AXP173 PMU multi_Test\n");
    lprintf(LOG_INFO,"AXP173 Init Success\n");
}


void KeyPressIRQEvent(){
    if (pmu.powerState()) {
        if (pmu.getShortPressIRQState()) {      //获取对应位IRQ状态信息 true or false
            lprintf(LOG_INFO,">PEK Short Press");
            pmu.setShortPressIRQDisabale();     //对应位写1结束中断
            static bool LOCK_STATE = 1; //初始化屏幕状态
            if (LOCK_STATE == 1) {      // = 1 睡眠
                // pmu.prepareToSleep();
                LOCK_STATE = 0;
                lprintf(LOG_INFO," -Screen Lock-\n");
                u8g2.setPowerSave(1);
            }
            else if(LOCK_STATE == 0) {  // = 0 唤醒
                // pmu.RestoreFromLightSleep();
                LOCK_STATE = 1;
                lprintf(LOG_INFO," -Screen UnLock-\n");
                u8g2.setPowerSave(0);
            } 
        }
        else if (pmu.getLongPressIRQState()) {  //获取对应位IRQ状态信息 true or false
            lprintf(LOG_INFO,">PEK Long Press");
            pmu.setLongPressIRQDisabale();      //对应位写1结束中断
            static bool CONTRAST_STATE = 1; //初始化亮度状态
            if (CONTRAST_STATE) {           // = 1 暗
                CONTRAST_STATE = 0;
                lprintf(LOG_INFO," -Screen Dims-\n");
                u8g2.setContrast(0);
            }
            else if(CONTRAST_STATE == 0) {  // = 0 亮
                CONTRAST_STATE = 1;
                lprintf(LOG_INFO," -Screen Brightens-\n");
                u8g2.setContrast(255);
            } 
        }
    }
    else {
        lprintf(LOG_ERROR,"AXP173 ERROR!\n");
        delay(1000); 
    }
}


void loop(){

    // printPmuInfo();  //测试 1 打印你想要的电源信息（测试一个之前先注释另一个）

    // pmu.setEnPinEnable(true);
    // delay(3000); 
    // pmu.setEnPinEnable(false);
    // delay(3000); 

    KeyPressIRQEvent(); //测试 2 长短按键事件测试（测试一个之前先注释另一个）

    screenPrint();
}

void screenPrint() {    //屏幕绘制设置
    u8g2.firstPage();
    do {
        u8g2.drawFrame(0,0,u8g2.getDisplayWidth(),u8g2.getDisplayHeight()/4);
        u8g2.drawFrame(0,0,u8g2.getDisplayWidth(),u8g2.getDisplayHeight());
        u8g2.setFont(u8g2_font_6x13_tr);		// font for the title
        u8g2.setCursor(2, 13);			        // title position on the display
        u8g2.print("Battery Log");        // output title
        u8g2.setFont(u8g2_font_tom_thumb_4x6_mf);		// set the font for the terminal window
        u8g2.drawLog(2, 24, u8g2log);		    // draw the terminal window on the display
    } while ( u8g2.nextPage() );
}

void printPmuInfo() {   //需要打印在屏幕上的芯片信息

    /* Get PMU temp info */
    lprintf(LOG_INFO,"CoreTemp :%.2f 'C", pmu.getAXP173Temp());                //芯片温度

    /* Get VBUS info */
    // lprintf(LOG_INFO,"VBUS_voltage :%.2f V\n", pmu.getVBUSVoltage());         //VBUS输入电压
    // lprintf(LOG_INFO,"VBUS_current :%.2f mA\n", pmu.getVBUSCurrent());        //VBUS输入电流

    if(pmu.isBatExist()){
        //lprintf(LOG_INFO,"Battery :Battery Exist");                         //电池接入状态
        /* Get Battery info */
        pmu.isCharging() ? lprintf(LOG_INFO,"Charging :Is Charging s") : lprintf(LOG_INFO,"Charging :NO or END Charging");//充电状态
        lprintf(LOG_INFO,"Bat_voltage :%.2f V", pmu.getBatVoltage());           //电池电压
        lprintf(LOG_INFO,"Bat_Current :%.2f mA", pmu.getBatCurrent());          //电池电流  正为充电，负为放电
        
        lprintf(LOG_INFO,"Bat_Level :%.2f %%", pmu.getBatLevel());            //电池电量百分比显示
        //lprintf(LOG_INFO,"Bat_BatPower :%.2f W", pmu.getBatPower());            //电池瞬时功率

        // lprintf(LOG_INFO,"GetBatCoulombInput :%.2f C", pmu.GetBatCoulombInput());   //Get Coulomb charge Data
        lprintf(LOG_INFO,"GetBatCoulombOutput :%.2f C", pmu.GetBatCoulombOutput());       //Get Coulomb Discharge Data
        lprintf(LOG_INFO,"CoulometerData :%.2f C", pmu.getCoulometerData());        //get coulomb val affter calculation
    }
    else{
        lprintf(LOG_INFO,"Battery :NO Battery");                            //没电池就输出这
    }

}
