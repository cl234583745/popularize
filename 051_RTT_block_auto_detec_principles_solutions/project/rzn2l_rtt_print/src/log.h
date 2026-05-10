/*
 * log.h
 *
 *  Created on: 2026年2月12日
 *      Author: Jerry.Chen
 */

#ifndef COMMON_LOG_H_
#define COMMON_LOG_H_

#ifndef LOG_H
#define LOG_H

#include <stdio.h>

/*
 * 打印方式选择: 0=UART, 1=RTT
 * 只能选择一种方式
 */
#define PRINTF_METHOD  1

#if (PRINTF_METHOD == 0)
    #define USE_UART_PRINT      1
    #define USE_RTT_PRINT       0
#elif (PRINTF_METHOD == 1)
    #define USE_UART_PRINT      0
    #define USE_RTT_PRINT       1
#endif


#if USE_RTT_PRINT
#define SEGGER_INDEX (0)
#include "SEGGER_RTT/SEGGER_RTT.h"
    #define printf(...) SEGGER_RTT_printf(SEGGER_INDEX, __VA_ARGS__)
#endif

// ---------- 日志级别定义 ----------
#define LOG_LEVEL_NONE  0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_INFO  3
#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_TRACE 5

// ---------- 全局总开关（默认 INFO） ----------
#ifndef GLOBAL_LOG_LEVEL
#define GLOBAL_LOG_LEVEL LOG_LEVEL_INFO
#endif

// ---------- 辅助宏：判断全局级别是否 ≥ 指定级别 ----------
#define GLOBAL_GE(level) (GLOBAL_LOG_LEVEL >= (level))

// ---------- 当前文件的有效级别（若无自定义，则等于全局级别） ----------
#ifdef CURRENT_LOG_LEVEL
    #define LOCAL_GE(level) (CURRENT_LOG_LEVEL >= (level))
#else
    #define LOCAL_GE(level) (GLOBAL_LOG_LEVEL >= (level))
#endif

// ---------- 各个日志级别的宏定义（编译时决定） ----------
// 只有同时满足【全局允许】且【当前文件允许】才输出

#if GLOBAL_GE(LOG_LEVEL_ERROR) && LOCAL_GE(LOG_LEVEL_ERROR)
    #define LOG_ERROR(...) printf("[ERROR] " __VA_ARGS__ )
#else
    #define LOG_ERROR(...) ((void)0)
#endif

#if GLOBAL_GE(LOG_LEVEL_WARN) && LOCAL_GE(LOG_LEVEL_WARN)
    #define LOG_WARN(...)  printf("[WARN] "  __VA_ARGS__ )
#else
    #define LOG_WARN(...)  ((void)0)
#endif

#if GLOBAL_GE(LOG_LEVEL_INFO) && LOCAL_GE(LOG_LEVEL_INFO)
    #define LOG_INFO(...)  printf("[INFO] "  __VA_ARGS__ )
#else
    #define LOG_INFO(...)  ((void)0)
#endif

#if GLOBAL_GE(LOG_LEVEL_DEBUG) && LOCAL_GE(LOG_LEVEL_DEBUG)
    #define LOG_DEBUG(...) printf("[DEBUG] " __VA_ARGS__ )
#else
    #define LOG_DEBUG(...) ((void)0)
#endif

#if GLOBAL_GE(LOG_LEVEL_TRACE) && LOCAL_GE(LOG_LEVEL_TRACE)
    #define LOG_TRACE(...) printf("[TRACE] " __VA_ARGS__ )
#else
    #define LOG_TRACE(...) ((void)0)
#endif

#endif // LOG_H

#endif /* COMMON_LOG_H_ */
