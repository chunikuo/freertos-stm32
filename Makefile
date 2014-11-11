PROJECT = main

OUTDIR = build
EXECUTABLE = $(OUTDIR)/$(PROJECT).elf
BIN_IMAGE = $(OUTDIR)/$(PROJECT).bin
HEX_IMAGE = $(OUTDIR)/$(PROJECT).hex
MAP_FILE = $(OUTDIR)/$(PROJECT).map
LIST_FILE = $(OUTDIR)/$(PROJECT).lst

# set the path to STM32F429I-Discovery firmware package
STM32_LIB = CORTEX_M4F_STM32F4/Libraries/STM32F4xx_StdPeriph_Driver

# Toolchain configurations
CROSS_COMPILE ?= arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
SIZE = $(CROSS_COMPILE)size

# Cortex-M4 implements the ARMv7E-M architecture
CPU = cortex-m4
CFLAGS = -mcpu=$(CPU) -march=armv7e-m -mtune=cortex-m4
CFLAGS += -mlittle-endian -mthumb
# Need study
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp

define get_library_path
    $(shell dirname $(shell $(CC) $(CFLAGS) -print-file-name=$(1)))
endef

# Basic configurations
CFLAGS += -g -std=c99
CFLAGS += -Wall -Werror
CFLAGS += -DUSER_NAME=\"$(USER)\"

# Optimizations
CFLAGS += -O0 -ffast-math
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wl,--gc-sections
CFLAGS += -fno-common
CFLAGS += --param max-inline-insns-single=1000

# specify STM32F429
CFLAGS += -DSTM32F429_439xx

# to run from FLASH
CFLAGS += -DVECT_TAB_FLASH
LDFLAGS += -T $(PWD)/CORTEX_M4F_STM32F4/stm32f429zi_flash.ld
LDFLAGS += -L $(call get_library_path,libc.a)
LDFLAGS += -L $(call get_library_path,libgcc.a)

# STARTUP FILE
#OBJS += $(PWD)/CORTEX_M4F_STM32F4/startup_stm32f429_439xx.o

# STM32F4xx_StdPeriph_Driver
CFLAGS += -DUSE_STDPERIPH_DRIVER
CFLAGS += -D"assert_param(expr)=((void)0)"

#files
SRCDIR = src \
	 FreeRTOS \
	 FreeRTOS/portable/GCC/ARM_CM4F \
#	 $(STM32_LIB)/src \
#	 Utilities/STM32F329I-Discovery

INCDIR = inc \
	 FreeRTOS/include \
	 FreeRTOS/portable/GCC/ARM_CM4F \
	 CORTEX_M4F_STM32F4 \
	 CORTEX_M4F_STM32F4/board \
	 CORTEX_M4F_STM32F4/Libraries/CMSIS/Device/ST/STM32F4xx/Include \
	 CORTEX_M4F_STM32F4/Libraries/CMSIS/Include \
	 $(STM32_LIB)/inc \

#My restart
SRC += CORTEX_M4F_STM32F4/startup_stm32f429_439xx.s \
       CORTEX_M4F_STM32F4/startup/system_stm32f4xx.c \
       #$(PWD)/CORTEX_M4F_STM32F4/stm32f4xx_it.o \

SRC += $(wildcard $(addsuffix /*.c,$(SRCDIR))) \
	$(wildcard $(addsuffix /*.s,$(SRCDIR)))

SRC += FreeRTOS/portable/MemMang/heap_1.c

SRC += $(STM32_LIB)/src/misc.c \
	$(STM32_LIB)/src/stm32f4xx_gpio.c \
	$(STM32_LIB)/src/stm32f4xx_rcc.c \
	$(STM32_LIB)/src/stm32f4xx_usart.c \
	$(STM32_LIB)/src/stm32f4xx_syscfg.c \
	$(STM32_LIB)/src/stm32f4xx_i2c.c \
	$(STM32_LIB)/src/stm32f4xx_dma.c \
	$(STM32_LIB)/src/stm32f4xx_spi.c \
	$(STM32_LIB)/src/stm32f4xx_exti.c \
	$(STM32_LIB)/src/stm32f4xx_dma2d.c \
	$(STM32_LIB)/src/stm32f4xx_ltdc.c \
	$(STM32_LIB)/src/stm32f4xx_fmc.c \
	$(STM32_LIB)/src/stm32f4xx_rng.c \

# Traffic
#OBJS += $(PWD)/CORTEX_M4F_STM32F4/traffic/draw_graph.o
#OBJS += $(PWD)/CORTEX_M4F_STM32F4/traffic/move_car.o
#OBJS += $(PWD)/CORTEX_M4F_STM32F4/traffic/traffic.o
#CFLAGS += -I $(PWD)/CORTEX_M4F_STM32F4/traffic/include

# bulletTime
#OBJS += $(PWD)/CORTEX_M4F_STM32F4/bulletTime/bulletTime.o
#OBJS += $(PWD)/CORTEX_M4F_STM32F4/bulletTime/ball.o
#CFLAGS += -I $(PWD)/CORTEX_M4F_STM32F4/bulletTime/include

# Menu
#OBJS += $(PWD)/CORTEX_M4F_STM32F4/menu/menu.o
#CFLAGS += -I $(PWD)/CORTEX_M4F_STM32F4/menu/include

#FreeRTOS Shell
#OBJS += $(PWD)/src/shell.o
#CFLAGS += -I $(PWD)/include

OBJS += $(addprefix $(OUTDIR)/,$(patsubst %.s,%.o,$(SRC:.c=.o)))
INCLUDES = $(addprefix -I, $(INCDIR))

all: $(BIN_IMAGE)

$(BIN_IMAGE): $(EXECUTABLE)
	$(OBJCOPY) -O binary $^ $@
	$(OBJCOPY) -O ihex $^ $(HEX_IMAGE)
	$(OBJDUMP) -h -S -D $^ > $(LIST_FILE)
	$(SIZE) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJS)
	$(LD) -o $@ $^ -Map=$(MAP_FILE)	$(LDFLAGS)

$(OUTDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $(INCLUDES) $< -o $@

$(OUTDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $(INCLUDES) $< -o $@

flash:
	st-flash write $(BIN_IMAGE) 0x8000000

openocd_flash:
	openocd \
	-f board/stm32f429discovery.cfg \
	-c "init" \
	-c "reset init" \
	-c "flash probe 0" \
	-c "flash info 0" \
	-c "flash write_image erase $(BIN_IMAGE)  0x08000000" \
	-c "reset run" -c shutdown

.PHONY: clean
clean:
	rm -rf $(EXECUTABLE)
	rm -rf $(BIN_IMAGE)
	rm -rf $(HEX_IMAGE)
	rm -f $(OBJS)
	rm -f $(PROJECT).lst
