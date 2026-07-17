# ============================================================
# Sensor Monitoring System
# Linux Kernel Module Makefile
# ============================================================

MODULE_NAME := sms_sensor

KERNEL_DIR := $(HOME)/embedded_lab1/kernel/linux-5.15
ARCH := arm
CROSS_COMPILE := arm-linux-gnueabihf-

obj-m += $(MODULE_NAME).o

$(MODULE_NAME)-objs := \
	src/sms_sensor.o \
	src/sms_fops.o

ccflags-y += -I$(src)/inc

.PHONY: all clean info

all:
	$(MAKE) -C $(KERNEL_DIR) \
		M=$(CURDIR) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		modules

clean:
	$(MAKE) -C $(KERNEL_DIR) \
		M=$(CURDIR) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		clean

info:
	@echo "========================================="
	@echo " Sensor Monitoring System Driver"
	@echo "========================================="
	@echo "Module name    : $(MODULE_NAME)"
	@echo "Kernel dir     : $(KERNEL_DIR)"
	@echo "Module dir     : $(CURDIR)"
	@echo "Architecture   : $(ARCH)"
	@echo "Cross compiler : $(CROSS_COMPILE)"