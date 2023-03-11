LOCAL_PATH := $(call my-dir)

ROOT_DIR     := $(LOCAL_PATH)/../../..
CORE_DIR     := $(ROOT_DIR)/libnumero/src
LIBRETRO_DIR := $(ROOT_DIR)/libnumero/libretro

HAVE_NETWORK := 1

include $(ROOT_DIR)/Makefile.common

COREFLAGS := -DINLINE=inline -DHAVE_STDINT_H -DHAVE_INTTYPES_H \
 -D__LIBRETRO__ -DVIDEO_RGB565 -Wno-c++11-narrowing

CFLAGS :=  -funroll-loops -fsigned-char -ffast-math -fno-signed-zeros \
 -mfloat-abi=hard -mfpu=neon -mthumb -msoft-float

ifeq ($(HAVE_NETWORK),1)
  COREFLAGS += -DHAVE_NETWORK
endif

GIT_VERSION := " $(shell git rev-parse --short HEAD || echo unknown)"
ifneq ($(GIT_VERSION)," unknown")
  COREFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\"
endif

include $(CLEAR_VARS)
LOCAL_MODULE    := retro
LOCAL_SRC_FILES := $(SOURCES_CXX) $(SOURCES_C)
LOCAL_CXXFLAGS  := $(COREFLAGS) $(INCFLAGS) $(CFLAGS)
LOCAL_CFLAGS    := $(INCFLAGS) $(CFLAGS)
LOCAL_LDFLAGS   := -Wl,-version-script=$(LIBRETRO_DIR)/link.T
include $(BUILD_SHARED_LIBRARY)
