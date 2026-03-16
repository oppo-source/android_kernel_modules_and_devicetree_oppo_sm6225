TARGET_ENABLE_STM_ESE := n

ifeq ($(CONFIG_ARCH_KALAMA),y)
dtbo-y += kalama-ese-mtp.dtbo
dtbo-y += kalama-ese-cdp.dtbo
dtbo-y += kalama-ese-qrd.dtbo

dtbo-y += kalama-v2-ese-mtp.dtbo
dtbo-y += kalama-v2-ese-cdp.dtbo
dtbo-y += kalama-v2-ese-qrd.dtbo
endif

ifeq ($(TARGET_ENABLE_STM_ESE),y)
ifeq ($(CONFIG_ARCH_CROW),y)
dtbo-y += crow-ese-idp.dtbo
endif
endif

always-y	:= $(dtb-y) $(dtbo-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb *.dtbo
