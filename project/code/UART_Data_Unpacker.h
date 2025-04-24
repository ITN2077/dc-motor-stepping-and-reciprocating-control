/*********************************************************************************************************************
 * 本模块借助zf_driver_uart模块实现，依赖的串口(uart_DEBUG)在zf_common_debug.h中配置
 * 简介：接收以~}开头以}~结尾的数据包，数据包前TAG_LENGTH位用于标签匹配，其余的作为数据接收
 * 实现：
 *    中断中将读取的字节直接存入环形缓冲区，在环形缓冲区中按照固定格式接收数据包，存入 UART_DEBUG_got_data[PACKET_MAX_SIZE] 中，
 *   主循环调用 UART_DEBUG_Ringbuffer_Processer(); 对环形缓冲区中的数据处理
 *   调用 PacketTag_Analysis(); 对 PacketTag_TpDef_struct 结构体数组中的标签匹配，然后对挂载变量赋值。
 * 用法：
 * *.载荷大小通过 #define PACKET_MAX_SIZE 修改
 * *.标签长度通过 TAG_LENGTH 修改
 * *.环形缓冲区大小通过 RINGBUFFER_SIZE 修改，只能是2的幂次方
 * *.PacketTag_TpDef_struct 创建结构体数组，存储标签、挂载函数以及挂载变量
 * *.数据经过环形缓冲区缓存，最终存储到 UART_DEBUG_got_data[] 中，使用时直接用
 * *.UART_DEBUG_data_packet_ready 这是接收数据完毕标志位，完成置1
 * *.提供了多种数据解析格式
 * 注意：
 * *.使用时需要对UART_DEBUG初始化、配置中断
 * *.使用时要 ring_buffer_init(&ringbuffer_UART_DEBUG, ringbuffer_place_UART_DEBUG, RINGBUFFER_SIZE); 初始化环形缓冲区结构体
 * *.中断函数的使用关闭了zf_common_debug.h 文件中的 #define DEBUG_UART_USE_INTERRUPT    (0)，如要使用debug uart的逐飞的中断函数，请手动打开
 * 例子：main.c
#include "zf_common_headfile.h"

int test_value_1 = 0;
float test_value_2 = 0.000;

PacketTag_TpDef_struct Test_packet[] = {
    {"1", UnpackData_Handle_Int_FireWater, &test_value_1},
    {"2", UnpackData_Handle_Float_FireWater, &test_value_2},
    // 添加更多的映射关系
};

int main(void)
{
    clock_init(SYSTEM_CLOCK_120M); // 初始化芯片时钟 工作频率为 120MHz
    debug_init();                  // 务必保留，本函数用于初始化MPU 时钟 调试串口
    uart_init(DEBUG_UART_INDEX, DEBUG_UART_BAUDRATE, DEBUG_UART_TX_PIN, DEBUG_UART_RX_PIN); // 串口初始化
    uart_rx_interrupt(DEBUG_UART_INDEX, ENABLE);                                            // 开启接收中断
    interrupt_set_priority(UART8_IRQn, (0 << 5) || 1);                                     // 设置usart3的中断优先级
    ring_buffer_init(&ringbuffer_UART_DEBUG, ringbuffer_place_UART_DEBUG, RINGBUFFER_SIZE); // 环形缓冲区初始化 V1.1添加
    printf_USART_DEBUG("hello,WSY!");
    while (1)
    {
        // 此处编写需要循环执行的代码
        UART_DEBUG_Ringbuffer_Processer();
        if (UART_DEBUG_data_packet_ready)
        {
            UART_DEBUG_data_packet_ready = false;
            DebugPrint(); // 输出接收的数据
            PacketTag_Analysis(Test_packet, 2);
            printf_USART_DEBUG("\r\ntestv1:%f\r\n", test_value_1);
            printf_USART_DEBUG("\r\ntestv2:%d\r\n", test_value_2);
        }
        // 此处编写需要循环执行的代码
    }
}
 * 修改记录
 *
 * 日期            作者                             备注
 * 2024-03-24     Sxxx
 * 2024-03-31     Sxxx      更新V1.1，添加环形缓冲区转存数据，减少串口中断开销
 * 2024-04-14     Sxxx      更新V1.2，修正依赖串口的逻辑，使其依赖所有串口。在DEBUG_UART配置。
 * 2024-04-21     Sxxx      更新V1.3，添加DebugPrint函数便于调试，优化注释
 ********************************************************************************************************************/
#ifndef UART_DATA_UNPACKER_H
#define UART_DATA_UNPACKER_H

#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include <stdbool.h>
#include "zf_driver_uart.h"
#include "Ring_Buffer.h"


#define PACKET_MAX_SIZE 10 // 载荷大小,自定义修改
#define TAG_LENGTH 1       // 标签长度，自定义修改
#define RINGBUFFER_SIZE 64 // 环形缓冲区大小，必须是2的幂

// 环形缓冲区变量声明，不可修改，私有
extern ring_buffer_t ringbuffer_UART_DEBUG;
extern uint8_t ringbuffer_place_UART_DEBUG[RINGBUFFER_SIZE];

extern uint8_t UART_DEBUG_got_data[PACKET_MAX_SIZE]; // 串口3接收的数据，公有
extern volatile bool UART_DEBUG_data_packet_ready;   // 串口3接收完数据包的标志位，数据包接收完毕置1，公有

// 私有元素START（结构体中函数挂载调用，私有）
// 定义一个函数指针类型，间接调用解包函数
typedef void (*Function_Unpack_Handler)(const char *, void *);
// 按照文本格式提取出float数据,包长度为 标签长度+3+小数长度+整数长度
void UnpackData_Handle_Float_FireWater(const char *packet, void *value);
// 按照文本格式提取出int数据，建议包长度为 标签长度+1+整数长度
void UnpackData_Handle_Int_FireWater(const char *packet, void *value);

void UnpackData_Handle_Int16_IEEE754(const uint8 *packet, void *int16_value);

// 私有结构体，封装标签和挂接函数以及存储数值
typedef struct
{
    const char tag[TAG_LENGTH + 1];
    Function_Unpack_Handler function_handler;
    void *value_ptr;
} PacketTag_TpDef_struct;


void UART_DEBUG_Init(void);
void PacketTag_Analysis(PacketTag_TpDef_struct Tag_packet[], uint8_t tag_count);
void printf_USART_DEBUG(char *format_str, ...);
void UART_DEBUG_Ringbuffer_Processer(void);
void DebugPrint(void);

#endif // UART_DATA_UNPACKER_H
