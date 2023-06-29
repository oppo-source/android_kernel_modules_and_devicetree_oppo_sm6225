ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)
	dtbo-$(CONFIG_ARCH_KHAJE) += khaje-muses-22341-overlay.dtbo\
								khaje-muses-22342-overlay.dtbo\
								khaje-muses-22343-overlay.dtbo

	khaje-muses-22341-overlay.dtbo-base := khaje-muses.dtb
	khaje-muses-22342-overlay.dtbo-base := khaje-muses.dtb
	khaje-muses-22343-overlay.dtbo-base := khaje-muses.dtb
endif

always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files    := *.dtb *.dtbo

