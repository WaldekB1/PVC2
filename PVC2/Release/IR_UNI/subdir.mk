################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../IR_UNI/ir_jvc.c \
../IR_UNI/ir_rc5.c \
../IR_UNI/ir_samsung.c \
../IR_UNI/ir_sony.c 

OBJS += \
./IR_UNI/ir_jvc.o \
./IR_UNI/ir_rc5.o \
./IR_UNI/ir_samsung.o \
./IR_UNI/ir_sony.o 

C_DEPS += \
./IR_UNI/ir_jvc.d \
./IR_UNI/ir_rc5.d \
./IR_UNI/ir_samsung.d \
./IR_UNI/ir_sony.d 


# Each subdirectory must supply rules for building sources it contributes
IR_UNI/%.o: ../IR_UNI/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


