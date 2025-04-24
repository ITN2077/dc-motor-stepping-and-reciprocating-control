/**
 * @brief    时间片错位轮询
 * @file     XxxTimeSliceOffset.c
 * @author   何锡斌
 * @email    2537274979@qq.com
 * @date     2024/01/26
 * @version  V1_0_0
 * @par      实现功能：
 * - 基于外部提供的tick(systick中断或定时器中断)，根据注册生成多种时间片(支持0*tick)轮询调用任务，优化裸机程序架构；
 * @par      注意事项：
 * - 裸机开发的通病，每个任务会相互影响实时性，所以尽可能设计让任务占用cpu时间越短(特别是频繁执行的)，建议使用状态机思想编程；
 * @par      修改日志：
 * <table>
 * <tr><th>日期          <th>版本        <th>作者    <th>更新内容        </tr>
 * <tr><td>2024/01/26    <td>V1_0_0      <td>何锡斌  <td>初版发布；      </tr>
 * </table>
 */
#include "XxxTimeSliceOffset.h"

#ifndef NULL
#define NULL (void *)0
#endif

static STR_XxxTimeSliceOffset *pTimeSliceList = NULL; /**< 时间片链表入口(仅入口，最终直接指向设备实体，所需无需申请空间。链表是单向线性链表) */

/**
 * @brief        注册
 * @param[in]    pTSlice         时间片对象指针
 * @param[in]    taskFunc        任务函数的函数指针
 * @param[in]    reloadVal       时间片重载值*tick基准即为任务执行间隔
 * @param[in]    offset          偏移量，这是错位的精髓
 * @return       配置是否成功
 * - 0   注册成功
 * - 1   配置完成，但对象已存在，无需加入链表
 * - -1  pTSlice为空指针，无效对象
 * @par          注意事项：
 * - reloadVal设置为零即非定时任务，则offset偏移量无效
 * @par          示例:
 * @code
 *
 * XxxTimeSliceOffset_Register(&m_timeSlice_1, Task_1, 0, 0);        //0，即非定时任务(每次轮询都会执行)
 * XxxTimeSliceOffset_Register(&m_timeSlice_2, Task_2, 10, 0);       //10*1ms，即10ms运行一次
 * XxxTimeSliceOffset_Register(&m_timeSlice_3, Task_3, 10, 5);       //10*1ms，即10ms运行一次，与Task_2错开5ms，这样就不会集中到同一个10ms的时间点上
 *
 * @endcode
 */
int XxxTimeSliceOffset_Register(STR_XxxTimeSliceOffset *pTSlice,
                                void (*taskFunc)(void),
                                unsigned short reloadVal,
                                unsigned short offset)
{
    if (NULL == pTSlice)
        return -1; /* 返回错误：无效对象 */

    pTSlice->reloadVal = reloadVal;
    pTSlice->count = reloadVal + offset; /* 添加偏移量，使得同一数值的时间片错开 */
    pTSlice->taskFunc = taskFunc;
    if (0 == reloadVal) /* 非定时任务 */
    {
        pTSlice->runFlag = 1; /* 非定时任务可运行标志默认为一 */
    }
    else /* 定时任务 */
    {
        pTSlice->runFlag = 0; /* 定时任务可运行标志默认为零 */
    }

    /*遍历链表，防止添加重复*/
    for (STR_XxxTimeSliceOffset *pTemp = pTimeSliceList; pTemp != NULL; pTemp = pTemp->pNext)
    {
        if (pTemp == pTSlice)
        {
            return 1; /* 返回成功：配置完成，但对象已存在，无需加入链表 */
        }
    }
    /*加入链表*/
    pTSlice->pNext = pTimeSliceList;
    pTimeSliceList = pTSlice; /* 把对象加入到链表头部 */

    return 0; /* 返回成功：注册成功 */
}

/**
 * @brief        启动时间片错位轮询(代替main的while循环)
 * @param        null
 * @return       null
 * @par          注意事项：
 * - null
 */
void XxxTimeSliceOffset_Start(void)
{
    while (1) /* 代替main的while循环 */
    {
        /*遍历时间片链表*/
        for (STR_XxxTimeSliceOffset *pTemp = pTimeSliceList; pTemp != NULL; pTemp = pTemp->pNext)
        {
            if (pTemp->runFlag) /* 可运行则调用任务函数 */
            {
                if (pTemp->reloadVal) /* 重载值不为0，即定时任务 */
                {
                    pTemp->runFlag = 0; /* 可运行标志清零，开启新一轮倒计时 */
                }
                pTemp->taskFunc();
            }
        }
    }
}

/**
 * @brief        时间片生成(放到systick或定时器中断处理函数内)
 * @param        null
 * @return       null
 * @par          注意事项：
 * - null
 */
void XxxTimeSliceOffset_Produce(void)
{
    /*遍历时间片链表*/
    for (STR_XxxTimeSliceOffset *pTemp = pTimeSliceList; pTemp != NULL; pTemp = pTemp->pNext)
    {
        if (pTemp->reloadVal) /* 重载值不为0，即定时任务 */
        {
            --pTemp->count;        /* 计数器递减 */
            if (0 == pTemp->count) /* 计数器递减到零 */
            {
                pTemp->runFlag = 1;              /* 允许执行 */
                pTemp->count = pTemp->reloadVal; /* 计数器重载 */
            }
        }
    }
}
