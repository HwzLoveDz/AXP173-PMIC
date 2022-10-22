/**
 * @file log.c
 * @author mondraker (691806052@qq.com)
 * @brief 只需要将#define OPEN_LOG 1注释即可取消测试日志打印
 *        只需要将#define LOG_SAVE 1注释即可取消文件日志存储
 *        只需要将#define ARDUINO_LOG 1注释即可取消所有ARDUINO串口打印
 *        只需要将#define U8G2_LOG 1注释即可取消所有U8g2屏幕打印
 *        #define LOG_LEVEL LOG_DEBUG表示打印大于等于LOG_DEBUG等级的日志
 * 
 *        依赖函数形式：int vsnprintf(char * __buf, size_t __len, const char * __format, __VALIST __local_argv)
 *        日志函数形式：logPrintf(level,__FUNCTION__,__LINE__,__DATE__,__TIME__,fmt)
 * 
 * @usage 日志函数调用形式：lprintf(level,fmt...)
 * 
 * @function 可控的日志输入输出，并且添加到日志文件中。支持打印输出类型，输出时间，输出日期，输出行号，输出函数位置
 * @version 0.1
 * @date 2022-09-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _MD_LOG_H_
#define _MD_LOG_H_

// #include <stdio.h>
// #include <stdarg.h>  //引入va_list等
#include <Arduino.h>    //在arduino中使用
#include <U8g2lib.h>    //屏幕打印日志使用

/* 用的时候可以直接全部注释，移到主代码里，方便调试 */
//#define ARDUINO_LOG 1 //注释即可取消所有ARDUINO串口打印
#define U8G2_LOG 1    //注释即可取消所有U8g2屏幕打印
//#define OPEN_LOG 1    //注释即可取消所有终端测试日志打印
//#define LOG_SAVE 1    //注释即可取消所有日志存储到文件中
#define LOG_LEVEL LOG_DEBUG //仅打印大于等于该等级的日志

enum LOG_MODE{      //枚举所需的输出日志类型
    LOG_DEBUG = 0,  //0
    LOG_INFO,       //1
    LOG_WARING,     //2
    LOG_ERROR,      //3
};

char *mdCheckLevel(const int level);    //返回对应的日志等级字符串

void logPrintf(const int level,const char * fun,const int line,const char * date,const char * time,const char *fmt,...);

#define lprintf(level,fmt...) logPrintf(level,__FUNCTION__,__LINE__,__DATE__,__TIME__,fmt)
/* __FUNCTION__,__LINE__为系统传入，不需要出现在用户输入中，可以用#define替换掉形参和臃肿的函数名称
 * __FUNCTION__:打印代码输出的函数位置
 * __LINE__:打印代码输出的行号
 * __TIME__:打印代码编译的时间
 * __DATE__:打印代码编译的日期
 */

extern U8G2LOG u8g2log;
#endif