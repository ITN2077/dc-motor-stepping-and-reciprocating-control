#include <stdint.h>

/*********************************************************************************************************************
/**
 * @file    motor_step_controller.h
 * @brief   步进控制模块（适用于无编码器直流有刷电机）
 *
 * @details 本模块通过状态机控制电机"运行-冷却"周期，适用于模拟步进推进等需求。
 *          支持多实例管理，控制逻辑与底层GPIO解耦，用户需提供控制函数。
 *
 * 使用示例：
 *  -------------------------------------------------------------------
 *   1. 完成驱动初始化，比如这里是初始化GPIO（电机使能、正转、反转）
 *  gpio_init(E2, GPO, 0, GPIO_PIN_CONFIG);
 *  gpio_init(E3, GPO, 0, GPIO_PIN_CONFIG);
 *  gpio_init(E4, GPO, 0, GPIO_PIN_CONFIG);
 *
 *   2. 实现PWM控制函数、方向控制函数、刹车函数
 *  void motor1_step_instance_set_pwm(uint16_t pwm)
 *  void motor1_step_instance_set_dir(motor_direction_et dir)
 *  void motor1_step_instance_brake(void)
 *
 *   3. 创建实例，同时完成初始化、挂载控制函数
 *  motor_step_instance_t P_M1_instance = {
 *      .state = MOTOR_STEP_STATE_IDLE,
 *      .direction = MOTOR_DIR_FORWARD,
 *      .set_pwm = motor1_step_instance_set_pwm,
 *      .set_dir = motor1_step_instance_set_dir,
 *      .brake = motor1_step_instance_brake,
 *      .name = "M1"
 *  };
 *
 *   4. 启动电机
 *  motor_step_instance_start(m1, 100, MOTOR_DIR_FORWARD, 100, 30); // 非抢占调用
 *  motor_step_instance_start_non_preemptive(m1, 100, MOTOR_DIR_FORWARD, 100, 30); // 抢占调用
 *  motor_step_instance_clear_count(m1); // 清除步数计数
 *
 *   5. 在定时任务中定期调用更新（如每10ms）
 *  motor_step_update(P_M1_instance, 10);
 *  -------------------------------------------------------------------
 *
* 修改记录
* 日期                                      作者                             备注
* 2025-04-24                              示新Sxx                           刚创建
* 2025-05-16                              示新Sxx                           V1.0：修改成每个电机通过结构体初始化和控制，更加灵活
* 2025-05-17                              示新Sxx                           V1.1：结构体中添加step_count，用于记录步数，并提供清除步数计数的接口
********************************************************************************************************************/
/**
 * @file motor_step_controller.h
 * @brief 步进电机控制模块，用于控制直流无刷电机的步进驱动
 */
#ifndef __MOTOR_STEP_CONTROLLER_H__
#define __MOTOR_STEP_CONTROLLER_H__

/**
 * @brief 电机旋转方向枚举
 */
typedef enum
{
    MOTOR_DIR_FORWARD,  /**< 电机正向旋转 */
    MOTOR_DIR_BACKWARD, /**< 电机反向旋转 */
} motor_direction_et;

/**
 * @brief 电机步进状态枚举
 */
typedef enum
{
    MOTOR_STEP_STATE_IDLE,    /**< 空闲状态 */
    MOTOR_STEP_STATE_DRIVING, /**< 驱动状态 */
    MOTOR_STEP_STATE_COOLDOWN /**< 冷却状态 */
} motor_step_state_et;

/**
 * @brief 电机步进控制实例结构体
 */
typedef struct
{
    motor_step_state_et state;    /**< 当前状态 */
    unsigned short timer_ms;      /**< 计时器（毫秒） */
    motor_direction_et direction; /**< 旋转方向 */
    _Bool request_pending;        /**< 待处理请求标志 */
    unsigned short pwm_duty;      /**< PWM占空比 */

    unsigned short drive_duration_ms;    /**< 驱动持续时间（毫秒） */
    unsigned short cooldown_duration_ms; /**< 冷却持续时间（毫秒） */

    void (*set_pwm)(uint16_t pwm);           // 设置 PWM
    void (*set_dir)(motor_direction_et dir); // 设置方向
    void (*brake)(void);                     // 主动刹车

    int32_t step_count; /**< 步数计数器，正数表示正向步数，负数表示反向步数 */

    const char *name; /**< 实例名称 */
} motor_step_instance_t;

/**
 * @brief 启动电机步进控制
 * @param instance 电机步进控制实例
 * @param pwm_duty PWM占空比
 * @param direction 旋转方向
 * @param dirve_duration_ms 驱动持续时间（毫秒）
 * @param cooldown_duration_ms 冷却持续时间（毫秒）
 */
void motor_step_instance_start(motor_step_instance_t *instance, const unsigned short pwm_duty, const motor_direction_et direction, const unsigned short dirve_duration_ms, const unsigned short cooldown_duration_ms);

/**
 * @brief 启动电机步进控制（非抢占式）
 * @param instance 电机步进控制实例
 * @param pwm_duty PWM占空比
 * @param direction 旋转方向
 * @param drive_duration_ms 驱动持续时间（毫秒）
 * @param cooldown_duration_ms 冷却持续时间（毫秒）
 */
void motor_step_instance_start_non_preemptive(motor_step_instance_t *instance, const unsigned short pwm_duty, const motor_direction_et direction, const unsigned short drive_duration_ms, const unsigned short cooldown_duration_ms);

/**
 * @brief 更新电机步进控制状态
 * @param instance 电机步进控制实例
 * @param elapse_ms 经过的时间（毫秒）
 * @note 在一个定时任务中调用，需要固定的时间调用，并将定时时间（ms）写入这个函数的参数中。用于更新电机步进控制状态
 */
void motor_step_update(motor_step_instance_t *instance, unsigned short elapse_ms);

/**
 * @brief 清除电机步数计数
 * @param instance 电机步进控制实例
 * @note 将步数计数器清零，不影响电机当前运行状态
 */
void motor_step_clear_count(motor_step_instance_t *instance);

#endif
