################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../gsm_module/gsm_module.c 

OBJS += \
./gsm_module/gsm_module.o 

C_DEPS += \
./gsm_module/gsm_module.d 


# Each subdirectory must supply rules for building sources it contributes
gsm_module/%.o gsm_module/%.su gsm_module/%.cyclo: ../gsm_module/%.c gsm_module/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/poc/project/FInal_project/gsm_module" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-gsm_module

clean-gsm_module:
	-$(RM) ./gsm_module/gsm_module.cyclo ./gsm_module/gsm_module.d ./gsm_module/gsm_module.o ./gsm_module/gsm_module.su

.PHONY: clean-gsm_module

