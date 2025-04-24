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
            PacketTag_Analysis(Test_packet, 2);
            DebugPrint(); // 输出接收的数据
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
#include "UART_Data_Unpacker.h"

// 公共变量定义
uint8_t UART_DEBUG_got_data[PACKET_MAX_SIZE] = {0}; // 串口接收的数据
volatile bool UART_DEBUG_data_packet_ready = false; // 串口接收完数据包的标志位，完成接收置1

// 环形缓冲区变量，私有
ring_buffer_t ringbuffer_UART_DEBUG;
uint8_t ringbuffer_place_UART_DEBUG[RINGBUFFER_SIZE] = {0};


/*
* @brief 串口初始化函数
*/
void UART_DEBUG_Init(void)
{
    uart_init(DEBUG_UART_INDEX, DEBUG_UART_BAUDRATE, DEBUG_UART_TX_PIN, DEBUG_UART_RX_PIN); // 串口初始化
    uart_rx_interrupt(DEBUG_UART_INDEX, ENABLE);                                            // 开启接收中断
    interrupt_set_priority(UART8_IRQn, (0 << 5) || 1);                                     // 设置usart3的中断优先级
    ring_buffer_init(&ringbuffer_UART_DEBUG, ringbuffer_place_UART_DEBUG, RINGBUFFER_SIZE); // 环形缓冲区初始化 V1.1添加
}

/**
 * @brief 对缓存包处理，包括标签匹配和数据使用。
 * @param Tag_packet[]： 结构体数组，用于创建查找表存放标签、链接函数与变量
 * @param tag_count： 查找表数量
 * @note  注意填入的标签数正确
 *
 */
void PacketTag_Analysis(PacketTag_TpDef_struct Tag_packet[], uint8_t tag_count)
{
    uint8_t i = 0;
    for (i = 0; i < tag_count; i++)
    {
        // 检查标签是否匹配
        if (strncmp(UART_DEBUG_got_data, Tag_packet[i].tag, TAG_LENGTH) == 0)
        {
            // 调用关联的处理函数
            Tag_packet[i].function_handler(UART_DEBUG_got_data, Tag_packet[i].value_ptr);
            return; // 找到匹配项后返回
        }
    }
    uint8_t error_tag[TAG_LENGTH + 1] = {0};
    strncpy(error_tag, UART_DEBUG_got_data, TAG_LENGTH);
    error_tag[TAG_LENGTH] = '\0';                             // 显式地设置空终止符
    printf_USART_DEBUG("The \"%s\" is not find.", error_tag); // 错误处理
    // 如果没有找到任何匹配，可以在这里处理错误
    // U1_printf("The \"%s\" is not find.", Tag_packet[i].tag);
}

/**
 *  @brief 使用fire water格式解析vofa+的数据
 *  @warning 请使用Float类型接收数据
 */
void UnpackData_Handle_Float_FireWater(const char *packet, void *value)
{
    // char numString[PACKET_MAX_SIZE];
    // strncpy(numString, packet + TAG_LENGTH, PACKET_MAX_SIZE - 1);
    *((float *)value) = atof(packet + TAG_LENGTH);
}
/**
 *  @brief 使用fire water格式解析vofa+的数据
 *  @warning 请使用Int类型接收数据
 */
void UnpackData_Handle_Int_FireWater(const char *packet, void *value)
{
    // char numString[PACKET_MAX_SIZE];
    // strncpy(numString, packet + TAG_LENGTH, PACKET_MAX_SIZE - 1);
    *((int *)value) = atoi(packet + TAG_LENGTH);
}
/**
 *  @brief 按ieee754格式提取出int16数据，紧跟着标签的是高八位，下一位是低八位。
 *  @param  packet DEBUG串口数据包
 *  @param  int16_value int16_t数据类型用于存储数据
 *  @note  数据限幅-32100~32100，建议包长度为 标签长度+3
 *  @warning 务必使用int16_t数据类型挂载此函数
 */
void UnpackData_Handle_Int16_IEEE754(const uint8 *packet, void *int16_value)
{
    uint8_t highByte = *(packet + TAG_LENGTH);    // 获取高字节
    uint8_t lowByte = *(packet + TAG_LENGTH + 1); // 获取低字节
    *(int16_t *)int16_value = (int16_t)((highByte << 8) | lowByte);
}

/**
 *  @brief 使用DEBUG_UART输出的printf
 *  @param 格式化字符串
 *  @return void
 *  @warning
 *  @note 例子： printf_USART_DEBUG("text:%d", 1212); 串口接收到 text:1212
 */
void printf_USART_DEBUG(char *format_str, ...)
{
    uint8_t buffer[64];
    va_list arg;
    va_start(arg, format_str);
    vsprintf(buffer, format_str, arg);
    va_end(arg);
    uart_write_string(DEBUG_UART_INDEX, buffer);
}

/**
 *  @brief 输出本次接收的信息，以及对应的HEX码,用于抓虫时观察
 */
void DebugPrint(void)
{
    printf_USART_DEBUG("\r\nReturns:%s\r\n", UART_DEBUG_got_data);
    for (int i = 0; i < PACKET_MAX_SIZE; i++)
    {
        printf_USART_DEBUG("0x%02X ", UART_DEBUG_got_data[i]);
    }
    printf_USART_DEBUG("\r\n");
}

/**
 *  @brief UART_DEBUG中断处理函数，接收一个数据包后存储到环形缓冲区
 *  @note 两种数据包接收方式。中断中接收、借助环形缓冲区。
 *  @warning 在isr.h中声明，在isr.c对应串口号引用
 */
void USART_DEBUG_IRQ_Function(void)
{
    uint8_t byte = 0;
    if (uart_query_byte(DEBUG_UART_INDEX, &byte))
    {
        ring_buffer_queue(&ringbuffer_UART_DEBUG, byte); // 将字节放入环形缓冲区
                                                         // 现在数据在缓冲区中，可以在主循环或其他地方处理
    }
    /*------------------------------以下是中断中接收数据包------------------------------*/
    //
    // static uint8_t UART_DEBUG_got_data_index = 0;
    // static uint8_t prev_byte = 0;
    // static bool start_load_packet_flag = false;
    // uint8_t byte = 0;
    // if (uart_query_byte(DEBUG_UART_INDEX, &byte))
    // {
    //     if (!start_load_packet_flag)
    //     {
    //         if (prev_byte == '~' && byte == '}')
    //         {
    //             start_load_packet_flag = true;
    //             UART_DEBUG_got_data_index = 0;
    //         }
    //     }
    //     else
    //     {
    //         if (prev_byte == '}' && byte == '~' && start_load_packet_flag)
    //         {
    //             UART_DEBUG_got_data[UART_DEBUG_got_data_index - 1] = '\0';
    //             UART_DEBUG_data_packet_ready = true;
    //             start_load_packet_flag = false;
    //         }
    //         else if (UART_DEBUG_got_data_index < PACKET_MAX_SIZE)
    //         {
    //             UART_DEBUG_got_data[UART_DEBUG_got_data_index++] = byte;
    //         }
    //         else
    //         {
    //             memset(UART_DEBUG_got_data, 0, PACKET_MAX_SIZE);
    //             printf_USART_DEBUG("Error: Data packet overflow.\r\n");
    //             start_load_packet_flag = false;
    //         }
    //     }
    //     prev_byte = byte;
    // }
}

/**
 *  @brief 对UART3环形缓冲区中的变量处理
 *  @note 接收到一个完整数据包以后（UART_DEBUG_data_packet_ready 置1）会退出，直到 UART_DEBUG_data_packet_ready 置0
 *  @warning UART_DEBUG_data_packet_ready 使用完以后一定要软件置0
 */
void UART_DEBUG_Ringbuffer_Processer(void)
{
    static uint8_t UART_DEBUG_got_data_index = 0;
    static uint8_t prev_byte = 0;
    static bool start_load_packet_flag = false;
    uint8_t data = 0;

    while (!ring_buffer_is_empty(&ringbuffer_UART_DEBUG) && !UART_DEBUG_data_packet_ready)
    {
        ring_buffer_dequeue(&ringbuffer_UART_DEBUG, &data); // 从缓冲区中取出一个字节

        if (!start_load_packet_flag)
        {
            if (prev_byte == '~' && data == '}')
            {
                start_load_packet_flag = true;
                UART_DEBUG_got_data_index = 0;
            }
        }
        else
        {
            if (prev_byte == '}' && data == '~' && start_load_packet_flag)
            {
                UART_DEBUG_got_data[UART_DEBUG_got_data_index - 1] = '\0'; // 一定要确保UART_DEBUG_got_data已经声明且大小足够
                UART_DEBUG_data_packet_ready = 1;
                start_load_packet_flag = false;
                break;
            }
            else if (UART_DEBUG_got_data_index < PACKET_MAX_SIZE)
            {
                UART_DEBUG_got_data[UART_DEBUG_got_data_index++] = data;
            }
            else
            {
                memset(UART_DEBUG_got_data, 0, PACKET_MAX_SIZE);
                printf_USART_DEBUG("Error: Data packet overflow.\r\n");
                for (int i = 0; i < PACKET_MAX_SIZE; i++)
                {
                    printf_USART_DEBUG("0x%02X ", UART_DEBUG_got_data[i]);
                }
                start_load_packet_flag = false;
            }
        }
        prev_byte = data;
    }
}
