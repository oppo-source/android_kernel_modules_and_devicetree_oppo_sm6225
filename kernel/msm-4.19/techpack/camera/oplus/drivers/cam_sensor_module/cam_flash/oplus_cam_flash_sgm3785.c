// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2017-2021, The Linux Foundation. All rights reserved.
 */
#include "oplus_cam_flash_sgm3785.h"

#define PWM_FRQ 48000
#define PWM_DEN 100
#define MAX_FLASH_CURRENT 1400
#define MAX_TORCH_CURRENT 200
#define FLASH_MODE 1
#define TORCH_MODE 2
#define TORCH_SET_DUTY 50
#define TORCH_TRUE_DUTY 55

static struct clk *pwm_clk = NULL;
static int sgm3785_flash_mode = 0;

int cam_flash_pwm_clk_enable_sgm3785(struct cam_flash_ctrl *flash_ctrl, int flash_duty)
{
	int ret = 0;
	struct cam_hw_soc_info  soc_info = flash_ctrl->soc_info;

	pwm_clk = devm_clk_get(soc_info.dev, soc_info.clk_name[0]);
	if (NULL == pwm_clk) {
		CAM_ERR(CAM_FLASH, "get pclk(gpio-pwm-clk) error");
		return -EINVAL;
	}

	ret = cam_soc_util_clk_enable(pwm_clk, soc_info.clk_name[0], PWM_FRQ);
	if (ret) {
		CAM_ERR(CAM_FLASH, "clk prepare and enable fail");
		return ret;
	} else {
		CAM_DBG(CAM_FLASH, "clk prepare and enable success");
	}

	ret = cam_soc_util_set_clk_duty_cycle(pwm_clk, soc_info.clk_name[0], flash_duty, PWM_DEN);
	if (ret) {
		CAM_ERR(CAM_FLASH, "clk duty cycle set fail");
		return ret;
	}
	return ret;
}

void cam_flash_pwm_clk_disable_sgm3785()
{
	if (pwm_clk == NULL) {
		return;
	} else {
		clk_disable_unprepare(pwm_clk);
	}
}

int cam_flash_pwm_selected_sgm3785(struct cam_flash_ctrl *flash_ctrl, bool is_pwm_selected)
{
	int ret;

	if (is_pwm_selected) {
		ret = pinctrl_select_state(flash_ctrl->power_info.pinctrl_info.pinctrl,
			flash_ctrl->power_info.pinctrl_info.pwm_state_active);
		if (ret)
			CAM_ERR(CAM_FLASH, "cannot set pin to pwm active state");
	} else {
		ret = pinctrl_select_state(flash_ctrl->power_info.pinctrl_info.pinctrl,
			flash_ctrl->power_info.pinctrl_info.gpio_state_active);
		if (ret)
			CAM_ERR(CAM_FLASH, "cannot set pin to gpio active state");
	}
	pr_info("is_pwm_selected:%d ret:%d\n", is_pwm_selected, ret);
	return ret;
}

int cam_flash_pwm_get_duty(int flash_current)
{
	int pwm_duty = 0;
	CAM_INFO(CAM_FLASH,"sgm3785_flash_mode is %d",sgm3785_flash_mode);
	if (sgm3785_flash_mode == FLASH_MODE) {
		pwm_duty = flash_current*100/MAX_FLASH_CURRENT;
		if (pwm_duty > 100)
		pwm_duty = 100;
	}
	if (sgm3785_flash_mode == TORCH_MODE) {
		pwm_duty = flash_current*100/MAX_TORCH_CURRENT;
		if (pwm_duty > 100) {
		pwm_duty = 100;
		}
		else if (pwm_duty == TORCH_SET_DUTY) {
		pwm_duty = TORCH_TRUE_DUTY;
		}
	}
	sgm3785_flash_mode = 0;
	CAM_INFO(CAM_FLASH,"flash_current is %d",flash_current);
	CAM_INFO(CAM_FLASH,"pwm_duty is %d",pwm_duty);
	return pwm_duty;
}

int cam_flash_gpio_off_sgm3785(struct cam_flash_ctrl *flash_ctrl)
{
	struct cam_hw_soc_info  soc_info = flash_ctrl->soc_info;
	struct msm_camera_gpio_num_info *gpio_num_info = NULL;
	gpio_num_info = flash_ctrl->power_info.gpio_num_info;
	sgm3785_flash_mode = 0;

	if (!flash_ctrl || !gpio_num_info) {
		CAM_ERR(CAM_FLASH, "Flash control Null");
		return -EINVAL;
	}

	flash_ctrl->flash_state = CAM_FLASH_STATE_START;
	CAM_INFO(CAM_FLASH,"cam_flash_gpio_off_sgm3785");
	if (gpio_num_info->valid[SENSOR_CUSTOM_GPIO1]) {
		cam_res_mgr_gpio_set_value(gpio_num_info->gpio_num[SENSOR_CUSTOM_GPIO1], 0);
		cam_res_mgr_gpio_free(soc_info.dev, gpio_num_info->gpio_num[SENSOR_CUSTOM_GPIO1]);
	}

	if (gpio_num_info->valid[SENSOR_CUSTOM_GPIO2]) {
		cam_res_mgr_gpio_set_value(gpio_num_info->gpio_num[SENSOR_CUSTOM_GPIO2], 0);
		cam_res_mgr_gpio_free(soc_info.dev, gpio_num_info->gpio_num[SENSOR_CUSTOM_GPIO2]);
	}
	cam_flash_pwm_clk_disable_sgm3785();
	return 0;
}

int cam_flash_gpio_high_sgm3785(struct cam_flash_ctrl *flash_ctrl,
	struct cam_flash_frame_setting *flash_data)
{
	int rc = 0;
	struct cam_hw_soc_info  soc_info = flash_ctrl->soc_info;
	struct msm_camera_gpio_num_info *gpio_num_info = NULL;
	int pwm_duty = 0;
	gpio_num_info = flash_ctrl->power_info.gpio_num_info;
	sgm3785_flash_mode = FLASH_MODE;

	if (!flash_data || !gpio_num_info) {
		CAM_ERR(CAM_FLASH, "Flash Data Null");
		return -EINVAL;
	}
	CAM_INFO(CAM_FLASH, "Flash high Triggered");
	rc = cam_flash_pwm_selected_sgm3785(flash_ctrl, true);
	if (rc) {
		CAM_ERR(CAM_FLASH, "flash select pwm failed");
		return rc;
	}
	pwm_duty = cam_flash_pwm_get_duty(flash_data->led_current_ma[0]);
		rc = cam_flash_pwm_clk_enable_sgm3785(flash_ctrl, pwm_duty);
	if(rc) {
		CAM_ERR(CAM_FLASH, "pwm clk enable set fail");
		return rc;
	}

	if (gpio_num_info->valid[SENSOR_CUSTOM_GPIO1]) {
		rc = cam_res_mgr_gpio_request(soc_info.dev, gpio_num_info->gpio_num[SENSOR_CUSTOM_GPIO1], 0, "CUSTOM_GPIO1");
		if(rc) {
			CAM_ERR(CAM_FLASH, "flash_en_gpio %d request fails", gpio_num_info->gpio_num[SENSOR_CUSTOM_GPIO1]);
			return rc;
		}
		cam_res_mgr_gpio_set_value(gpio_num_info->gpio_num[SENSOR_CUSTOM_GPIO1], 1);
	}

	return rc;
}

int cam_flash_gpio_low_sgm3785(struct cam_flash_ctrl *flash_ctrl,struct cam_flash_frame_setting *flash_data)
{
	int rc = 0;
	int pwm_duty = 0;
	int flash_current = 0;
	struct cam_hw_soc_info soc_info = flash_ctrl->soc_info;
	struct msm_camera_gpio_num_info *gpio_num_info = NULL;
	gpio_num_info = flash_ctrl->power_info.gpio_num_info;
	sgm3785_flash_mode = TORCH_MODE;
	if (!flash_data || !gpio_num_info) {
		CAM_ERR(CAM_FLASH, "Flash Data  is Null");
		return -EINVAL;
	}
	flash_current = flash_data->led_current_ma[0];
	pwm_duty = cam_flash_pwm_get_duty(flash_current);
	rc = cam_flash_pwm_clk_enable_sgm3785(flash_ctrl, pwm_duty);
	if (rc) {
		CAM_ERR(CAM_FLASH, "pwm clk enable set fail");
		return rc;
	}

	rc = cam_flash_pwm_selected_sgm3785(flash_ctrl, false);
	if (rc) {
		CAM_ERR(CAM_FLASH, "flash select gpio failed");
		return rc;
	}
	CAM_DBG(CAM_FLASH, "Flash low en flash_torch_gpio %d, valid = %d",gpio_num_info->gpio_num[SENSOR_CUSTOM_GPIO2],gpio_num_info->valid[SENSOR_CUSTOM_GPIO2]);

	if (gpio_num_info->valid[SENSOR_CUSTOM_GPIO2]) {
		rc = cam_res_mgr_gpio_request(soc_info.dev, gpio_num_info->gpio_num[SENSOR_CUSTOM_GPIO2], 0, "CUSTOM_GPIO2");
		if(rc) {
			CAM_ERR(CAM_FLASH, "flash_torch_gpio %d request fails", gpio_num_info->gpio_num[SENSOR_CUSTOM_GPIO2]);
			return rc;
		}
		cam_res_mgr_gpio_set_value(gpio_num_info->gpio_num[SENSOR_CUSTOM_GPIO2],1);
		mdelay(5);
	}

	rc = cam_flash_pwm_selected_sgm3785(flash_ctrl, true);
	if (rc) {
		CAM_ERR(CAM_FLASH, "flash select pwm failed");
		return rc;
	}
	return rc;
}

int cam_flash_gpio_flush_request_sgm3785(struct cam_flash_ctrl *fctrl,enum cam_flash_flush_type type, uint64_t req_id) {
	int rc = 0;
	int i = 0, j = 0;
	int frame_offset = 0;
	bool is_off_needed = false;
	struct cam_flash_frame_setting *flash_data = NULL;
	CAM_DBG(CAM_FLASH, "cam_flash_gpio");
	if (!fctrl) {
		CAM_ERR(CAM_FLASH, "Device data is NULL");
		return -EINVAL;
	}

	if (type == FLUSH_ALL) {
	/* flush all requests*/
		for (i = 0; i < MAX_PER_FRAME_ARRAY; i++) {
			flash_data =
				&fctrl->per_frame[i];
			if ((flash_data->opcode ==
				CAMERA_SENSOR_FLASH_OP_OFF) &&
				(flash_data->cmn_attr.request_id > 0) &&
				(flash_data->cmn_attr.request_id <= req_id) &&
				flash_data->cmn_attr.is_settings_valid) {
				is_off_needed = true;
				CAM_DBG(CAM_FLASH,
					"FLASH_ALL: Turn off the flash for req %llu",
					flash_data->cmn_attr.request_id);
			}

			flash_data->cmn_attr.request_id = 0;
			flash_data->cmn_attr.is_settings_valid = false;
			flash_data->cmn_attr.count = 0;
			for (j = 0; j < CAM_FLASH_MAX_LED_TRIGGERS; j++)
				flash_data->led_current_ma[j] = 0;
		}

	} else if ((type == FLUSH_REQ) && (req_id != 0)) {
	/* flush request with req_id*/
		frame_offset = req_id % MAX_PER_FRAME_ARRAY;
		flash_data =
			&fctrl->per_frame[frame_offset];

		if (flash_data->opcode ==
			CAMERA_SENSOR_FLASH_OP_OFF) {
			is_off_needed = true;
			CAM_DBG(CAM_FLASH,
				"FLASH_REQ: Turn off the flash for req %llu",
				flash_data->cmn_attr.request_id);
		}

		flash_data->cmn_attr.request_id = 0;
		flash_data->cmn_attr.is_settings_valid = false;
		flash_data->cmn_attr.count = 0;
		for (i = 0; i < CAM_FLASH_MAX_LED_TRIGGERS; i++)
			flash_data->led_current_ma[i] = 0;
	} else if ((type == FLUSH_REQ) && (req_id == 0)) {
		/* Handels NonRealTime usecase */
	} else {
		CAM_ERR(CAM_FLASH, "Invalid arguments");
		return -EINVAL;
	}

	if (is_off_needed)
		cam_flash_off(fctrl);

	return rc;
}

int cam_flash_gpio_power_on_sgm3785(struct cam_sensor_power_ctrl_t *ctrl,
		struct cam_hw_soc_info *soc_info)
{
	int rc = 0;

	CAM_DBG(CAM_FLASH, "Enter");
	if (!ctrl) {
		CAM_ERR(CAM_FLASH, "Invalid ctrl handle");
		return -EINVAL;
	}

	rc = msm_flash_pinctrl_init(&(ctrl->pinctrl_info), ctrl->dev);
	if (rc < 0) {
		/* Some sensor subdev no pinctrl. */
		CAM_ERR(CAM_FLASH, "Initialization of pinctrl failed");
		ctrl->cam_pinctrl_status = 0;
	} else {
		ctrl->cam_pinctrl_status = 1;
	}

	if (ctrl->cam_pinctrl_status) {
		rc = pinctrl_select_state(ctrl->pinctrl_info.pinctrl,ctrl->pinctrl_info.gpio_state_active);
		if (rc)
			CAM_ERR(CAM_FLASH, "cannot set pin to active state");
	}

	return rc;
}

int cam_flash_gpio_power_down_sgm3785(struct cam_sensor_power_ctrl_t *ctrl,
		struct cam_hw_soc_info *soc_info)
{
	int  ret = 0;

	CAM_DBG(CAM_FLASH, "Enter");
	if (!ctrl || !soc_info) {
		CAM_ERR(CAM_FLASH, "failed ctrl %pK",  ctrl);
		return -EINVAL;
	}

	if (ctrl->cam_pinctrl_status) {
		ret = pinctrl_select_state(
			ctrl->pinctrl_info.pinctrl,
			ctrl->pinctrl_info.gpio_state_suspend);
		if (ret)
			CAM_ERR(CAM_FLASH, "cannot set pin to suspend state");

		devm_pinctrl_put(ctrl->pinctrl_info.pinctrl);
	}

	ctrl->cam_pinctrl_status = 0;
	return ret;
}

int cam_flash_gpio_power_ops_sgm3785(struct cam_flash_ctrl *fctrl,bool regulator_enable) {
	int rc = 0;
	struct cam_hw_soc_info *soc_info = &fctrl->soc_info;
	struct cam_sensor_power_ctrl_t *power_info =&fctrl->power_info;
	static int init_inistance = 0;
	if (init_inistance > 0)
		return rc;
	init_inistance = 1;
	if (!power_info || !soc_info) {
		CAM_ERR(CAM_FLASH, "Power Info is NULL");
		return -EINVAL;
	}
	power_info->dev = soc_info->dev;
	CAM_INFO(CAM_FLASH,"regulator_enable = %d,fctrl->is_regulator_enabled=%d",regulator_enable,fctrl->is_regulator_enabled);

	if (regulator_enable && (fctrl->is_regulator_enabled == false)) {
		rc = cam_flash_gpio_power_on_sgm3785(power_info, soc_info);
		if (rc) {
			CAM_ERR(CAM_FLASH, "power up the core is failed:%d",rc);
		}
		fctrl->is_regulator_enabled = true;
	} else if ((!regulator_enable) && (fctrl->is_regulator_enabled == true)) {
		rc = cam_flash_gpio_power_down_sgm3785(power_info, soc_info);
		if (rc) {
			CAM_ERR(CAM_FLASH, "power down the core is failed:%d",rc);
			return rc;
		}
		fctrl->is_regulator_enabled = false;
	}
	return rc;
}

int cam_flash_gpio_pkt_parser_sgm3785(struct cam_flash_ctrl *fctrl, void *arg) {
	int rc = 0, i = 0;
	uintptr_t generic_ptr, cmd_buf_ptr;
	uint32_t *cmd_buf =  NULL;
	uint32_t *offset = NULL;
	uint32_t frm_offset = 0;
	size_t len_of_buffer;
	size_t remain_len;
	struct cam_control *ioctl_ctrl = NULL;
	struct cam_packet *csl_packet = NULL;
	struct cam_cmd_buf_desc *cmd_desc = NULL;
	struct common_header *cmn_hdr;
	struct cam_config_dev_cmd config;
	struct cam_req_mgr_add_request add_req = {0};
	struct cam_flash_init *cam_flash_info = NULL;
	struct cam_flash_set_rer *flash_rer_info = NULL;
	struct cam_flash_set_on_off *flash_operation_info = NULL;
	struct cam_flash_query_curr *flash_query_info = NULL;
	struct cam_flash_frame_setting *flash_data = NULL;
	struct cam_flash_private_soc *soc_private = NULL;
	CAM_DBG(CAM_FLASH, "cam_flash_gpio");
	if (!fctrl || !arg) {
		CAM_ERR(CAM_FLASH, "fctrl/arg is NULL");
		return -EINVAL;
	}

	soc_private = (struct cam_flash_private_soc *)
	fctrl->soc_info.soc_private;

	/* getting CSL Packet */
	ioctl_ctrl = (struct cam_control *)arg;

	if (copy_from_user((&config),
		u64_to_user_ptr(ioctl_ctrl->handle),
		sizeof(config))) {
		CAM_ERR(CAM_FLASH, "Copy cmd handle from user failed");
		rc = -EFAULT;
		return rc;
	}

	rc = cam_mem_get_cpu_buf(config.packet_handle,
		&generic_ptr, &len_of_buffer);
	if (rc) {
		CAM_ERR(CAM_FLASH, "Failed in getting the packet: %d", rc);
		return rc;
	}

	remain_len = len_of_buffer;
	if ((sizeof(struct cam_packet) > len_of_buffer) ||
		((size_t)config.offset >= len_of_buffer -
		sizeof(struct cam_packet))) {
		CAM_ERR(CAM_FLASH,
			"Inval cam_packet strut size: %zu, len_of_buff: %zu",
			 sizeof(struct cam_packet), len_of_buffer);
		rc = -EINVAL;
		return rc;
	}

	remain_len -= (size_t)config.offset;
	/* Add offset to the flash csl header */
	csl_packet = (struct cam_packet *)(generic_ptr + config.offset);

	if (cam_packet_util_validate_packet(csl_packet,
		remain_len)) {
		CAM_ERR(CAM_FLASH, "Invalid packet params");
		rc = -EINVAL;
		return rc;
	}

	if ((csl_packet->header.op_code & 0xFFFFFF) !=
		CAM_FLASH_PACKET_OPCODE_INIT &&
		csl_packet->header.request_id <= fctrl->last_flush_req
		&& fctrl->last_flush_req != 0) {
		CAM_WARN(CAM_FLASH,
			"reject request %lld, last request to flush %d",
			csl_packet->header.request_id, fctrl->last_flush_req);
		rc = -EINVAL;
		return rc;
	}

	if (csl_packet->header.request_id > fctrl->last_flush_req)
		fctrl->last_flush_req = 0;

	switch (csl_packet->header.op_code & 0xFFFFFF) {
	case CAM_FLASH_PACKET_OPCODE_INIT: {
		/* INIT packet*/
		offset = (uint32_t *)((uint8_t *)&csl_packet->payload +
			csl_packet->cmd_buf_offset);
		cmd_desc = (struct cam_cmd_buf_desc *)(offset);
		rc = cam_mem_get_cpu_buf(cmd_desc->mem_handle,
			&cmd_buf_ptr, &len_of_buffer);
		if (rc) {
			CAM_ERR(CAM_FLASH, "Fail in get buffer: %d", rc);
			return rc;
		}
		if ((len_of_buffer < sizeof(struct cam_flash_init)) ||
			(cmd_desc->offset >
			(len_of_buffer - sizeof(struct cam_flash_init)))) {
			CAM_ERR(CAM_FLASH, "Not enough buffer");
			rc = -EINVAL;
			return rc;
		}
		remain_len = len_of_buffer - cmd_desc->offset;
		cmd_buf = (uint32_t *)((uint8_t *)cmd_buf_ptr +
			cmd_desc->offset);
		cam_flash_info = (struct cam_flash_init *)cmd_buf;

		switch (cam_flash_info->cmd_type) {
		case CAMERA_SENSOR_FLASH_CMD_TYPE_INIT_INFO: {
			CAM_DBG(CAM_FLASH, "INIT_INFO CMD CALLED");
			fctrl->flash_init_setting.cmn_attr.request_id = 0;
			fctrl->flash_init_setting.cmn_attr.is_settings_valid = true;
			fctrl->flash_type = cam_flash_info->flash_type;
			fctrl->is_regulator_enabled = false;
			fctrl->nrt_info.cmn_attr.cmd_type =
				CAMERA_SENSOR_FLASH_CMD_TYPE_INIT_INFO;

			rc = fctrl->func_tbl.power_ops(fctrl, true);
			if (rc) {
				CAM_ERR(CAM_FLASH,
					"Enable Regulator Failed rc = %d", rc);
				return rc;
			}

			fctrl->flash_state =
				CAM_FLASH_STATE_CONFIG;
			break;
		}
		case CAMERA_SENSOR_FLASH_CMD_TYPE_INIT_FIRE: {
			CAM_DBG(CAM_FLASH, "INIT_FIRE Operation");

			if (remain_len < sizeof(struct cam_flash_set_on_off)) {
				CAM_ERR(CAM_FLASH, "Not enough buffer");
				rc = -EINVAL;
				return rc;
			}

			flash_operation_info =
				(struct cam_flash_set_on_off *) cmd_buf;
			if (!flash_operation_info) {
				CAM_ERR(CAM_FLASH,
					"flash_operation_info Null");
				rc = -EINVAL;
				return rc;
			}
			if (flash_operation_info->count >
				CAM_FLASH_MAX_LED_TRIGGERS) {
				CAM_ERR(CAM_FLASH, "led count out of limit");
				rc = -EINVAL;
				return rc;
			}
			fctrl->nrt_info.cmn_attr.count =
				flash_operation_info->count;
			fctrl->nrt_info.cmn_attr.request_id = 0;
			fctrl->nrt_info.opcode =
				flash_operation_info->opcode;
			fctrl->nrt_info.cmn_attr.cmd_type =
				CAMERA_SENSOR_FLASH_CMD_TYPE_INIT_FIRE;
			for (i = 0;
				i < flash_operation_info->count; i++)
				fctrl->nrt_info.led_current_ma[i] =
				flash_operation_info->led_current_ma[i];

			rc = fctrl->func_tbl.apply_setting(fctrl, 0);
			if (rc)
				CAM_ERR(CAM_FLASH,
					"Apply setting failed: %d",
					rc);

			fctrl->flash_state = CAM_FLASH_STATE_CONFIG;
			break;
		}
		default:
			CAM_ERR(CAM_FLASH, "Wrong cmd_type = %d",
				cam_flash_info->cmd_type);
			rc = -EINVAL;
			return rc;
		}
		break;
	}
	case CAM_FLASH_PACKET_OPCODE_SET_OPS: {
		offset = (uint32_t *)((uint8_t *)&csl_packet->payload +
			csl_packet->cmd_buf_offset);
		frm_offset = csl_packet->header.request_id %
			MAX_PER_FRAME_ARRAY;
		flash_data = &fctrl->per_frame[frm_offset];

		if (flash_data->cmn_attr.is_settings_valid == true) {
			flash_data->cmn_attr.request_id = 0;
			flash_data->cmn_attr.is_settings_valid = false;
			for (i = 0; i < flash_data->cmn_attr.count; i++)
				flash_data->led_current_ma[i] = 0;
		}

		flash_data->cmn_attr.request_id = csl_packet->header.request_id;
		flash_data->cmn_attr.is_settings_valid = true;
		cmd_desc = (struct cam_cmd_buf_desc *)(offset);
		rc = cam_mem_get_cpu_buf(cmd_desc->mem_handle,
			&cmd_buf_ptr, &len_of_buffer);
		if (rc) {
			CAM_ERR(CAM_FLASH, "Fail in get buffer: 0x%x",
				cmd_desc->mem_handle);
			return rc;
		}

		if ((len_of_buffer < sizeof(struct common_header)) ||
			(cmd_desc->offset >
			(len_of_buffer - sizeof(struct common_header)))) {
			CAM_ERR(CAM_FLASH, "not enough buffer");
			rc = -EINVAL;
			return rc;
		}
		remain_len = len_of_buffer - cmd_desc->offset;

		cmd_buf = (uint32_t *)((uint8_t *)cmd_buf_ptr +
			cmd_desc->offset);
		if (!cmd_buf) {
			rc = -EINVAL;
			return rc;
		}
		cmn_hdr = (struct common_header *)cmd_buf;

		switch (cmn_hdr->cmd_type) {
		case CAMERA_SENSOR_FLASH_CMD_TYPE_FIRE: {
			CAM_DBG(CAM_FLASH,
				"CAMERA_SENSOR_FLASH_CMD_TYPE_FIRE cmd called");
			if ((fctrl->flash_state == CAM_FLASH_STATE_INIT) ||
				(fctrl->flash_state ==
					CAM_FLASH_STATE_ACQUIRE)) {
				CAM_WARN(CAM_FLASH,
					"Rxed Flash fire ops without linking");
				flash_data->cmn_attr.is_settings_valid = false;
				return -EINVAL;
			}
			if (remain_len < sizeof(struct cam_flash_set_on_off)) {
				CAM_ERR(CAM_FLASH, "Not enough buffer");
				rc = -EINVAL;
				return rc;
			}

			flash_operation_info =
				(struct cam_flash_set_on_off *) cmd_buf;
			if (!flash_operation_info) {
				CAM_ERR(CAM_FLASH,
					"flash_operation_info Null");
				rc = -EINVAL;
				return rc;
			}
			if (flash_operation_info->count >
				CAM_FLASH_MAX_LED_TRIGGERS) {
				CAM_ERR(CAM_FLASH, "led count out of limit");
				rc = -EINVAL;
				return rc;
			}

			flash_data->opcode = flash_operation_info->opcode;
			flash_data->cmn_attr.count =
				flash_operation_info->count;
			for (i = 0; i < flash_operation_info->count; i++)
				flash_data->led_current_ma[i]
				= flash_operation_info->led_current_ma[i];

			if (flash_data->opcode == CAMERA_SENSOR_FLASH_OP_OFF)
				add_req.skip_before_applying |= SKIP_NEXT_FRAME;
		}
		break;
		default:
			CAM_ERR(CAM_FLASH, "Wrong cmd_type = %d",
				cmn_hdr->cmd_type);
			rc = -EINVAL;
			return rc;
		}
		break;
	}
	case CAM_FLASH_PACKET_OPCODE_NON_REALTIME_SET_OPS: {
		offset = (uint32_t *)((uint8_t *)&csl_packet->payload +
			csl_packet->cmd_buf_offset);
		fctrl->nrt_info.cmn_attr.is_settings_valid = true;
		cmd_desc = (struct cam_cmd_buf_desc *)(offset);
		rc = cam_mem_get_cpu_buf(cmd_desc->mem_handle,
			&cmd_buf_ptr, &len_of_buffer);
		if (rc) {
			CAM_ERR(CAM_FLASH, "Fail in get buffer: %d", rc);
			return rc;
		}

		if ((len_of_buffer < sizeof(struct common_header)) ||
			(cmd_desc->offset >
			(len_of_buffer - sizeof(struct common_header)))) {
			CAM_ERR(CAM_FLASH, "Not enough buffer");
			rc = -EINVAL;
			return rc;
		}
		remain_len = len_of_buffer - cmd_desc->offset;
		cmd_buf = (uint32_t *)((uint8_t *)cmd_buf_ptr +
			cmd_desc->offset);
		cmn_hdr = (struct common_header *)cmd_buf;

		switch (cmn_hdr->cmd_type) {
		case CAMERA_SENSOR_FLASH_CMD_TYPE_WIDGET: {
			CAM_DBG(CAM_FLASH, "Widget Flash Operation");
			if (remain_len < sizeof(struct cam_flash_set_on_off)) {
				CAM_ERR(CAM_FLASH, "Not enough buffer");
				rc = -EINVAL;
				return rc;
			}
			flash_operation_info =
				(struct cam_flash_set_on_off *) cmd_buf;
			if (!flash_operation_info) {
				CAM_ERR(CAM_FLASH,
					"flash_operation_info Null");
				rc = -EINVAL;
				return rc;
			}
			if (flash_operation_info->count >
				CAM_FLASH_MAX_LED_TRIGGERS) {
				CAM_ERR(CAM_FLASH, "led count out of limit");
				rc = -EINVAL;
				return rc;
			}

			fctrl->nrt_info.cmn_attr.count =
				flash_operation_info->count;
			fctrl->nrt_info.cmn_attr.request_id = 0;
			fctrl->nrt_info.opcode =
				flash_operation_info->opcode;
			fctrl->nrt_info.cmn_attr.cmd_type =
				CAMERA_SENSOR_FLASH_CMD_TYPE_WIDGET;

			for (i = 0; i < flash_operation_info->count; i++)
				fctrl->nrt_info.led_current_ma[i] =
					flash_operation_info->led_current_ma[i];

			rc = fctrl->func_tbl.apply_setting(fctrl, 0);
			if (rc)
				CAM_ERR(CAM_FLASH, "Apply setting failed: %d",
					rc);
			return rc;
		}
		case CAMERA_SENSOR_FLASH_CMD_TYPE_QUERYCURR: {
			int query_curr_ma = 0;

			if (remain_len < sizeof(struct cam_flash_query_curr)) {
				CAM_ERR(CAM_FLASH, "Not enough buffer");
				rc = -EINVAL;
				return rc;
			}
			flash_query_info =
				(struct cam_flash_query_curr *)cmd_buf;

			if (soc_private->is_wled_flash)
				rc = wled_flash_led_prepare(
					fctrl->switch_trigger,
					QUERY_MAX_AVAIL_CURRENT,
					&query_curr_ma);
			else
				rc = qpnp_flash_led_prepare(
					fctrl->switch_trigger,
					QUERY_MAX_AVAIL_CURRENT,
					&query_curr_ma);

			CAM_DBG(CAM_FLASH, "query_curr_ma = %d",
				query_curr_ma);
			if (rc) {
				CAM_ERR(CAM_FLASH,
				"Query current failed with rc=%d", rc);
				return rc;
			}
			flash_query_info->query_current_ma = query_curr_ma;
			break;
		}
		case CAMERA_SENSOR_FLASH_CMD_TYPE_RER: {
			rc = 0;
			if (remain_len < sizeof(struct cam_flash_set_rer)) {
				CAM_ERR(CAM_FLASH, "Not enough buffer");
				rc = -EINVAL;
				return rc;
			}
			flash_rer_info = (struct cam_flash_set_rer *)cmd_buf;
			if (!flash_rer_info) {
				CAM_ERR(CAM_FLASH,
					"flash_rer_info Null");
				rc = -EINVAL;
				return rc;
			}
			if (flash_rer_info->count >
				CAM_FLASH_MAX_LED_TRIGGERS) {
				CAM_ERR(CAM_FLASH, "led count out of limit");
				rc = -EINVAL;
				return rc;
			}

			fctrl->nrt_info.cmn_attr.cmd_type =
				CAMERA_SENSOR_FLASH_CMD_TYPE_RER;
			fctrl->nrt_info.opcode = flash_rer_info->opcode;
			fctrl->nrt_info.cmn_attr.count = flash_rer_info->count;
			fctrl->nrt_info.cmn_attr.request_id = 0;
			fctrl->nrt_info.num_iterations =
				flash_rer_info->num_iteration;
			fctrl->nrt_info.led_on_delay_ms =
				flash_rer_info->led_on_delay_ms;
			fctrl->nrt_info.led_off_delay_ms =
				flash_rer_info->led_off_delay_ms;

			for (i = 0; i < flash_rer_info->count; i++)
				fctrl->nrt_info.led_current_ma[i] =
					flash_rer_info->led_current_ma[i];

			rc = fctrl->func_tbl.apply_setting(fctrl, 0);
			if (rc)
				CAM_ERR(CAM_FLASH, "apply_setting failed: %d",
					rc);
			return rc;
		}
		default:
			CAM_ERR(CAM_FLASH, "Wrong cmd_type : %d",
				cmn_hdr->cmd_type);
			rc = -EINVAL;
			return rc;
		}

		break;
	}
	case CAM_PKT_NOP_OPCODE: {
		frm_offset = csl_packet->header.request_id %
			MAX_PER_FRAME_ARRAY;
		if ((fctrl->flash_state == CAM_FLASH_STATE_INIT) ||
			(fctrl->flash_state == CAM_FLASH_STATE_ACQUIRE)) {
			CAM_WARN(CAM_FLASH,
				"Rxed NOP packets without linking");
			fctrl->per_frame[frm_offset].cmn_attr.is_settings_valid
				= false;
			return -EINVAL;
		}

		fctrl->per_frame[frm_offset].cmn_attr.is_settings_valid = false;
		fctrl->per_frame[frm_offset].cmn_attr.request_id = 0;
		fctrl->per_frame[frm_offset].opcode = CAM_PKT_NOP_OPCODE;
		CAM_DBG(CAM_FLASH, "NOP Packet is Received: req_id: %llu",
			csl_packet->header.request_id);
		break;
	}
	default:
		CAM_ERR(CAM_FLASH, "Wrong Opcode : %d",
			(csl_packet->header.op_code & 0xFFFFFF));
		rc = -EINVAL;
		return rc;
	}

	if (((csl_packet->header.op_code  & 0xFFFFF) ==
		CAM_PKT_NOP_OPCODE) ||
		((csl_packet->header.op_code & 0xFFFFF) ==
		CAM_FLASH_PACKET_OPCODE_SET_OPS)) {
		add_req.link_hdl = fctrl->bridge_intf.link_hdl;
		add_req.req_id = csl_packet->header.request_id;
		add_req.dev_hdl = fctrl->bridge_intf.device_hdl;

		if ((csl_packet->header.op_code & 0xFFFFF) ==
			CAM_FLASH_PACKET_OPCODE_SET_OPS)
			add_req.skip_before_applying |= 1;
		else
			add_req.skip_before_applying = 0;

		if (fctrl->bridge_intf.crm_cb &&
			fctrl->bridge_intf.crm_cb->add_req)
			fctrl->bridge_intf.crm_cb->add_req(&add_req);
		CAM_DBG(CAM_FLASH, "add req to req_mgr= %lld", add_req.req_id);
	}

	return rc;
}

int cam_flash_gpio_delete_req_sgm3785(struct cam_flash_ctrl *fctrl,
	uint64_t req_id)
{
	int i = 0;
	struct cam_flash_frame_setting *flash_data = NULL;
	uint64_t top = 0, del_req_id = 0;
	int frame_offset = 0;

	if (req_id != 0) {
		for (i = 0; i < MAX_PER_FRAME_ARRAY; i++) {
			flash_data = &fctrl->per_frame[i];
			if (req_id >= flash_data->cmn_attr.request_id &&
				flash_data->cmn_attr.is_settings_valid
				== 1) {
				if (top < flash_data->cmn_attr.request_id) {
					del_req_id = top;
					top = flash_data->cmn_attr.request_id;
				} else if (top >
					flash_data->cmn_attr.request_id &&
					del_req_id <
					flash_data->cmn_attr.request_id) {
					del_req_id =
						flash_data->cmn_attr.request_id;
				}
			}
		}

		if (top < req_id) {
			if ((((top % MAX_PER_FRAME_ARRAY) - (req_id %
				MAX_PER_FRAME_ARRAY)) >= BATCH_SIZE_MAX) ||
				(((top % MAX_PER_FRAME_ARRAY) - (req_id %
				MAX_PER_FRAME_ARRAY)) <= -BATCH_SIZE_MAX))
				del_req_id = req_id;
		}

		if (!del_req_id)
			return 0;

		CAM_DBG(CAM_FLASH, "top: %llu, del_req_id:%llu",
			top, del_req_id);
	}

	/* delete the request */
	frame_offset = del_req_id % MAX_PER_FRAME_ARRAY;
	flash_data = &fctrl->per_frame[frame_offset];
	flash_data->cmn_attr.request_id = 0;
	flash_data->cmn_attr.is_settings_valid = false;
	flash_data->cmn_attr.count = 0;

	for (i = 0; i < CAM_FLASH_MAX_LED_TRIGGERS; i++)
		flash_data->led_current_ma[i] = 0;

	return 0;
}

int cam_flash_gpio_apply_setting_sgm3785(struct cam_flash_ctrl *fctrl,
		uint64_t req_id)
{
		int rc = 0, i = 0;
		int frame_offset = 0;
		uint16_t num_iterations;
		struct cam_flash_frame_setting *flash_data = NULL;
		CAM_DBG(CAM_FLASH, "cam_flash_gpio req_id = %d",req_id);
		if (req_id == 0) {
			if (fctrl->nrt_info.cmn_attr.cmd_type ==
				CAMERA_SENSOR_FLASH_CMD_TYPE_INIT_FIRE) {
				flash_data = &fctrl->nrt_info;
				CAM_DBG(CAM_REQ,
					"FLASH_INIT_FIRE req_id: %u flash_opcode: %d",
					req_id, flash_data->opcode);

				if (flash_data->opcode ==
					CAMERA_SENSOR_FLASH_OP_FIREHIGH) {
					if (fctrl->flash_state ==
						CAM_FLASH_STATE_START) {
						CAM_WARN(CAM_FLASH,
						"Wrong state :Prev state: %d",
						fctrl->flash_state);
						return -EINVAL;
					}

					rc = cam_flash_gpio_high_sgm3785(fctrl, flash_data);
					if (rc)
						CAM_ERR(CAM_FLASH,
							"FLASH ON failed : %d", rc);
				}
				if (flash_data->opcode ==
					CAMERA_SENSOR_FLASH_OP_FIRELOW) {
					if (fctrl->flash_state ==
						CAM_FLASH_STATE_START) {
						CAM_WARN(CAM_FLASH,
						"Wrong state :Prev state: %d",
						fctrl->flash_state);
						return -EINVAL;
					}

					rc = cam_flash_gpio_low_sgm3785(fctrl, flash_data);
					if (rc)
						CAM_ERR(CAM_FLASH,
							"TORCH ON failed : %d", rc);
				}
				if (flash_data->opcode ==
					CAMERA_SENSOR_FLASH_OP_OFF) {
					rc = cam_flash_off(fctrl);
					if (rc) {
						CAM_ERR(CAM_FLASH,
						"LED OFF FAILED: %d",
						rc);
						return rc;
					}
				}
			} else if (fctrl->nrt_info.cmn_attr.cmd_type ==
				CAMERA_SENSOR_FLASH_CMD_TYPE_WIDGET) {
				flash_data = &fctrl->nrt_info;
				CAM_DBG(CAM_REQ,
					"FLASH_WIDGET req_id: %u flash_opcode: %d",
					req_id, flash_data->opcode);

				if (flash_data->opcode ==
					CAMERA_SENSOR_FLASH_OP_FIRELOW) {
					rc = cam_flash_gpio_low_sgm3785(fctrl, flash_data);
					if (rc) {
						CAM_ERR(CAM_FLASH,
							"Torch ON failed : %d",
							rc);
						goto nrt_del_req;
					}
				} else if (flash_data->opcode ==
					CAMERA_SENSOR_FLASH_OP_OFF) {
					rc = cam_flash_off(fctrl);
					if (rc)
						CAM_ERR(CAM_FLASH,
						"LED off failed: %d",
						rc);
				}
			} else if (fctrl->nrt_info.cmn_attr.cmd_type ==
				CAMERA_SENSOR_FLASH_CMD_TYPE_RER) {
				flash_data = &fctrl->nrt_info;
				if (fctrl->flash_state != CAM_FLASH_STATE_START) {
					rc = cam_flash_off(fctrl);
					if (rc) {
						CAM_ERR(CAM_FLASH,
							"Flash off failed: %d",
							rc);
						goto nrt_del_req;
					}
				}
				CAM_DBG(CAM_REQ, "FLASH_RER req_id: %u", req_id);

				num_iterations = flash_data->num_iterations;
				for (i = 0; i < num_iterations; i++) {
					/* Turn On Torch */
					if (fctrl->flash_state ==
						CAM_FLASH_STATE_START) {
						rc = cam_flash_gpio_low_sgm3785(fctrl, flash_data);
						if (rc) {
							CAM_ERR(CAM_FLASH,
								"Fire Torch Failed");
							goto nrt_del_req;
						}

						usleep_range(
						flash_data->led_on_delay_ms * 1000,
						flash_data->led_on_delay_ms * 1000 +
							100);
					}
					/* Turn Off Torch */
					rc = cam_flash_off(fctrl);
					if (rc) {
						CAM_ERR(CAM_FLASH,
							"Flash off failed: %d", rc);
						continue;
					}
					fctrl->flash_state = CAM_FLASH_STATE_START;
					usleep_range(
					flash_data->led_off_delay_ms * 1000,
					flash_data->led_off_delay_ms * 1000 + 100);
				}
			}
		} else {
			frame_offset = req_id % MAX_PER_FRAME_ARRAY;
			flash_data = &fctrl->per_frame[frame_offset];
			CAM_DBG(CAM_REQ, "FLASH_RT req_id: %u flash_opcode: %d",
				req_id, flash_data->opcode);

			if ((flash_data->opcode == CAMERA_SENSOR_FLASH_OP_FIREHIGH) &&
				(flash_data->cmn_attr.is_settings_valid) &&
				(flash_data->cmn_attr.request_id == req_id)) {
				/* Turn On Flash */
				if (fctrl->flash_state == CAM_FLASH_STATE_START) {
					rc = cam_flash_gpio_high_sgm3785(fctrl, flash_data);
					if (rc) {
						CAM_ERR(CAM_FLASH,
							"Flash ON failed: rc= %d",
							rc);
						goto apply_setting_err;
					}
				}
			} else if ((flash_data->opcode ==
				CAMERA_SENSOR_FLASH_OP_FIRELOW) &&
				(flash_data->cmn_attr.is_settings_valid) &&
				(flash_data->cmn_attr.request_id == req_id)) {
				/* Turn On Torch */
				if (fctrl->flash_state == CAM_FLASH_STATE_START) {
					rc = cam_flash_gpio_low_sgm3785(fctrl, flash_data);
					if (rc) {
						CAM_ERR(CAM_FLASH,
							"Torch ON failed: rc= %d",
							rc);
						goto apply_setting_err;
					}
				}
			} else if ((flash_data->opcode == CAMERA_SENSOR_FLASH_OP_OFF) &&
				(flash_data->cmn_attr.is_settings_valid) &&
				(flash_data->cmn_attr.request_id == req_id)) {
				rc = cam_flash_off(fctrl);
				if (rc) {
					CAM_ERR(CAM_FLASH,
						"Flash off failed %d", rc);
					goto apply_setting_err;
				}
			} else if (flash_data->opcode == CAM_PKT_NOP_OPCODE) {
				CAM_DBG(CAM_FLASH, "NOP Packet");
			} else {
				rc = -EINVAL;
				CAM_ERR(CAM_FLASH, "Invalid opcode: %d req_id: %llu",
					flash_data->opcode, req_id);
				goto apply_setting_err;
			}
		}

	nrt_del_req:
		cam_flash_gpio_delete_req_sgm3785(fctrl, req_id);
	apply_setting_err:
		return rc;
}


