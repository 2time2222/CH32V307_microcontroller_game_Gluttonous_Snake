################################################################################
# MRS Version: 2.5.0
# Automatically-generated file. Do not edit!
################################################################################
-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
st7735фад╩.hex \

SECONDARY_LIST += \
st7735фад╩.lst \

SECONDARY_SIZE += \
st7735фад╩.siz \


# All Target
all: 
	$(MAKE) --no-print-directory main-build 

main-build: st7735фад╩.elf secondary-outputs

# Tool invocations
st7735фад╩.elf: $(OBJS) $(USER_OBJS_ESCAPE)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU RISC-V Cross C Linker'
	riscv-wch-elf-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -msave-restore -fmax-errors=20 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fsingle-precision-constant -Wunused -Wuninitialized -g -T "d:\╣╔ф╛╩ЗоНд©\st7735фад╩\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"st7735фад╩.map" --specs=nano.specs --specs=nosys.specs -o "st7735фад╩.elf" $(OBJS) $(LIBS) -lprintf
	@echo 'Finished building target: $@'
	@
st7735фад╩.hex: st7735фад╩.elf
	@echo 'Invoking: GNU RISC-V Cross Create Flash Image'
	@	riscv-wch-elf-objcopy -O ihex "st7735фад╩.elf" "st7735фад╩.hex"
	@echo 'Finished building $@'
	@
st7735фад╩.lst: st7735фад╩.elf
	@echo 'Invoking: GNU RISC-V Cross Create Listing'
	@	riscv-wch-elf-objdump --all-headers --demangle --disassemble "st7735фад╩.elf" > "st7735фад╩.lst"
	@echo 'Finished building $@'
	@
st7735фад╩.siz: st7735фад╩.elf
	@echo 'Invoking: GNU RISC-V Cross Print Size'
	riscv-wch-elf-size --format=berkeley "st7735фад╩.elf"
	@echo 'Finished building $@'
	@

# Other Targets
clean:
	-$(RM) $(DIR_OBJS) $(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE) st7735фад╩.elf
	-$(RM) $(DIR_EXPANDS) $(CALLGRAPH_DOT)
	-$(RM) $(DIR_DEPS)
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets