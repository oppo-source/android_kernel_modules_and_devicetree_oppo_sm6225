# add-overlay defines the target with following naming convention:
# <base>-<board>-dtbs = base.dtb board.dtbo
#
# Combined dtb target is also generated using the fdt_overlay tool.
# dtb-y += <base>-<board>.dtb
add-overlays = $(foreach o,$1,$(foreach b,$2,$(eval $(basename $b)-$(basename $o)-dtbs = $b $o) $(basename $b)-$(basename $o).dtb))

# khaje.dtb
KHAJE_BASE_DTB += ../qcom/khaje.dtb

OPLUS_KHAJE_BOARDS += \
		muses-22341-khaje-qrd-nopmi-overlay.dtbo \
		muses-22342-khaje-qrd-nopmi-overlay.dtbo \
		muses-22343-khaje-qrd-nopmi-overlay.dtbo \
		avatar-l4-23271-khaje-qrd-nopmi-overlay.dtbo \
		avatar-l4-23273-khaje-qrd-nopmi-overlay.dtbo \
		avatar-l4-23274-khaje-qrd-nopmi-overlay.dtbo \
		Tacoo-F-24675-khaje-qrd-nopmi-overlay.dtbo \
		Tacoo-F-24676-khaje-qrd-nopmi-overlay.dtbo \
		AMG-4G-24241-khaje-qrd-nopmi-overlay.dtbo \
		AMG-4G-24242-khaje-qrd-nopmi-overlay.dtbo

bengal-dtb-$(CONFIG_ARCH_KHAJE) += $(OPLUS_KHAJE_BOARDS) $(KHAJE_BASE_DTB)
bengal-overlays-dtb-$(CONFIG_ARCH_KHAJE) += $(OPLUS_KHAJE_BOARDS) $(KHAJE_BASE_DTB)
dtb-y += $(bengal-dtb-y)

# bengal.dtb
BENGAL_BASE_DTB += ../qcom/bengal.dtb

OPLUS_BENGAL_BOARDS += \
		Avatar-B4-23311-bengal-qrd-nopmi-overlay.dtbo \
		Avatar-B4-23317-bengal-qrd-nopmi-overlay.dtbo \
		Alpha-L4-24281-bengal-qrd-nopmi-overlay.dtbo \
		Alpha-L4-24282-bengal-qrd-nopmi-overlay.dtbo \
		Alpha-L4-24284-bengal-qrd-nopmi-overlay.dtbo

bengal-dtb-$(CONFIG_ARCH_BENGAL) += $(OPLUS_BENGAL_BOARDS) $(BENGAL_BASE_DTB)
bengal-overlays-dtb-$(CONFIG_ARCH_BENGAL) += $(OPLUS_BENGAL_BOARDS) $(BENGAL_BASE_DTB)
dtb-y += $(bengal-dtb-y)

always-y        := $(dtb-y)
subdir-y        := $(dts-dirs)
clean-files     := *.dtb *.dtbo

