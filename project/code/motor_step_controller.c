#include "motor_step_controller.h"

void motor_step_instance_start(motor_step_instance_t *instance, const unsigned short pwm_duty, const motor_direction_et direction, const unsigned short dirve_duration_ms, const unsigned short cooldown_duration_ms)
{
    // 空闲状态：直接启动
    if (instance->state == MOTOR_STEP_STATE_IDLE)
    {
        instance->request_pending = 1;
        instance->timer_ms = 0;
        instance->pwm_duty = pwm_duty;
        instance->direction = direction;
        instance->drive_duration_ms = dirve_duration_ms;
        instance->cooldown_duration_ms = cooldown_duration_ms;
    }
    // 驱动状态：检查方向
    else if (instance->state == MOTOR_STEP_STATE_DRIVING)
    {
        // 相同方向：刷新参数
        if (instance->direction == direction)
        {
            // 直接更新参数，继续当前驱动
            instance->pwm_duty = pwm_duty;
            instance->drive_duration_ms = dirve_duration_ms;
            instance->cooldown_duration_ms = cooldown_duration_ms;
            instance->timer_ms = 0; // 重置计时器，重新开始计时

            // 更新PWM占空比（不改变方向）
            if ((instance->set_pwm != (void *)0) && (instance->set_dir != (void *)0))
            {
                instance->set_pwm(instance->pwm_duty);
                instance->set_dir(instance->direction);
            }
        }
        // 不同方向：立即进入冷却，等待执行新方向
        else
        {
            // 立即停止当前驱动
            if (instance->brake != (void *)0)
            {
                instance->brake();
            }

            // 进入冷却状态
            instance->state = MOTOR_STEP_STATE_COOLDOWN;
            instance->timer_ms = 0;

            // 保存新请求的参数，设置请求标志
            instance->request_pending = 1;
            instance->pwm_duty = pwm_duty;
            instance->direction = direction;
            instance->drive_duration_ms = dirve_duration_ms;
            instance->cooldown_duration_ms = cooldown_duration_ms;
        }
    }
}
/**
 * @brief 启动电机步进控制（非抢占式）
 * @param instance 电机步进控制实例
 * @param pwm_duty PWM占空比
 * @param direction 旋转方向
 * @param drive_duration_ms 驱动持续时间（毫秒）
 * @param cooldown_duration_ms 冷却持续时间（毫秒）
 */
void motor_step_instance_start_non_preemptive(motor_step_instance_t *instance, const unsigned short pwm_duty, const motor_direction_et direction, const unsigned short drive_duration_ms, const unsigned short cooldown_duration_ms)
{
    // 只在空闲状态下启动电机
    if (instance->state == MOTOR_STEP_STATE_IDLE)
    {
        instance->request_pending = 1;
        instance->timer_ms = 0;
        instance->pwm_duty = pwm_duty;
        instance->direction = direction;
        instance->drive_duration_ms = drive_duration_ms;
        instance->cooldown_duration_ms = cooldown_duration_ms;

        // 根据方向更新步数
        if (direction == MOTOR_DIR_FORWARD)
        {
            instance->step_count++;
        }
        else
        {
            instance->step_count--;
        }
    }
    // 其他状态不做任何操作，等待当前操作完成
}

void motor_step_update(motor_step_instance_t *instance, unsigned short elapse_ms)
{
    if (instance == (void *)0 || instance->set_pwm == (void *)0 || instance->brake == (void *)0 || instance->set_dir == (void *)0)
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
            instance->request_pending = 0;
        }
        break;
    case MOTOR_STEP_STATE_DRIVING:
        instance->timer_ms += elapse_ms;
        if (instance->timer_ms >= instance->drive_duration_ms)
        {
            instance->brake();
            instance->state = MOTOR_STEP_STATE_COOLDOWN;
            instance->timer_ms = 0;
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
 * @brief 清除电机步数计数
 * @param instance 电机步进控制实例
 * @note 将步数计数器清零，不影响电机当前运行状态
 */
void motor_step_clear_count(motor_step_instance_t *instance)
{
    if (instance != (void *)0)
    {
        instance->step_count = 0;
    }
}
