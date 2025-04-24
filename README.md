# motor_step_controller.c && motor_step_controller.h

用于控制**无编码器直流有刷电机**进行“步进式”动作的控制模块。适用于需要控制电机“运行-停止-运行”周期行为的场景，如模拟步进运动、定程推进等。

---

## ✅ 功能概述

该模块通过状态机管理电机启停行为，支持：
- 指定**PWM占空比**与**旋转方向**
- 配置**驱动时长**与**冷却间隔**
- 多实例支持（当前最多支持 2 个电机实例）
- 与硬件层函数解耦（通过回调函数控制电机输出）

---

## 📦 使用方法

### 1. 提供硬件控制函数

你需要实现电机的 `start_pwm` 和 `stop_pwm` 控制函数，例如：

```c
void motor1_start(unsigned short duty, motor_direction_et dir) {
    if (dir == MOTOR_DIR_FORWARD) {
        gpio_set_level(E2, GPIO_HIGH); // 使能
        gpio_set_level(E3, GPIO_HIGH); // 正转
        gpio_set_level(E4, GPIO_LOW);
    } else {
        gpio_set_level(E2, GPIO_HIGH); // 使能
        gpio_set_level(E3, GPIO_LOW); 
        gpio_set_level(E4, GPIO_HIGH); // 反转
    }
}

void motor1_stop(void) {
    gpio_set_level(E2, GPIO_HIGH);
    gpio_set_level(E3, GPIO_LOW);
    gpio_set_level(E4, GPIO_LOW);
}
```

### 2. 初始化 GPIO 并创建实例

```c
gpio_init(E2, GPO, 0, GPIO_PIN_CONFIG);
gpio_init(E3, GPO, 0, GPIO_PIN_CONFIG);
gpio_init(E4, GPO, 0, GPIO_PIN_CONFIG);

motor_step_instance_t *m1 = motor_step_instance_create(motor1_start, motor1_stop);
```

### 3. 启动电机控制任务

```c
motor_step_instance_start(m1, 100, MOTOR_DIR_FORWARD, 100, 30); // 非抢占调用
motor_step_instance_start_non_preemptive(m1, 100, MOTOR_DIR_FORWARD, 100, 30); // 抢占调用
```

### 4. 周期调用更新函数

建议每 10ms 调用一次：

```c
void motor_step_update_task(void) {
    motor_step_update(10);  //要将定时的时间传入，供程序内定时器使用
}
```

---

## ⚙️ 实现逻辑

### 状态机：

模块基于三态状态机进行控制：

- `IDLE`：等待新请求，准备启动
- `DRIVING`：正在驱动电机运行，计时中
- `COOLDOWN`：驱动完成后的冷却阶段，等待下一次驱动

### 启动逻辑：

- 如果在 `IDLE` 状态，收到新请求则立即启动电机
- 如果在 `DRIVING` 状态，方向相同则刷新参数；方向不同则先冷却再执行
- 在冷却完成后，若存在新请求则自动执行

### 结构体字段：

- `pwm_duty`：占空比
- `direction`：正转/反转
- `drive_duration_ms`：本次驱动时长
- `cooldown_duration_ms`：驱动后冷却时长
- `timer_ms`：当前状态计时器
- `request_pending`：是否有待处理请求

---

## 🔌 未来可扩展

- 增加对驱动时间与冷却时间配置的动态管理接口
- 增加支持多种工作模式（如脉冲计数）
- 增加运行状态反馈机制（回调、LED 指示、串口打印等）

