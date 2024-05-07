KHAJE_BASE_DTB += ../qcom/khaje.dtb

OPLUS_KHAJE_BOARDS += \
		muses-22341-khaje-qrd-nopmi-overlay.dtbo \
		muses-22342-khaje-qrd-nopmi-overlay.dtbo \
		muses-22343-khaje-qrd-nopmi-overlay.dtbo \
		avatar-l4-23271-khaje-qrd-nopmi-overlay.dtbo \
		avatar-l4-23273-khaje-qrd-nopmi-overlay.dtbo \
		avatar-l4-23274-khaje-qrd-nopmi-overlay.dtbo

bengal-dtb-y +=
bengal-overlays-dtb-y += $(OPLUS_KHAJE_BOARDS) $(KHAJE_BASE_DTB)
dtb-y += $(OPLUS_KHAJE_BOARDS) $(KHAJE_BASE_DTB)

always-y	:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb *.dtbo
