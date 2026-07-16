// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2019-2021 Oplus. All rights reserved.
 */

#define pr_fmt(fmt) "%s:%s " fmt, KBUILD_MODNAME, __func__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/thermal.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/idr.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/iio/consumer.h>
#include <linux/pinctrl/consumer.h>

#include <horae_flash_led_temp.h>

int flash_led_temp_table[] = {
	-40, 1832, -39, 1829, -38, 1825, -37, 1822, -36, 1818, -35, 1814, -34, 1810, -33, 1806, -32, 1801, -31, 1796,
	-30, 1791, -29, 1785, -28, 1779, -27, 1773, -26, 1767, -25, 1760, -24, 1753, -23, 1746, -22, 1738, -21, 1730,
	-20, 1721, -19, 1712, -18, 1703, -17, 1693, -16, 1683, -15, 1672, -14, 1661, -13, 1650, -12, 1638, -11, 1625,
	-10, 1613, -9, 1599, -8, 1586, -7, 1571, -6, 1557, -5, 1542, -4, 1526, -3, 1510, -2, 1494, -1, 1477,
	0, 1460, 1, 1442, 2, 1424, 3, 1405, 4, 1386, 5, 1367, 6, 1348, 7, 1328, 8, 1308, 9, 1287,
	10, 1267, 11, 1246, 12, 1225, 13, 1203, 14, 1182, 15, 1160, 16, 1139, 17, 1117, 18, 1095, 19, 1073,
	20, 1051, 21, 1029, 22, 1007, 23, 986, 24, 964, 25, 942, 26, 920, 27, 899, 28, 877, 29, 856,
	30, 835, 31, 814, 32, 794, 33, 773, 34, 753, 35, 733, 36, 714, 37, 695, 38, 676, 39, 658,
	40, 640, 41, 622, 42, 605, 43, 588, 44, 572, 45, 556, 46, 540, 47, 525, 48, 510, 49, 495,
	50, 481, 51, 467, 52, 453, 53, 440, 54, 428, 55, 415, 56, 403, 57, 392, 58, 380, 59, 369,
	60, 359, 61, 348, 62, 338, 63, 329, 64, 319, 65, 310, 66, 301, 67, 293, 68, 284, 69, 276,
	70, 269, 71, 261, 72, 254, 73, 247, 74, 240, 75, 234, 76, 227, 77, 221, 78, 215, 79, 210,
	80, 204, 81, 199, 82, 193, 83, 188, 84, 184, 85, 179, 86, 174, 87, 170, 88, 166, 89, 162,
	90, 158, 91, 154, 92, 150, 93, 147, 94, 144, 95, 140, 96, 137, 97, 134, 98, 131, 99, 128,
	100, 125, 101, 123, 102, 120, 103, 117, 104, 115, 105, 113, 106, 110, 107, 108, 108, 106, 109, 104,
	110, 102, 111, 100, 112, 98, 113, 96, 114, 95, 115, 93, 116, 91, 117, 90, 118, 88, 119, 87,
	120, 85, 121, 84, 122, 82, 123, 81, 124, 80, 125, 79,
};

struct flash_led_thermal *g_chip;
static void oplus_ntc_switch_init(struct flash_led_thermal *chip);

void oplus_ntc_switch_set_lock(struct flash_led_thermal *chip, bool lock)
{
	if (!chip) {
		pr_err("chip is NULL\n");
		return;
	}

	mutex_lock(&chip->ntc_switch_gpio.ntc_lock);
	chip->ntc_switch_gpio.ntc_switch = lock;
	mutex_unlock(&chip->ntc_switch_gpio.ntc_lock);
}

void oplus_ntc_switch_get_lock(struct flash_led_thermal *chip, bool *lock)
{
	if (!chip) {
		pr_err("chip is NULL\n");
		return;
	}

	mutex_lock(&chip->ntc_switch_gpio.ntc_lock);
	*lock = chip->ntc_switch_gpio.ntc_switch;
	mutex_unlock(&chip->ntc_switch_gpio.ntc_lock);
}

static void oplus_ntc_switch(enum ntc_switch_type switch_type, struct flash_led_thermal *chip)
{
	if (!chip) {
		pr_err("invalid chip");
		return;
	}

	if (IS_ERR_OR_NULL(chip->ntc_switch_gpio.pinctrl_gpio)) {
		pr_err("invalid pinctrl_gpio, try reinit");
		oplus_ntc_switch_init(chip);
		if (IS_ERR_OR_NULL(chip->ntc_switch_gpio.pinctrl_gpio)) {
			pr_err("reinit pinctrl_gpio failed");
			return;
		}
	}

	switch (switch_type) {
	case USBTEMP_L:
		if (IS_ERR_OR_NULL(chip->ntc_switch_gpio.ntc_switch_usbtemp_l)) {
			pr_err("invalid ntc_switch_usbtemp_l");
			return;
		}
		pinctrl_select_state(chip->ntc_switch_gpio.pinctrl_gpio, chip->ntc_switch_gpio.ntc_switch_usbtemp_l);
		break;
	case USBTEMP_R:
		if (IS_ERR_OR_NULL(chip->ntc_switch_gpio.ntc_switch_usbtemp_r)) {
			pr_err("invalid ntc_switch_usbtemp_r");
			return;
		}
		pinctrl_select_state(chip->ntc_switch_gpio.pinctrl_gpio, chip->ntc_switch_gpio.ntc_switch_usbtemp_r);
		break;
	case FLASH_LED:
		if (IS_ERR_OR_NULL(chip->ntc_switch_gpio.ntc_switch_flash_led)) {
			pr_err("invalid ntc_switch_flash_led");
			return;
		}
		pinctrl_select_state(chip->ntc_switch_gpio.pinctrl_gpio, chip->ntc_switch_gpio.ntc_switch_flash_led);
		break;
	case SUBBOARD:
		if (IS_ERR_OR_NULL(chip->ntc_switch_gpio.ntc_switch_subboard)) {
			pr_err("invalid ntc_switch_subboard");
			return;
		}
		pinctrl_select_state(chip->ntc_switch_gpio.pinctrl_gpio, chip->ntc_switch_gpio.ntc_switch_subboard);
		break;
	default:
		break;
	}
}

bool oplus_set_ntc_switch_lock(enum ntc_switch_type type, bool lock)
{
	bool ret = false;
	bool lock_state;

	if (IS_ERR_OR_NULL(g_chip)) {
		pr_err("get chip fail");

	} else {
		if (!lock) {
			oplus_ntc_switch_set_lock(g_chip, false);
			return true;
		}

		oplus_ntc_switch_get_lock(g_chip, &lock_state);
		while (lock_state) {
			mdelay(3);
			oplus_ntc_switch_get_lock(g_chip, &lock_state);
		}
		oplus_ntc_switch_set_lock(g_chip, true);
		oplus_ntc_switch(type, g_chip);
		ret = true;
	}
	return ret;
}
EXPORT_SYMBOL(oplus_set_ntc_switch_lock);

static int horae_vol_convert_temp(int volt)
{
	int i;
	int size;

	size = NTC_TEMP_COUNT - 1;
	for (i = 1; i <= size; (i = i + 2)) {
		if (i >= size || flash_led_temp_table[i] <= volt)
			break;
	}
	if (i > size)
		i = size;

	i = i - 1;

	return flash_led_temp_table[i] * 1000;
}

static void oplus_ntc_switch_init(struct flash_led_thermal *chip)
{
	if (!chip) {
		pr_err("chip not ready!\n");
		return;
	}

	chip->ntc_switch_gpio.pinctrl_gpio = devm_pinctrl_get(chip->dev);
	if (IS_ERR_OR_NULL(chip->ntc_switch_gpio.pinctrl_gpio)) {
		pr_err("get pinctrl_gpio pinctrl fail\n");
		return;
	}

	chip->ntc_switch_gpio.ntc_switch_usbtemp_l =
	pinctrl_lookup_state(chip->ntc_switch_gpio.pinctrl_gpio, "ntc_switch_usbtemp_l");
	if (IS_ERR_OR_NULL(chip->ntc_switch_gpio.ntc_switch_usbtemp_l)) {
		pr_err("get ntc_switch_usbtemp_l fail\n");
	}

	chip->ntc_switch_gpio.ntc_switch_usbtemp_r =
	pinctrl_lookup_state(chip->ntc_switch_gpio.pinctrl_gpio, "ntc_switch_usbtemp_r");
	if (IS_ERR_OR_NULL(chip->ntc_switch_gpio.ntc_switch_usbtemp_r)) {
		pr_err("get ntc_switch_usbtemp_r fail\n");
	}

	chip->ntc_switch_gpio.ntc_switch_flash_led =
	pinctrl_lookup_state(chip->ntc_switch_gpio.pinctrl_gpio, "ntc_switch_flash_led");
	if (IS_ERR_OR_NULL(chip->ntc_switch_gpio.ntc_switch_flash_led)) {
		pr_err("get ntc_switch_flash_led fail\n");
	}

	chip->ntc_switch_gpio.ntc_switch_subboard =
	pinctrl_lookup_state(chip->ntc_switch_gpio.pinctrl_gpio, "ntc_switch_subboard");
	if (IS_ERR_OR_NULL(chip->ntc_switch_gpio.ntc_switch_subboard)) {
		pr_err("get ntc_switch_subboard fail\n");
	}
}

int horae_get_flash_led_temp(struct thermal_zone_device *tz, int *temp)
{
	int flash_led_volt = 0;
	int rc = 0;
	bool ret;
	struct flash_led_thermal *chip;

	chip = (struct flash_led_thermal *)tz->devdata;
	if (chip) {
		if (IS_ERR_OR_NULL(chip->ntc_switch_gpio.flash_led_adc)) {
			*temp = NTC_TEMP_DEFAULT;
			pr_err("flash_led_adc is null\n");
			return 0;
		}
		ret = oplus_set_ntc_switch_lock(FLASH_LED, true);
		if (ret) {
			mdelay(10);
			rc = iio_read_channel_processed(chip->ntc_switch_gpio.flash_led_adc, &flash_led_volt);
			oplus_set_ntc_switch_lock(FLASH_LED, false);
			if (rc >= 0) {
				flash_led_volt /= 1000;
				*temp = horae_vol_convert_temp(flash_led_volt);
			} else {
				*temp = NTC_TEMP_DEFAULT;
				pr_err("iio_read fail\n");
			}
		} else {
			*temp = NTC_TEMP_DEFAULT;
			pr_err("ntc switch fail\n");
		}
	} else {
		*temp = NTC_TEMP_DEFAULT;
		pr_err("chip is null\n");
	}
	pr_err("flash_led_volt:%d, temp:%d,\n", flash_led_volt, *temp);
	return 0;
}

static struct thermal_zone_device_ops flash_led_temp_ops = {
	.get_temp = horae_get_flash_led_temp,
};

static void register_flash_led_thermal(struct flash_led_thermal *chip)
{
	int ret = 0;
	struct thermal_zone_device *tz_dev;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0))
	tz_dev = thermal_tripless_zone_device_register("flash_led_temp", chip, &flash_led_temp_ops, NULL);
#else
	tz_dev = thermal_zone_device_register("flash_led_temp", 0, 0, chip, &flash_led_temp_ops, NULL, 0, 0);
#endif
	if (IS_ERR(tz_dev)) {
		pr_err("flash_led_temp register fail");
		return;
	}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
	ret = thermal_zone_device_enable(tz_dev);
	if (ret) {
		thermal_zone_device_unregister(tz_dev);
		return;
	}
#endif

	chip->tz_dev = tz_dev;
	pr_err("register_flash_led_thermal success\n");
}

static const struct of_device_id horae_flash_led_of_match[] = {
	{.compatible = "oplus,flash-led-temp"},
	{},
};

static int horae_flash_led_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *dev_node = dev->of_node;
	struct flash_led_thermal *chip;
	int ret = 0;

	pr_err("get horae_flash_led_probe enter.\n");
	chip = kzalloc(sizeof(struct flash_led_thermal), GFP_KERNEL);
	if (!chip) {
		pr_err("alloc flash_led mem failed\n");
		return -ENOMEM;
	}
	chip->dev = &pdev->dev;
	platform_set_drvdata(pdev, chip);

	ret = of_property_match_string(dev_node, "io-channel-names", "flash_led_temp_adc");
	if (ret >= 0) {
		chip->ntc_switch_gpio.flash_led_adc = iio_channel_get(chip->dev, "flash_led_temp_adc");
		if (IS_ERR_OR_NULL(chip->ntc_switch_gpio.flash_led_adc)) {
			ret = PTR_ERR(chip->ntc_switch_gpio.flash_led_adc);
			pr_err("flash_led_adc get error, ret=%d\n", ret);
			chip->ntc_switch_gpio.flash_led_adc = NULL;
			kfree(chip);
			return -EPROBE_DEFER;
		}
	}
	g_chip = chip;
	mutex_init(&chip->ntc_switch_gpio.ntc_lock);
	oplus_ntc_switch_init(chip);
	register_flash_led_thermal(chip);
	pr_err("horae_flash_led_probe end\n");

	return 0;
}

static int horae_flash_led_remove(struct platform_device *pdev)
{
	struct flash_led_thermal *chip = platform_get_drvdata(pdev);

	if (chip) {
		platform_set_drvdata(pdev, NULL);
		thermal_zone_device_unregister(chip->tz_dev);
		if (!IS_ERR_OR_NULL(chip->ntc_switch_gpio.flash_led_adc)) {
			iio_channel_release(chip->ntc_switch_gpio.flash_led_adc);
			chip->ntc_switch_gpio.flash_led_adc = NULL;
		}
		if (g_chip == chip) {
			g_chip = NULL;
		}
		kfree(chip);
	}
	return 0;
}

static struct platform_driver horae_flash_led_platdrv = {
	.driver = {
		.name		= "horae-flash-led-temp",
		.owner		= THIS_MODULE,
		.of_match_table = horae_flash_led_of_match,
	},
	.probe	= horae_flash_led_probe,
	.remove = horae_flash_led_remove,
};

static int __init horae_flash_led_init(void)
{
	return platform_driver_register(&horae_flash_led_platdrv);
}

static void __exit horae_flash_led_exit(void)
{
	platform_driver_unregister(&horae_flash_led_platdrv);
}

module_init(horae_flash_led_init);
module_exit(horae_flash_led_exit);
MODULE_LICENSE("GPL v2");
