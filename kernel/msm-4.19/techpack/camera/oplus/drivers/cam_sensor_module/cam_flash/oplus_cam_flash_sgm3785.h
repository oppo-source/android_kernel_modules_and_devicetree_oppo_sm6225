#ifndef OPLUS_CAM_FLASH_SGM3785_H
#define OPLUS_CAM_FLASH_SGM3785_H

#include <linux/module.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/timer.h>

#include "cam_sensor_cmn_header.h"
#include "cam_flash_core.h"
#include "cam_res_mgr_api.h"
#include "cam_common_util.h"
#include "cam_packet_util.h"

int cam_flash_pwm_clk_enable_sgm3785(struct cam_flash_ctrl *flash_ctrl, int flash_duty);
void cam_flash_pwm_clk_disable_sgm3785(void);
int cam_flash_pwm_selected_sgm3785(struct cam_flash_ctrl *flash_ctrl, bool is_pwm_selected);
int cam_flash_gpio_get_level_sgm3785(int flash_current);
int cam_flash_pwm_get_duty(int flash_current);
int cam_flash_gpio_off_sgm3785(struct cam_flash_ctrl *flash_ctrl);
int cam_flash_gpio_high_sgm3785(struct cam_flash_ctrl *flash_ctrl, struct cam_flash_frame_setting *flash_data);
int cam_flash_gpio_low_sgm3785(struct cam_flash_ctrl *flash_ctrl, struct cam_flash_frame_setting *flash_data);
int cam_flash_gpio_flush_request_sgm3785(struct cam_flash_ctrl *fctrl,enum cam_flash_flush_type type, uint64_t req_id);
int cam_flash_gpio_power_on_sgm3785(struct cam_sensor_power_ctrl_t *ctrl,
		struct cam_hw_soc_info *soc_info);
int cam_flash_gpio_power_down_sgm3785(struct cam_sensor_power_ctrl_t *ctrl,
		struct cam_hw_soc_info *soc_info);
int cam_flash_gpio_power_ops_sgm3785(struct cam_flash_ctrl *fctrl,bool regulator_enable);
int cam_flash_gpio_pkt_parser_sgm3785(struct cam_flash_ctrl *fctrl, void *arg);
int cam_flash_gpio_delete_req_sgm3785(struct cam_flash_ctrl *fctrl,
	uint64_t req_id);
int cam_flash_gpio_apply_setting_sgm3785(struct cam_flash_ctrl *fctrl,
		uint64_t req_id);

#endif //OPLUS_CAM_FLASH_SGM3785_H
