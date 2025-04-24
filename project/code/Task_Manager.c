#include "Task_Manager.h"
void motor1_step_instance_start(unsigned short duty, motor_direction_et dir)
{
    if (dir == MOTOR_DIR_FORWARD)
    {
        gpio_set_level(E2, GPIO_HIGH);
        gpio_set_level(E3, GPIO_HIGH);
        gpio_set_level(E4, GPIO_LOW);
    }
    else if (dir == MOTOR_DIR_BACKWARD)
    {
        gpio_set_level(E2, GPIO_HIGH);
        gpio_set_level(E3, GPIO_LOW);
        gpio_set_level(E4, GPIO_HIGH);
    }
}
void motor1_step_instance_stop(void)
{
    gpio_set_level(E2, GPIO_HIGH);
    gpio_set_level(E3, GPIO_LOW);
    gpio_set_level(E4, GPIO_LOW);
}
void motor_step_update_task(void)
{
    motor_step_update(10);
}

motor_step_instance_t *P_M1_instance = NULL; // 只声明指针，不在全局初始化

//!------------------🍅🍅🍅🍅🍅🍅 注册时间片轮询任务 START 🍒🍒🍒🍒🍒🍒---------⬇️⬇️⬇️⬇️⬇️⬇️
STR_XxxTimeSliceOffset Uart_task, Motor_task, While_task; // 创建任务句柄,While_task,Key_task,
/**
 *  @brief 时间片轮询任务创建函数
 *  @note 记得创建任务句柄
 */
void Time_Slice_Offset_Register(void)
{
    // !任务调度系统节拍 单位 10 ms 以下是注册任务
    XxxTimeSliceOffset_Register(&While_task, While_Task, 10, 0);              // 注册while循环任务。
    XxxTimeSliceOffset_Register(&Uart_task, UART_packet_TASKhandler, 0, 0);  // 注册串口数据包接收任务, 轮询时间为0即while，偏移0.
    XxxTimeSliceOffset_Register(&Motor_task, motor_step_update_task, 10, 0); // 注册电机步进任务, 轮询时间为1ms，偏移0.
    // XxxTimeSliceOffset_Register(&Key_task, key_Processing, 2, 1);           // 按键扫描函数,需要使用记得注册任务以及初始化 key_init(20);
    //  注册任务结束
}
//!------------------✨✨✨✨✨✨ 注册时间片轮询任务 END 🌸🌸🌸🌸🌸🌸---------⬆️⬆️⬆️⬆️⬆️⬆️

//!----------------🍅🍅🍅🍅🍅🍅 串口数据包任务使用的 START 🍒🍒🍒🍒🍒🍒---------⬇️⬇️⬇️⬇️⬇️⬇️
#define NumOfMsg 2 // 定义串口接收的数据要解析的数据的个数
int test_value_1 = 0;
float test_value_2 = 0.000;

PacketTag_TpDef_struct Test_packet[] = {
    {"1", UnpackData_Handle_Int_FireWater, &test_value_1},
    {"2", UnpackData_Handle_Float_FireWater, &test_value_2},
    // 添加更多的映射关系
};
//!------------------✨✨✨✨✨✨ 串口数据包任务使用的 END 🌸🌸🌸🌸🌸🌸---------⬆️⬆️⬆️⬆️⬆️⬆️

//!------------------🍅🍅🍅🍅🍅🍅 非时间片轮询任务调度函数 START  🍒🍒🍒🍒🍒🍒---------⬇️⬇️⬇️⬇️⬇️⬇️
/**********************************************************************
 *  @brief 全部外设的初始化
 *  @param
 *  @return
 *  @note
 *  @warning
 */
void PeripheraAll_Init()
{
    UART_DEBUG_Init();

    pit_ms_init(TIM6_PIT, 1);             // 定时器6初始化，提供软实时任务调度系统节拍
    interrupt_set_priority(TIM6_IRQn, 0); // 最高中断优先级

    // pit_ms_init(TIM7_PIT, 50);            // 定时器七初始化，用于硬实时任务调度
    // interrupt_set_priority(TIM7_IRQn, 0);

    // timer_init(TIM_5, TIMER_US); // 初始化定时器5用于计时

    gpio_init(E2, GPO, 0, GPIO_PIN_CONFIG); // 电机使能
    gpio_init(E3, GPO, 0, GPIO_PIN_CONFIG); // 电机正转
    gpio_init(E4, GPO, 0, GPIO_PIN_CONFIG); // 电机反转

    // 在此处初始化P_M1_instance
    P_M1_instance = motor_step_instance_create(motor1_step_instance_start, motor1_step_instance_stop);
    motor_step_instance_start(P_M1_instance, 100, MOTOR_DIR_FORWARD, 100, 30);

    // key_init(20); // 按键初始化，20ms一次中断
    printf_USART_DEBUG("hello,WSY!\r\n");
    printf_USART_DEBUG("hello,WSY! Let`s start!\r\n");
    // Task_Disable();  // 定时器中断失能。即所有实时任务停止
}
//!------------------✨✨✨✨✨✨ 非时间片轮询任务调度函数 END 🌸🌸🌸🌸🌸🌸---------⬆️⬆️⬆️⬆️⬆️⬆️

//!------------------🍅🍅🍅🍅🍅🍅 时间片轮询任务 START 🍒🍒🍒🍒🍒🍒---------⬇️⬇️⬇️⬇️⬇️⬇️
/**
 * @brief While 循环，无定时任务，全速运行。
 *
 */
void While_Task(void)
{
    // 写入while循环任务
    // 当test_value_1大于零时，电机正转一次
    uint16_t dirve_duration_ms = 100;
    uint16_t cooldown_duration_ms = 100;
    if (test_value_1 > 0)
    {
        motor_step_instance_start(P_M1_instance, 100, MOTOR_DIR_FORWARD, dirve_duration_ms, cooldown_duration_ms);
        printf_USART_DEBUG("forward\r\n");
    }
    // 当test_value_1小于零时，电机反转一次
    else if (test_value_1 < 0)
    {
        motor_step_instance_start(P_M1_instance, 100, MOTOR_DIR_BACKWARD, dirve_duration_ms, cooldown_duration_ms);
        printf_USART_DEBUG("backward\r\n");
    }
}
/**
 *  @brief 串口数据包处理任务函数
 *  @note  务必设置为无定时任务，全速运行防止缓冲区溢出
 */
void UART_packet_TASKhandler(void)
{
    UART_DEBUG_Ringbuffer_Processer();
    if (UART_DEBUG_data_packet_ready)
    {
        UART_DEBUG_data_packet_ready = false;
        PacketTag_Analysis(Test_packet, NumOfMsg); // 此处NumOfMsg在串口处定义。
        DebugPrint();                              // 输出接收的数据
        printf_USART_DEBUG("\r\ntestv1:%d\r\n", test_value_1);
        printf_USART_DEBUG("\r\ntestv2:%f\r\n", test_value_2);
    }
}
/**
 *  @brief 按键扫描、处理任务，默认20ms处理一次
 *  @note   按键引脚要修改key.h中的key.list，对应任务句柄Key_task
 */
void key_Processing(void)
{
    key_scanner();
    if (KEY_SHORT_PRESS == key_get_state(KEY_1)) // 按键1短按
    {
        // printf_USART_DEBUG("KEY_1 pressed.");
    }
    else if (KEY_LONG_PRESS == key_get_state(KEY_1)) // 按键1长按
    {
    }
    else if (KEY_SHORT_PRESS == key_get_state(KEY_2))
    {
    }
    else if (KEY_LONG_PRESS == key_get_state(KEY_2))
    {
    }
}
//!------------------✨✨✨✨✨✨ 时间片轮询任务调度 END 🌸🌸🌸🌸🌸🌸---------⬆️⬆️⬆️⬆️⬆️⬆️

//!------------------🍅🍅🍅🍅🍅🍅 硬实时任务 START 🍒🍒🍒🍒🍒🍒---------⬇️⬇️⬇️⬇️⬇️⬇️
/**
 *  @brief 硬实时任务处理，到点就触发，比软实时任务高一级
 *  @param
 *  @return
 *  @note   此函数在TIM7中断中被使用，将函数写入此函数中即可。
 *  @warning
 */
void Hard_Real_Time_Processing(void)
{
}
//!------------------✨✨✨✨✨✨ 硬实时任务 END 🌸🌸🌸🌸🌸🌸---------⬆️⬆️⬆️⬆️⬆️⬆️