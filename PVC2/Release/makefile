################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include RTC_PCF8583/subdir.mk
-include PVC/subdir.mk
-include LCD/subdir.mk
-include IR_UNI/subdir.mk
-include I2C_TWI/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
PVC2.lss \

FLASH_IMAGE += \
PVC2.hex \

EEPROM_IMAGE += \
PVC2.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: PVC2.elf secondary-outputs

# Tool invocations
PVC2.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,PVC2.map -mmcu=atmega32 -o "PVC2.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

PVC2.lss: PVC2.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S PVC2.elf  >"PVC2.lss"
	@echo 'Finished building: $@'
	@echo ' '

PVC2.hex: PVC2.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex PVC2.elf  "PVC2.hex"
	@echo 'Finished building: $@'
	@echo ' '

PVC2.eep: PVC2.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex PVC2.elf  "PVC2.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: PVC2.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 PVC2.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) PVC2.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
