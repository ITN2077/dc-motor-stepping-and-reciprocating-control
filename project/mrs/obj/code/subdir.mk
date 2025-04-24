################################################################################
# MRS Version: 1.9.2
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
X:/8-CodeProjects/WCH/CH32V307-PlayGround-GitRepo/10.直流无刷电机步进驱动测试/project/code/Ring_Buffer.c \
X:/8-CodeProjects/WCH/CH32V307-PlayGround-GitRepo/10.直流无刷电机步进驱动测试/project/code/Task_Manager.c \
X:/8-CodeProjects/WCH/CH32V307-PlayGround-GitRepo/10.直流无刷电机步进驱动测试/project/code/UART_Data_Unpacker.c \
X:/8-CodeProjects/WCH/CH32V307-PlayGround-GitRepo/10.直流无刷电机步进驱动测试/project/code/XxxTimeSliceOffset.c \
X:/8-CodeProjects/WCH/CH32V307-PlayGround-GitRepo/10.直流无刷电机步进驱动测试/project/code/motor_step_controller.c 

OBJS += \
./code/Ring_Buffer.o \
./code/Task_Manager.o \
./code/UART_Data_Unpacker.o \
./code/XxxTimeSliceOffset.o \
./code/motor_step_controller.o 

C_DEPS += \
./code/Ring_Buffer.d \
./code/Task_Manager.d \
./code/UART_Data_Unpacker.d \
./code/XxxTimeSliceOffset.d \
./code/motor_step_controller.d 


# Each subdirectory must supply rules for building sources it contributes
code/Ring_Buffer.o: X:/8-CodeProjects/WCH/CH32V307-PlayGround-GitRepo/10.直流无刷电机步进驱动测试/project/code/Ring_Buffer.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\Libraries\doc" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_components" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Core" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Ld" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Peripheral" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Startup" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\project\user\inc" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_common" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_device" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\project\code" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
code/Task_Manager.o: X:/8-CodeProjects/WCH/CH32V307-PlayGround-GitRepo/10.直流无刷电机步进驱动测试/project/code/Task_Manager.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\Libraries\doc" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_components" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Core" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Ld" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Peripheral" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Startup" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\project\user\inc" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_common" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_device" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\project\code" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
code/UART_Data_Unpacker.o: X:/8-CodeProjects/WCH/CH32V307-PlayGround-GitRepo/10.直流无刷电机步进驱动测试/project/code/UART_Data_Unpacker.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\Libraries\doc" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_components" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Core" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Ld" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Peripheral" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Startup" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\project\user\inc" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_common" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_device" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\project\code" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
code/XxxTimeSliceOffset.o: X:/8-CodeProjects/WCH/CH32V307-PlayGround-GitRepo/10.直流无刷电机步进驱动测试/project/code/XxxTimeSliceOffset.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\Libraries\doc" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_components" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Core" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Ld" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Peripheral" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Startup" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\project\user\inc" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_common" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_device" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\project\code" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
code/motor_step_controller.o: X:/8-CodeProjects/WCH/CH32V307-PlayGround-GitRepo/10.直流无刷电机步进驱动测试/project/code/motor_step_controller.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\Libraries\doc" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_components" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Core" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Ld" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Peripheral" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\sdk\Startup" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\project\user\inc" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_common" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_device" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\project\code" -I"X:\8-CodeProjects\WCH\CH32V307-PlayGround-GitRepo\10.直流无刷电机步进驱动测试\libraries\zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

