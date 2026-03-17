# add-overlay defines the target with following naming convention:
# <base>-<board>-dtbs = base.dtb board.dtbo
#
# Combined dtb target is also generated using the fdt_overlay tool.
# dtb-y += <base>-<board>.dtb
add-overlays = $(foreach o,$1,$(foreach b,$2,$(eval $(basename $b)-$(basename $o)-dtbs = $b $o) $(basename $b)-$(basename $o).dtb))

# khaje.dtb
KHAJE_BASE_DTB += ../qcom/khaje.dtb

OPLUS_KHAJE_BOARDS += \
		BB4_25251-khaje-qrd-nopmi-overlay.dtbo \
		BB4_25253-khaje-qrd-nopmi-overlay.dtbo \
		BB4_25253-khaje-qrd-nopmi-overlay-gdpr.dtbo \
		Baikal-B4-25031-khaje-qrd-nopmi-overlay.dtbo \
		Baikal-B4-25258-khaje-qrd-nopmi-overlay.dtbo \
		Baikal-B4-25258-khaje-qrd-nopmi-overlay-gdpr.dtbo \
		Baikal-B4-25243-khaje-qrd-nopmi-overlay.dtbo \
		Baikal-B4-25243-khaje-qrd-nopmi-overlay-gdpr.dtbo


bengal-dtb-$(CONFIG_ARCH_KHAJE) += $(OPLUS_KHAJE_BOARDS) $(KHAJE_BASE_DTB)
bengal-overlays-dtb-$(CONFIG_ARCH_KHAJE) += $(OPLUS_KHAJE_BOARDS) $(KHAJE_BASE_DTB)
dtb-y += $(bengal-dtb-y)

# bengal.dtb
BENGAL_BASE_DTB += ../qcom/bengal.dtb

bengal-dtb-$(CONFIG_ARCH_BENGAL) += $(OPLUS_BENGAL_BOARDS) $(BENGAL_BASE_DTB)
bengal-overlays-dtb-$(CONFIG_ARCH_BENGAL) += $(OPLUS_BENGAL_BOARDS) $(BENGAL_BASE_DTB)
dtb-y += $(bengal-dtb-y)

always-y        := $(dtb-y)
subdir-y        := $(dts-dirs)
clean-files     := *.dtb *.dtbo

