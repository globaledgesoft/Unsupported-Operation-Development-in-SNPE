#================================================================================
# Auto Generated Code for SeluUdoPackage
#================================================================================

# define default
default: all

# define package name
PACKAGE_NAME := $(notdir $(shell pwd))

# define library prerequisites list
lib_cpu := jni/src/CPU
lib_reg := jni/src/reg
lib_gpu := jni/src/GPU
lib_dsp := jni/src/DSP

LIB_SOURCES = $(lib_cpu) $(lib_reg) $(lib_dsp)

# define target_architecture
export TARGET_AARCH_VARS:= -march=x86-64

# define target name
export TARGET = x86-64_linux_clang

# specify compiler
ifndef CXX
export CXX := clang++
endif

# define default Android ABI
PLATFORM ?= arm64-v8a armeabi-v7a

.PHONY: all $(LIB_SOURCES) all_android all_x86 cpu dsp reg cpu_x86 dsp_android reg_x86 cpu_android gpu_android reg_android
all: $(LIB_SOURCES) all_x86 all_android

# Combined Targets
cpu: cpu_x86 cpu_android
gpu: gpu_android
dsp: dsp_android
reg: reg_x86 reg_android
clean: clean_x86 clean_android

# x86 Targets
all_x86: cpu_x86 reg_x86

cpu_x86: reg_x86
	$(call build_if_exists,$(lib_cpu),-$(MAKE) -C $(lib_cpu))

reg_x86:
	-$(MAKE) -C $(lib_reg)


clean_x86:
	@rm -rf libs obj

# Android Targets
NDK_CPU_IMPL_LIB := Udo$(PACKAGE_NAME)ImplCpu
NDK_GPU_IMPL_LIB := Udo$(PACKAGE_NAME)ImplGpu
NDK_REG_LIB := Udo$(PACKAGE_NAME)Reg

all_android: dsp_android warn_gpu check_ndk
ifneq ($(and $(wildcard $(lib_gpu)), $(wildcard $(lib_cpu))),)
	$(ANDROID_NDK_ROOT)/ndk-build APP_ABI="$(PLATFORM)"
else
	$(call build_if_exists,$(lib_cpu),-$(ANDROID_NDK_ROOT)/ndk-build APP_MODULES="$(NDK_CPU_IMPL_LIB) $(NDK_REG_LIB)" APP_ALLOW_MISSING_DEPS=true APP_ABI="$(PLATFORM)")
	$(call build_if_exists,$(lib_gpu),-$(ANDROID_NDK_ROOT)/ndk-build APP_MODULES="$(NDK_GPU_IMPL_LIB) $(NDK_REG_LIB)"  APP_ABI="$(PLATFORM)")
endif

dsp_android: reg_android
	$(call build_if_exists,$(lib_dsp),$(MAKE) -C $(lib_dsp) dsp)

cpu_android: check_ndk
	$(call build_if_exists,$(lib_cpu),$(ANDROID_NDK_ROOT)/ndk-build APP_MODULES="$(NDK_CPU_IMPL_LIB) $(NDK_REG_LIB)" APP_ALLOW_MISSING_DEPS=true APP_ABI="$(PLATFORM)")

gpu_android: warn_gpu check_ndk
	$(call build_if_exists,$(lib_gpu),$(ANDROID_NDK_ROOT)/ndk-build APP_MODULES="$(NDK_GPU_IMPL_LIB) $(NDK_REG_LIB)"  APP_ABI="$(PLATFORM)")

reg_android: check_ndk
	-$(ANDROID_NDK_ROOT)/ndk-build APP_MODULES="$(NDK_REG_LIB)" APP_ALLOW_MISSING_DEPS=true APP_ABI="$(PLATFORM)"

clean_android: check_ndk
	-$(ANDROID_NDK_ROOT)/ndk-build clean

# utilities
# Syntax: $(call build_if_exists <dir>,<cmd>)
build_if_exists = $(if $(wildcard $(1)),$(2),$(warning WARNING: $(1) does not exist. Skipping Compilation))

warn_gpu:
ifneq (1,$(words [$(PLATFORM)]))
	$(warning WARNING: More than one platform selected for GPU compilation. libOpenCl is platform dependent and may not be compatible on all selected platforms.)
endif

check_ndk:
ifeq ($(ANDROID_NDK_ROOT),)
	$(error ERROR: ANDROID_NDK_ROOT not set, skipping compilation for Android platform(s).)
endif

