#include <stdio.h>

#define MACROS_TABLE                    \
    X_MACROS(CMD_LED_ON,  led_on)       \
    X_MACROS(CMD_LED_OFF, led_off)      \

/*定义命令列表*/
typedef enum
{
    #define X_MACROS(a, b) a,
    MACROS_TABLE
    #undef X_MACROS
    CMD_MAX
}cmd_e;


/*定义字符串列表用作Log打印*/
const char* cmd_str[] = 
{
    #define X_MACROS(a, b) #a,
    MACROS_TABLE
    #undef X_MACROS
};


typedef void (*func)(void* p);

static void led_on(void* p)
{
    printf("%s \r\n", (char *)p);
}

static void led_off(void* p)
{
    printf("%s \r\n", (char *)p);
}

/*定义函数列表*/
const func func_table[] = 
{
    #define X_MACROS(a, b) b,
    MACROS_TABLE
    #undef X_MACROS
};


/*直接通过索引的方式调用函数*/
static void cmd_handle(cmd_e cmd)
{
    if(cmd < CMD_MAX)
    {
        func_table[cmd]((void*)cmd_str[cmd]);
    }
}


void main(void)
{
    cmd_handle(CMD_LED_ON);
    cmd_handle(CMD_LED_OFF);
}
