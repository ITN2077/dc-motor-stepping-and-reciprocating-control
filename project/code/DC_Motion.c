#include "DC_Motion.h"
#include <stdbool.h>
#ifndef NULL
#define NULL ((void *)0)
#endif
/**
 * @brief 启动电机步进控制（非抢占式）
 * @param instance 电机步进控制实例
 * @param pwm_duty PWM占空比
 * @param direction 旋转方向
 * @param drive_duration_ms 驱动持续时间（毫秒）
 * @param cooldown_duration_ms 冷却持续时间（毫秒）
 */
bool motor_step_instance_start(motor_step_instance_t *instance, const unsigned short pwm_duty, const motor_direction_et direction, const unsigned short drive_duration_ms, const unsigned short cooldown_duration_ms)
{
    if (instance == NULL || instance->set_pwm == NULL || instance->brake == NULL || instance->set_dir == NULL)
    {
        return false; // 跳过无效的实例或驱动
    }

    // 只在空闲状态下启动电机
    if (instance->state == MOTOR_STEP_STATE_IDLE)
    {
        instance->request_pending = true;
        instance->timer_ms = 0;
        instance->pwm_duty = pwm_duty;
        instance->direction = direction;
        instance->drive_duration_ms = drive_duration_ms;
        instance->cooldown_duration_ms = cooldown_duration_ms;
        return true;
    }
    else
    {
        return false; // 其他状态不做任何操作，等待当前操作完成
    }
}

/**
 * @brief 清除电机步数计数
 * @param instance 电机步进控制实例
 * @note 将步数计数器清零，不影响电机当前运行状态
 */
void motor_step_clear_count(motor_step_instance_t *instance)
{
    if (instance == NULL)
        return;

    instance->step_count = 0;
}

/**
 * @brief 更新电机步进控制状态
 */
void motor_step_update(motor_step_instance_t *instance, unsigned short elapse_ms)
{
    if (instance == NULL || instance->set_pwm == NULL || instance->brake == NULL || instance->set_dir == NULL)
    {
        return; // 跳过无效的实例或驱动
    }

    switch (instance->state)
    {
    case MOTOR_STEP_STATE_IDLE:
        if (instance->request_pending)
        {
            instance->set_pwm(instance->pwm_duty);
            instance->set_dir(instance->direction);
            instance->state = MOTOR_STEP_STATE_DRIVING;
            instance->timer_ms = 0;
            instance->request_pending = false;
        }
        break;
    case MOTOR_STEP_STATE_DRIVING:
        instance->timer_ms += elapse_ms;
        if (instance->timer_ms >= instance->drive_duration_ms)
        {
            instance->state = MOTOR_STEP_STATE_COOLDOWN;
            instance->timer_ms = 0;
            instance->brake();
            // 更新步数计数
            if (instance->direction == MOTOR_DIR_FORWARD)
            {
                instance->step_count++;
            }
            else
            {
                instance->step_count--;
            }
        }
        break;
    case MOTOR_STEP_STATE_COOLDOWN:
        instance->timer_ms += elapse_ms;
        if (instance->timer_ms >= instance->cooldown_duration_ms)
        {
            instance->state = MOTOR_STEP_STATE_IDLE;
            instance->timer_ms = 0;
        }
        break;
    default:
        break;
    }
}

/**
 * @brief 启动电机往复运动控制
 */
bool motor_recip_instance_start(motor_recip_instance_t *instance, const unsigned short pwm_duty, const unsigned short forward_move_duration_ms, const unsigned short backward_move_duration_ms, const unsigned short cooldown_duration_ms)
{
    if (instance == NULL || instance->set_pwm == NULL || instance->brake == NULL || instance->set_dir == NULL)
    {
        return false; // 跳过无效的实例或驱动
    }

    if (instance->state == MOTOR_RECIP_STATE_IDLE)
    {
        // 保存参数
        instance->pwm_duty = pwm_duty;
        instance->forward_duration_ms = forward_move_duration_ms;
        instance->backward_duration_ms = backward_move_duration_ms;
        instance->cooldown_duration_ms = cooldown_duration_ms;

        // 设置待处理标志
        instance->request_pending = true;
        instance->timer_ms = 0;
        instance->running = true;
        return true;
    }
    else
    {
        return false; // 其他状态不做任何操作，等待当前操作完成
    }
}

/**
 * @brief 停止电机往复运动
 */
void motor_recip_instance_stop(motor_recip_instance_t *instance)
{
    if (instance == NULL)
        return;

    instance->running = false;
    instance->state = MOTOR_RECIP_STATE_IDLE;
    instance->brake();
}

/**
 * @brief 更新电机往复运动状态
 */
void motor_recip_update(motor_recip_instance_t *instance, unsigned short elapse_ms)
{
    if (instance == NULL || instance->set_pwm == NULL || instance->brake == NULL || instance->set_dir == NULL)
    {
        return; // 跳过无效的实例或驱动
    }

    // 状态机处理
    if (instance->running)
    {
        switch (instance->state)
        {
        case MOTOR_RECIP_STATE_IDLE:
            if (instance->request_pending)
            {
                instance->set_pwm(instance->pwm_duty);
                instance->set_dir(MOTOR_DIR_FORWARD);
                instance->state = MOTOR_RECIP_STATE_FORWARD;
                instance->timer_ms = 0;
                instance->request_pending = false;
            }
            break;

        case MOTOR_RECIP_STATE_FORWARD:
            instance->timer_ms += elapse_ms;
            if (instance->timer_ms >= instance->forward_duration_ms)
            {
                instance->state = MOTOR_RECIP_STATE_FORWARD_COOLDOWN;
                instance->timer_ms = 0;
                instance->brake();
            }
            break;

        case MOTOR_RECIP_STATE_FORWARD_COOLDOWN:
            instance->timer_ms += elapse_ms;
            if (instance->timer_ms >= instance->cooldown_duration_ms)
            {
                instance->state = MOTOR_RECIP_STATE_BACKWARD;
                instance->timer_ms = 0;
                instance->set_pwm(instance->pwm_duty);
                instance->set_dir(MOTOR_DIR_BACKWARD);
            }
            break;

        case MOTOR_RECIP_STATE_BACKWARD:
            instance->timer_ms += elapse_ms;
            if (instance->timer_ms >= instance->backward_duration_ms)
            {
                instance->state = MOTOR_RECIP_STATE_BACKWARD_COOLDOWN;
                instance->timer_ms = 0;
                instance->brake();
            }
            break;

        case MOTOR_RECIP_STATE_BACKWARD_COOLDOWN:
            instance->timer_ms += elapse_ms;
            if (instance->timer_ms >= instance->cooldown_duration_ms)
            {
                instance->state = MOTOR_RECIP_STATE_FORWARD;
                instance->timer_ms = 0;
                instance->set_pwm(instance->pwm_duty);
                instance->set_dir(MOTOR_DIR_FORWARD);
            }
            break;

        default:
            break;
        }
    }
}
