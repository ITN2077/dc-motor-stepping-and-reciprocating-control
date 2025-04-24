/**
 * @brief    时间片错位轮询
 * @file     XxxTimeSliceOffset.h
 * @author   何锡斌
 * @email    2537274979@qq.com
 * @date     2024/01/26
 * @version  V1_0_0
 * @par      实现功能：
 * - 基于外部提供的tick(systick中断或定时器中断)，根据注册生成多种时间片(支持0*tick)轮询调用任务，优化裸机程序架构；
 * @par      注意事项：
 * - 裸机开发的通病，每个任务会相互影响实时性，所以尽可能设计让任务占用cpu时间越短(特别是频繁执行的)，可以使用状态机思想编程；
 * @par      修改日志：
 * <table>
 * <tr><th>日期          <th>版本        <th>作者    <th>更新内容        </tr>
 * <tr><td>2024/01/26    <td>V1_0_0      <td>何锡斌  <td>初版发布；      </tr>
 * </table>
 */
#ifndef _XXXTIMESLICEOFFSET_H_
#define _XXXTIMESLICEOFFSET_H_

/**时间片类*/
typedef struct _STR_XxxTimeSliceOffset
{
    volatile unsigned char runFlag;        /**< 可运行标志(1:可运行/0:不可运行) */
    volatile unsigned short count;         /**< 计数器 */
    unsigned short reloadVal;              /**< 重载值 */
    void (*taskFunc)(void);                /**< 任务函数的函数指针 */
    struct _STR_XxxTimeSliceOffset *pNext; /**< 指向下一个对象 */
} STR_XxxTimeSliceOffset;

/********************************************函数声明********************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*注册*/
    int XxxTimeSliceOffset_Register(STR_XxxTimeSliceOffset *pTSlice,
                                    void (*taskFunc)(void),
                                    unsigned short reloadVal,
                                    unsigned short offset);
    /*启动时间片错位轮询(代替main的while循环)*/
    void XxxTimeSliceOffset_Start(void);
    /*时间片生成(放到systick或定时器中断处理函数内)*/
    void XxxTimeSliceOffset_Produce(void);

#ifdef __cplusplus
}
#endif
#endif
