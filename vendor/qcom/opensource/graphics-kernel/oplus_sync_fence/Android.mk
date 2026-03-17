LOCAL_PATH := $(call my-dir)

KGSL_SELECT := CONFIG_OPLUS_SYNC_FENCE=m
KBUILD_OPTIONS += BOARD_PLATFORM=$(TARGET_BOARD_PLATFORM)
KBUILD_OPTIONS += $(KGSL_SELECT)
KBUILD_OPTIONS += MODNAME=oplus_sync_fence

include $(CLEAR_VARS)
LOCAL_SRC_FILES   		  := $(wildcard $(LOCAL_PATH)/**/*) $(wildcard $(LOCAL_PATH)/*)
LOCAL_MODULE              := oplus_sync_fence.ko
LOCAL_MODULE_KBUILD_NAME  := oplus_sync_fence.ko
LOCAL_MODULE_TAGS         := optional
LOCAL_MODULE_DEBUG_ENABLE := true
LOCAL_MODULE_DDK_BUILD 	  := true
LOCAL_MODULE_PATH         := $(KERNEL_MODULES_OUT)

BOARD_VENDOR_KERNEL_MODULES += $(LOCAL_MODULE_PATH)/$(LOCAL_MODULE)

include $(TOP)/device/qcom/common/dlkm/Build_external_kernelmodule.mk