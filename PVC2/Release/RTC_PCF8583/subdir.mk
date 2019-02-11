################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../RTC_PCF8583/rtc_pcf8583.c 

OBJS += \
./RTC_PCF8583/rtc_pcf8583.o 

C_DEPS += \
./RTC_PCF8583/rtc_pcf8583.d 


# Each subdirectory must supply rules for building sources it contributes
RTC_PCF8583/%.o: ../RTC_PCF8583/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


