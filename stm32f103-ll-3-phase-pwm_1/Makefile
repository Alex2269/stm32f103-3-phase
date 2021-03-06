
######################################
# target
######################################
TARGET = stm32f103-ll-3-phase-pwm


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -O2


#######################################
# paths
#######################################
# source path
ROOT_SRC_DIR = .
SOURCES_DIR   += $(shell find ${ROOT_SRC_DIR} -type d)

# firmware library path
PERIFLIB_PATH = 

# Build path
BUILD_DIR = build

######################################
# source
######################################
ROOT_DIR = .
# $(shell find ${ROOT_DIR} -type d -print0 | xargs -0 rmdir -p --ignore-fail-on-non-empty) # remove empty directories
# C sources
C_SOURCES   += $(shell find ${ROOT_DIR} -name '*.c')


# ASM sources
ASM_SOURCES =  \
startup_stm32f103xb.s


######################################
# firmware library
######################################
PERIFLIB_SOURCES = 


#######################################
# binaries
#######################################
BINPATH = /usr/bin
PREFIX = arm-none-eabi-
CC = $(BINPATH)/$(PREFIX)gcc -std=c11
AS = $(BINPATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(BINPATH)/$(PREFIX)objcopy
AR = $(BINPATH)/$(PREFIX)ar
SZ = $(BINPATH)/$(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m3

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi


# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DSTM32F103xB \
-DUSE_FULL_LL_DRIVER \
-DHSE_VALUE=8000000 \
-DHSE_STARTUP_TIMEOUT=100 \
-DLSE_STARTUP_TIMEOUT=5000 \
-DLSE_VALUE=32768 \
-DHSI_VALUE=8000000 \
-DLSI_VALUE=40000 \
-DVDD_VALUE=3300 \
-DPREFETCH_ENABLE=1


# AS includes
AS_INCLUDES = 

# C includes
ROOT_INC_DIR = .
C_INCLUDES += $(shell find ${ROOT_INC_DIR} -type d -name Inc | sed 's,\./,-I,g')

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F103C8Tx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# Make flash
#	"write through st-link v2"
# Make stm32flash:
#	"write through uart /dev/ttyUSB0"
#######################################
flash:
	st-flash --reset write $(BUILD_DIR)/$(TARGET).bin 0x8000000

stm32flash:
	stm32flash -w $(BUILD_DIR)/$(TARGET).bin -v -g 0x0 /dev/ttyUSB0
#######################################
debug:
	openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg &
	gdb-multiarch $(BUILD_DIR)/$(TARGET).elf
#######################################
# clean up
#######################################
clean:
	-rm -fR .dep $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***
