/**
 * @file AXP173_U8G2.ino
 * @author By mondraker (691806052@qq.com) (qq:735791683)
 * @brief  This is AXP173 PMU buttonLog&batteryInfo Test
 *         ! You need to get the "U8g2lib.h" library first
 *         ! 你需要先下载 "U8g2lib.h" 库
 *         ! Arduino not support "printf", Please replace all with "print" or "println"
 *         ! Arduino 不支持 "printf", 请全部替换为 "print" 或 "println"
 * IRQ_PIN 12
 * SDA&SCA hardwareIIC
 * @version 0.2
 * @date 2022-12-09
 * @copyright Copyright (c) 2022
 */

#include "OneButton.h"
#include "AXP173.h"
#include "log.h"

// Setup a new OneButton on pin D5.
OneButton buttonPress(D5, true);
// Setup a new OneButton on pin D3.
OneButton buttonUp(D3, true);
// Setup a new OneButton on pin D4.
OneButton buttonDown(D4, true);

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

/* U8g2 constructor */
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
bool LOCK_STATE = 1;     //初始化屏幕状态

/* Screen Log Set */
#define U8LOG_WIDTH 30
#define U8LOG_HEIGHT 7
uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];

/* IIC pin(ESP8266) */
// SCL  5   D1
// SDA  4   D2

// U8G2LOG u8g2log;
// AXP173 pmu;       //声明pmu对象
// I2C_PORT iic;     //声明iic对象

uint8_t test_log = 0;
uint8_t func_index = 0;
void (*current_operation_index)();
void fun1();    //菜单1-1
void fun2();    //菜单1-2
void page_1_to_2();//菜单1光标
void fun3();    //菜单1-1-1
void fun4();    //菜单1-2-1

typedef struct
{
    uint8_t current;
    uint8_t up;     // 向上翻索引号
    uint8_t down;   // 向下翻索引号
    uint8_t enter;  // 确认索引号
    uint8_t back;   // 返回翻索引号
    void (*current_operation)();
} key_table;

key_table table[4]=
{//{界面序号，上，下，确认}
    {0,1,1,2,0,(*fun1)},
    {1,0,0,3,1,(*fun2)},

    {2,2,2,2,0,(*fun3)},
    {3,3,3,3,1,(*fun4)},
};

void setup()
{

    /* Init Serial */
    Serial.begin(115200); //设置波特率为 115200

    /* Init IIC */
    Wire.begin(); // IIC初始化

    /* Init PMU */
    pmu.begin(&Wire); // AXP173 iic初始化(包含ADC以及电源，按键时长设置等等，只要不断电源，即使关机之后这些信息也会保存)
    // iic.I2C_dev_scan();     //扫描IIC设备地址

    /* Init Screen */
    // u8g2.begin();
    u8g2.begin(/*Select=*/ D5, /*Right/Next=*/ U8X8_PIN_NONE, /*Left/Prev=*/ U8X8_PIN_NONE, /*Up=*/ D3, /*Down=*/ D4, /*Home/Cancel=*/ U8X8_PIN_NONE); // Arduboy DevKit
    /* Init Screen Log */
    u8g2log.begin(U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);

    /* Init Button Function */
    buttonFunctionInit();

    // lprintf(LOG_INFO, ">This is AXP173 PMU multi_Test\n");
    // lprintf(LOG_INFO, " -AXP173 Init Success\n");
    // lprintf(LOG_INFO, " -OneButton Init Success\n");

    delay(100); //在这儿停顿
}

void loop()
{
    /*******************按键索引****************************/
    buttonHandle();
    current_operation_index = table[func_index].current_operation;
    (*current_operation_index)(); // 执行当前操作函数
}

void KeyPressIRQEvent()
{
    if (pmu.powerState())
    {
        if (pmu.getShortPressIRQState())
        { //获取对应位IRQ状态信息 true or false
            lprintf(LOG_INFO, ">PEK Short Press");
            pmu.setShortPressIRQDisabale(); //对应位写1结束中断
            // static bool LOCK_STATE = 1;     //初始化屏幕状态
            if (LOCK_STATE == 1)
            { // = 1 睡眠
                // pmu.prepareToSleep();
                LOCK_STATE = 0;
                lprintf(LOG_INFO, " -Screen Lock-\n");
                u8g2.setPowerSave(1);
            }
            else if (LOCK_STATE == 0)
            { // = 0 唤醒
                // pmu.RestoreFromLightSleep();
                LOCK_STATE = 1;
                lprintf(LOG_INFO, " -Screen UnLock-\n");
                u8g2.setPowerSave(0);
            }
        }
        else if (pmu.getLongPressIRQState())
        { //获取对应位IRQ状态信息 true or false
            lprintf(LOG_INFO, ">PEK Long Press");
            pmu.setLongPressIRQDisabale();  //对应位写1结束中断
            static bool CONTRAST_STATE = 1; //初始化亮度状态
            if (CONTRAST_STATE)
            { // = 1 暗
                CONTRAST_STATE = 0;
                LOCK_STATE = 1;
                u8g2.setPowerSave(0);
                lprintf(LOG_INFO, " -Screen Dims-\n");
                u8g2.setFont(u8g2_font_6x12_tr);
                u8g2.userInterfaceMessage(
                    "PEK Long Press",
                    "-Screen Dims-",
                    "",
                    " ok \n cancel "
                );
                u8g2.setContrast(0);
            }
            else if (CONTRAST_STATE == 0)
            { // = 0 亮
                CONTRAST_STATE = 1;
                LOCK_STATE = 1;
                u8g2.setPowerSave(0);
                lprintf(LOG_INFO, " -Screen Brightens-\n");
                u8g2.setFont(u8g2_font_6x12_tr);
                u8g2.userInterfaceMessage(
                    "PEK Long Press",
                    "-Screen Brightens-",
                    "",
                    " ok \n cancel "
                );
                u8g2.setContrast(255);
            }
            u8g2.setFont(u8g2_font_6x13_tr);
        }
    }
    else
    {
        lprintf(LOG_ERROR, "AXP173 ERROR!\n");
        delay(1000);
    }
}

void screenPrint()
{ //屏幕绘制设置
    u8g2.firstPage();
    do
    {
        u8g2.drawFrame(0, 0, u8g2.getDisplayWidth(), u8g2.getDisplayHeight() / 4);
        u8g2.drawFrame(0, 0, u8g2.getDisplayWidth(), u8g2.getDisplayHeight());
        u8g2.setFont(u8g2_font_6x13_tr);          // font for the title
        u8g2.setCursor(2+5*6, 13);                    // title position on the display
        if (test_log == 0){u8g2.print("Battery Log");}
        else if(test_log == 1){u8g2.print("Button Log");}
        u8g2.setFont(u8g2_font_tom_thumb_4x6_mf); // set the font for the terminal window
        u8g2.drawLog(2, 24, u8g2log);             // draw the terminal window on the display
    } while (u8g2.nextPage());
}

void printPmuInfo()
{ //需要打印在屏幕上的芯片信息

    /* Get PMU temp info */
    lprintf(LOG_INFO, "CoreTemp :%.2f 'C", pmu.getAXP173Temp()); //芯片温度

    /* Get VBUS info */
    // lprintf(LOG_INFO,"VBUS_voltage :%.2f V\n", pmu.getVBUSVoltage());         //VBUS输入电压
    // lprintf(LOG_INFO,"VBUS_current :%.2f mA\n", pmu.getVBUSCurrent());        //VBUS输入电流

    if (pmu.isBatExist())
    {
        // lprintf(LOG_INFO,"Battery :Battery Exist");                         //电池接入状态
        /* Get Battery info */
        pmu.isCharging() ? lprintf(LOG_INFO, "Charging :Is Charging s") : lprintf(LOG_INFO, "Charging :NO or END Charging"); //充电状态
        lprintf(LOG_INFO, "Bat_voltage :%.2f V", pmu.getBatVoltage());                                                       //电池电压
        lprintf(LOG_INFO, "Bat_Current :%.2f mA", pmu.getBatCurrent());                                                      //电池电流  正为充电，负为放电

        lprintf(LOG_INFO, "Bat_Level :%.2f %%", pmu.getBatLevel()); //电池电量百分比显示
        // lprintf(LOG_INFO,"Bat_BatPower :%.2f W", pmu.getBatPower());            //电池瞬时功率

        // lprintf(LOG_INFO,"GetBatCoulombInput :%.2f C", pmu.GetBatCoulombInput());   //Get Coulomb charge Data
        lprintf(LOG_INFO, "GetBatCoulombOutput :%.2f C", pmu.GetBatCoulombOutput()); // Get Coulomb Discharge Data
        lprintf(LOG_INFO, "CoulometerData :%.2f C", pmu.getCoulometerData());        // get coulomb val affter calculation
    }
    else
    {
        lprintf(LOG_INFO, "Battery :NO Battery"); //没电池就输出这
    }
}

void menuUp()
{
    click2();
    func_index = table[func_index].up; // 向上翻
}

void menuDown()
{
    click3();
    func_index = table[func_index].down; // 向下翻
}

void menuEnter()
{
    click1();
    func_index = table[func_index].enter; // 确认
}

void menuBack(){
    doubleclick2();
    func_index = table[func_index].back; // 返回
}

void buttonFunctionInit()
{

    /* use for menu */
    buttonPress.attachClick(menuEnter);
    buttonUp.attachClick(menuUp);
    buttonUp.attachDoubleClick(menuBack);
    buttonDown.attachClick(menuDown);
    /* use for test */
    // buttonPress.attachClick(click1);
    // buttonUp.attachClick(click2);
    // buttonUp.attachDoubleClick(doubleclick2);
    // buttonDown.attachClick(click3);

    // link the buttonPress functions.
    buttonPress.attachDoubleClick(doubleclick1);
    buttonPress.attachLongPressStart(longPressStart1);
    buttonPress.attachLongPressStop(longPressStop1);
    buttonPress.attachDuringLongPress(longPress1);

    // link the buttonUp functions.
    buttonUp.attachLongPressStart(longPressStart2);
    buttonUp.attachLongPressStop(longPressStop2);
    buttonUp.attachDuringLongPress(longPress2);

    // link the buttonDown functions.
    buttonDown.attachDoubleClick(doubleclick3);
    buttonDown.attachLongPressStart(longPressStart3);
    buttonDown.attachLongPressStop(longPressStop3);
    buttonDown.attachDuringLongPress(longPress3);
}

void buttonClickDet(){
    if (digitalRead(D5) == 0)
    {
        u8g2.setPowerSave(0);
        LOCK_STATE = 1;
    }
    else if (digitalRead(D3) == 0)
    {
        u8g2.setPowerSave(0);
        LOCK_STATE = 1;
    }
    else if (digitalRead(D4) == 0)
    {
        u8g2.setPowerSave(0);
        LOCK_STATE = 1;
    }
}

void PmuInfoTest(){
    test_log = 0;
    printPmuInfo();  //测试 1:打印你想要的电源信息
    screenPrint();
}

void buttonClickTest(){
    test_log = 1;
    buttonHandle();
    buttonClickDet();

    KeyPressIRQEvent(); //测试 2:AXP173长短按键事件
    screenPrint();
}

void buttonHandle(){
    buttonPress.tick();
    buttonUp.tick();
    buttonDown.tick();
}



/* buttonPress callback functions */

// This function will be called when the buttonPress was pressed 1 time (and no 2. button press followed).
void click1()
{
    lprintf(LOG_INFO, ">ButtonPress");
    lprintf(LOG_INFO, " -Click-\n");
} // click1

// This function will be called when the buttonPress was pressed 2 times in a short timeframe.
void doubleclick1()
{
    lprintf(LOG_INFO, ">ButtonPress");
    lprintf(LOG_INFO, " -DoubleClick-\n");
} // doubleclick1

// This function will be called once, when the buttonPress is pressed for a long time.
void longPressStart1()
{
    lprintf(LOG_INFO, ">ButtonPress");
    lprintf(LOG_INFO, " -ButtonPress LongPress Start-");
} // longPressStart1

// This function will be called often, while the buttonPress is pressed for a long time.
void longPress1()
{
    lprintf(LOG_INFO, " |         LongPress         |");
} // longPress1

// This function will be called once, when the buttonPress is released after beeing pressed for a long time.
void longPressStop1()
{
    lprintf(LOG_INFO, " -ButtonPress LongPress  Stop-\n");
} // longPressStop1

// ... and the same for buttonUp:
void click2()
{
    lprintf(LOG_INFO, ">ButtonUp");
    lprintf(LOG_INFO, " -Click-\n");
} // click2

void doubleclick2()
{
    lprintf(LOG_INFO, ">ButtonUp");
    lprintf(LOG_INFO, " -DoubleClick-\n");
} // doubleclick2

void longPressStart2()
{
    lprintf(LOG_INFO, ">ButtonUp");
    lprintf(LOG_INFO, " -ButtonUp LongPress Start-");
} // longPressStart2

void longPress2()
{
    lprintf(LOG_INFO, " |        LongPress       |");
} // longPress2

void longPressStop2()
{
    lprintf(LOG_INFO, " -ButtonUp LongPress  Stop-\n");
} // longPressStop2

// ... and the same for buttonDown:
void click3()
{
    lprintf(LOG_INFO, ">ButtonDown");
    lprintf(LOG_INFO, " -Click-\n");
} // click3

void doubleclick3()
{
    lprintf(LOG_INFO, ">ButtonDown");
    lprintf(LOG_INFO, " -DoubleClick-\n");
} // doubleclick3

void longPressStart3()
{
    lprintf(LOG_INFO, ">ButtonDown");
    lprintf(LOG_INFO, " -ButtonDown LongPress Start-");
} // longPressStart3

void longPress3()
{
    lprintf(LOG_INFO, " |         LongPress        |");
} // longPress3

void longPressStop3()
{
    lprintf(LOG_INFO, " -ButtonDown LongPress  Stop-\n");
} // longPressStop3
// End


/*******************界面设置****************************/
void fun1()//菜单1-1
{
    u8g2.clearBuffer();
    page_1_to_2();
    // 显示光标
    u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
    u8g2.drawGlyph(5, 16 * (2 % 4 == 0 ? 4 : 2 % 4) - 4, 118);
    u8g2.sendBuffer();
}
void fun2()//菜单1-2
{
    u8g2.clearBuffer();
    page_1_to_2();
    // 显示光标
    u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
    u8g2.drawGlyph(5, 16 * (3 % 4 == 0 ? 4 : 3 % 4) - 4, 118);
    u8g2.sendBuffer();
}
void page_1_to_2()
{
    u8g2.setFont(u8g2_font_wqy14_t_gb2312a);
    u8g2.setCursor(20, 16 * 1 - 2);
    u8g2.print("  _Test Menu_");

    u8g2.setCursor(20, 16 * 2 - 2);
    u8g2.print(" Battery Log ");
    u8g2.setCursor(20, 16 * 3 - 2);
    u8g2.print(" Button Log ");
}

void fun3()//菜单1-1-1
{
    PmuInfoTest();
}
void fun4()//菜单1-2-1
{
    buttonClickTest();
}