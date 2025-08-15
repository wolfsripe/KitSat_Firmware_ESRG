################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/sx1278/SX1278.c \
../Core/sx1278/SX1278_hw.c 

OBJS += \
./Core/sx1278/SX1278.o \
./Core/sx1278/SX1278_hw.o 

C_DEPS += \
./Core/sx1278/SX1278.d \
./Core/sx1278/SX1278_hw.d 


# Each subdirectory must supply rules for building sources it contributes
Core/sx1278/%.o Core/sx1278/%.su Core/sx1278/%.cyclo: ../Core/sx1278/%.c Core/sx1278/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F767xx -c -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-sx1278

clean-Core-2f-sx1278:
	-$(RM) ./Core/sx1278/SX1278.cyclo ./Core/sx1278/SX1278.d ./Core/sx1278/SX1278.o ./Core/sx1278/SX1278.su ./Core/sx1278/SX1278_hw.cyclo ./Core/sx1278/SX1278_hw.d ./Core/sx1278/SX1278_hw.o ./Core/sx1278/SX1278_hw.su

.PHONY: clean-Core-2f-sx1278

