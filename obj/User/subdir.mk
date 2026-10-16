################################################################################
# MRS Version: 2.5.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v30x_it.c \
../User/img_data.c \
../User/main.c \
../User/st7735s_driver.c \
../User/system_ch32v30x.c 

C_DEPS += \
./User/ch32v30x_it.d \
./User/img_data.d \
./User/main.d \
./User/st7735s_driver.d \
./User/system_ch32v30x.d 

OBJS += \
./User/ch32v30x_it.o \
./User/img_data.o \
./User/main.o \
./User/st7735s_driver.o \
./User/system_ch32v30x.o 

DIR_OBJS += \
./User/*.o \

DIR_DEPS += \
./User/*.d \

DIR_EXPANDS += \
./User/*.253r.expand \


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-wch-elf-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -msave-restore -fmax-errors=20 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fsingle-precision-constant -Wunused -Wuninitialized -g -I"d:/单片机项目/st7735屏幕/Debug" -I"d:/单片机项目/st7735屏幕/Core" -I"d:/单片机项目/st7735屏幕/User" -I"d:/单片机项目/st7735屏幕/Peripheral/inc" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@

