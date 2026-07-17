# ============================================================
# Sensor Monitoring System - Kernel Driver
# Build target: ARMv7 / QEMU vexpress-a9
# ============================================================

KDIR := $(HOME)/embedded_lab1/kernel/linux-5.15

ARCH := arm
CROSS_COMPILE := arm-linux-gnueabihf-

# Kernel module name
MODULE_NAME := sms_sensor

# When invoked directly, use CURDIR.
# When invoked by Kbuild, use src supplied by the kernel.
MODULE_DIR := $(if $(src),$(src),$(CURDIR))

# Automatically find every C source file in src/
SOURCE_FILES := $(wildcard $(MODULE_DIR)/src/*.c)

# Convert absolute C paths into relative object paths
MODULE_OBJECTS := $(patsubst $(MODULE_DIR)/%.c,%.o,$(SOURCE_FILES))

# Final output: sms_sensor.ko
obj-m += $(MODULE_NAME).o

# Link every object in src/ into the final module
$(MODULE_NAME)-y := $(MODULE_OBJECTS)

# Include path and compiler flags
ccflags-y += -I$(MODULE_DIR)/inc
ccflags-y += -march=armv7-a
ccflags-y += -marm

.PHONY: all clean info

all:
	$(MAKE) -C $(KDIR) \
		M=$(CURDIR) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		modules

clean:
	$(MAKE) -C $(KDIR) \
		M=$(CURDIR) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		clean

info:
	@echo "========================================="
	@echo " Sensor Monitoring System Driver"
	@echo "========================================="
	@echo "Module directory : $(MODULE_DIR)"
	@echo "Kernel directory : $(KDIR)"
	@echo "Module name      : $(MODULE_NAME)"
	@echo "Source files     : $(SOURCE_FILES)"
	@echo "Module objects   : $(MODULE_OBJECTS)"