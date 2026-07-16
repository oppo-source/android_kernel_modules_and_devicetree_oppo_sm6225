/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2025 Oplus. All rights reserved.
 */


#ifndef _HORAE_FLASH_LED_TEMP_H
#define _HORAE_FLASH_LED_TEMP_H

#define NTC_TEMP_COUNT 332
#define NTC_TEMP_DEFAULT 25000
#define NTC_TEMP_CONVERSION 1000

enum ntc_switch_type {
	USBTEMP_L,
	USBTEMP_R,
	FLASH_LED,
	SUBBOARD,
};

struct oplus_ntc_switch_gpio {
	bool ntc_switch;
	struct mutex ntc_lock;
	struct pinctrl *pinctrl_gpio;
	struct pinctrl_state *ntc_switch_usbtemp_l;
	struct pinctrl_state *ntc_switch_usbtemp_r;
	struct pinctrl_state *ntc_switch_flash_led;
	struct pinctrl_state *ntc_switch_subboard;

	struct iio_channel *flash_led_adc;
	int other_temp_table[NTC_TEMP_COUNT];
	int subboard_temp_table[NTC_TEMP_COUNT];
};

struct flash_led_thermal {
	struct device *dev;
	struct thermal_zone_device *tz_dev;
	struct oplus_ntc_switch_gpio ntc_switch_gpio;
};

void oplus_ntc_switch_get_lock(struct flash_led_thermal *chip, bool *lock);
void oplus_ntc_switch_set_lock(struct flash_led_thermal *chip, bool lock);
bool oplus_set_ntc_switch_lock(enum ntc_switch_type type, bool lock);

#endif /* _HORAE_FLASH_LED_TEMP_H */
