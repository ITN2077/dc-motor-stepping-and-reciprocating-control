/*********************************************************************************************************************
 SWDIO PA13 ,SWCLK PA14
默认DEBUG串口是UART8  TX_E14，RX_E15
KEY_LIST  {A8 , D8 , B12 , B0 } 对应 zf_device_key.h 中 KEY_LIST

定时器6用于软实时任务节拍
定时器7用于硬实时任务节拍
 ********************************************************************************************************************/
#include "Task_Manager.h"

int main(void)
{
    clock_init(SYSTEM_CLOCK_120M); // 初始化芯片时钟 工作频率为 120MHz
    debug_init();                  // 务必保留，本函数用于初始化MPU 时钟 调试串口
    PeripheraAll_Init();           // 全部外设初始化
    Time_Slice_Offset_Register();  // !时间片轮询任务创建函数，调度节拍 10 ms

    XxxTimeSliceOffset_Start(); // 执行任务，包含while，替代了主循环while
}

