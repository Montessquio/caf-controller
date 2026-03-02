SDK_PATH = ./vendor/gecko_sdk
DEVICE_PATH = $(SDK_PATH)/platform/Device/SiliconLabs/EFR32BG22
VENDOR_LIB_PATH = ./vendor/arm-none-eabi/lib

SRC_DIR = src

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:.c=.o)

INCLUDES = -I$(DEVICE_PATH)/Include \
           -I$(SDK_PATH)/platform/common/inc \
           -I$(SDK_PATH)/platform/CMSIS/Core/Include \
		   -I$(SDK_PATH)/platform/emlib/inc

DEFINES = -DEFR32BG22C224F512IM40

CFLAGS = --target=arm-none-eabi \
         -mcpu=cortex-m33 \
         -mthumb \
         -mfloat-abi=hard \
         -mfpu=fpv5-sp-d16 \
         -mcmse \
         $(DEFINES) \
         $(INCLUDES) \
         -O0 -g \
         -Wall -Wextra \
         -ffunction-sections -fdata-sections

LDFLAGS = --target=arm-none-eabi \
          -mcpu=cortex-m33 \
          -mthumb \
          -mfloat-abi=hard \
          -mfpu=fpv5-sp-d16 \
          -T efr32bg22.ld \
          -fuse-ld=lld \
          -nostartfiles \
          -nodefaultlibs \
          -L$(VENDOR_LIB_PATH) \
          -lgcc \
          -lc \
          -lnosys \
          -Wl,--gc-sections \
          -Wl,-Map=firmware.map

all: firmware.elf firmware.bin

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	clang $(CFLAGS) -c $< -o $@

firmware.elf: $(OBJS)
	clang $(LDFLAGS) $(OBJS) -o $@
	llvm-size $@

firmware.bin: firmware.elf
	llvm-objcopy -O binary $< $@

clean:
	rm -f $(OBJS) firmware.elf firmware.bin firmware.map

.PHONY: all clean