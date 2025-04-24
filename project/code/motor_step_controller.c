#include "motor_step_controller.h"

static motor_step_instance_t motor_step_instances[MAX_MOTOR_INSTANCES];
static unsigned char motor_step_instance_count = 0;

motor_step_instance_t *motor_step_instance_create(const void (*start_pwm)(unsigned short duty, motor_direction_et dir), const void (*stop_pwm)(void))
{
    if (motor_step_instance_count >= MAX_MOTOR_INSTANCES)
    {
        return (void *)0;
    }

    motor_step_instance_t *instance = &motor_step_instances[motor_step_instance_count++];
    instance->state = MOTOR_STEP_STATE_IDLE;
    instance->timer_ms = 0;
    instance->direction = MOTOR_DIR_FORWARD;
    instance->request_pending = 0;
    instance->pwm_duty = 0;
    instance->start_pwm = start_pwm;
    instance->stop_pwm = stop_pwm;

    return instance;
}

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
            if (instance->start_pwm != (void *)0)
            {
                instance->start_pwm(instance->pwm_duty, instance->direction);
            }
        }
        // 不同方向：立即进入冷却，等待执行新方向
        else
        {
            // 立即停止当前驱动
            if (instance->stop_pwm != (void *)0)
            {
                instance->stop_pwm();
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
    }
    // 其他状态不做任何操作，等待当前操作完成
}

void motor_step_update(unsigned short elapse_ms)
{
    for (unsigned char i = 0; i < motor_step_instance_count; i++)
    {
        motor_step_instance_t *instance = &motor_step_instances[i];
        if (instance == (void *)0 || instance->start_pwm == (void *)0 || instance->stop_pwm == (void *)0)
        {
            continue; // 跳过无效的实例或驱动
        }

        switch (instance->state)
        {
        case MOTOR_STEP_STATE_IDLE:
            if (instance->request_pending)
            {
                instance->start_pwm(instance->pwm_duty, instance->direction);
                instance->state = MOTOR_STEP_STATE_DRIVING;
                instance->timer_ms = 0;
                instance->request_pending = 0;
            }
            break;
        case MOTOR_STEP_STATE_DRIVING:
            instance->timer_ms += elapse_ms;
            if (instance->timer_ms >= instance->drive_duration_ms)
            {
                instance->stop_pwm();
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
}
