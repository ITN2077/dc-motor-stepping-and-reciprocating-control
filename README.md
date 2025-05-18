# DC Motor Control Module

该模块用于控制直流有刷电机实现模拟步进式运动与往复式运动，适用于嵌入式系统（如单片机）。通过状态机管理电机的驱动和冷却周期，支持多实例控制，并与底层硬件驱动解耦。

---

## 功能特性
- **步进控制**  
  - 非抢占式启动驱动周期（驱动时间 + 冷却时间）
  - 支持正反向运动，自动更新步数计数器
  - 提供步数清零接口
- **往复运动控制**  
  - 支持正反向交替运动，可配置正反向驱动时间
  - 支持冷却时间配置
  - 提供启动/停止接口
- **多实例支持**  
  - 通过结构体实例化多个电机控制器
  - 控制逻辑与硬件驱动分离（需用户实现PWM、方向、刹车函数）

---

## 快速开始

### 依赖项
- C编译器支持C99标准
- 标准头文件：`<stdbool.h>`, `<stdint.h>`
- 用户需实现底层硬件驱动函数（PWM、方向、刹车）

---

### 步进控制示例

#### 1. 初始化硬件驱动
```c
// 示例：初始化GPIO和PWM
gpio_init(E2, GPO, 0, GPIO_PIN_CONFIG); // 方向引脚
gpio_init(E3, GPO, 0, GPIO_PIN_CONFIG); // 使能引脚
gpio_init(E4, GPO, 0, GPIO_PIN_CONFIG); // 刹车引脚

// 实现控制函数
void motor_set_pwm(uint16_t pwm) { /* PWM设置逻辑 */ }
void motor_set_dir(motor_direction_et dir) { /* 方向设置逻辑 */ }
void motor_brake(void) { /* 刹车逻辑 */ }
```

#### 2. 创建控制实例
```c
motor_step_instance_t motor_step = {
    .state = MOTOR_STEP_STATE_IDLE,
    .set_pwm = motor_set_pwm,
    .set_dir = motor_set_dir,
    .brake = motor_brake,
    .name = "STEP_MOTOR"
};
```

#### 3. 定时任务中更新状态
```c
// 每10ms调用一次（周期需严格保证）
motor_step_update(&motor_step, 10);
```

#### 4. 启动步进运动
```c
// 参数：PWM占空比、方向、驱动时间(ms)、冷却时间(ms)
motor_step_instance_start(&motor_step, 100, MOTOR_DIR_FORWARD, 100, 30);
// 清除步数计数
motor_step_clear_count(&motor_step);
```

---

### 往复运动示例

#### 1. 创建往复控制实例
```c
motor_recip_instance_t motor_recip = {
    .state = MOTOR_RECIP_STATE_IDLE,
    .set_pwm = motor_set_pwm,
    .set_dir = motor_set_dir,
    .brake = motor_brake,
    .name = "RECIP_MOTOR"
};
```

#### 2. 定时任务中更新状态
```c
motor_recip_update(&motor_recip, 10);
```

#### 3. 启动往复运动
```c
// 参数：PWM占空比、正向时间(ms)、反向时间(ms)、冷却时间(ms)
motor_recip_instance_start(&motor_recip, 100, 1000, 600, 100);
// 停止运动
motor_recip_instance_stop(&motor_recip);
```

---

## API 参考

### 步进控制
- **`motor_step_instance_start()`**  
  启动步进运动（仅在空闲状态下生效）。
- **`motor_step_clear_count()`**  
  清零步数计数器。
- **`motor_step_update()`**  
  更新状态机，需周期性调用。

### 往复控制
- **`motor_recip_instance_start()`**  
  启动往复运动。
- **`motor_recip_instance_stop()`**  
  强制停止运动并复位状态。
- **`motor_recip_update()`**  
  更新状态机，需周期性调用。

---

## 注意事项
1. **定时任务周期**  
   `motor_step_update` 和 `motor_recip_update` 必须严格按固定周期调用（如10ms），否则时间计算会偏差。
2. **抢占式控制**  
   步进控制为非抢占式，若电机正在运行，新请求会被忽略。
3. **硬件驱动实现**  
   用户需根据实际硬件实现 `set_pwm`、`set_dir`、`brake` 函数，确保逻辑正确。
4. **线程安全**  
   若在RTOS中使用，需自行添加互斥锁保护共享资源（如实例状态）。

---

## 版本历史
- **V1.0** (2025-05-16)  
  支持步进控制，多实例管理。
- **V1.1** (2025-05-17)  
  添加步数计数器及清零接口。
- **V1.2** (2025-05-18)  
  新增往复运动控制功能。

```