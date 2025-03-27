
// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2017-2021, The Linux Foundation. All rights reserved.
 */
#include "cam_eeprom_dev.h"
#include "cam_req_mgr_dev.h"
#include "cam_eeprom_soc.h"
#include "cam_eeprom_core.h"
#include "cam_debug_util.h"
#include "oplus_cam_insensor_eeprom_sc520cs.h"


#include <linux/delay.h>

#define sc520CS_MAP_SIZE 0x72f

struct sc520cs_eeprom_memory_map_init_write_params sc520cs_eeprom_pag1[3] = {
	{
		.slave_addr = 0x6c,
		.mem_settings =
		{
			{0x36b0, CAMERA_SENSOR_I2C_TYPE_WORD, 0x4c, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//r1
			{0x36b1, CAMERA_SENSOR_I2C_TYPE_WORD, 0x48, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x36b2, CAMERA_SENSOR_I2C_TYPE_WORD, 0xc1, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},

			{0x4408, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//pag1
			{0x4409, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440a, CAMERA_SENSOR_I2C_TYPE_WORD, 0x07, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440b, CAMERA_SENSOR_I2C_TYPE_WORD, 0xff, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4401, CAMERA_SENSOR_I2C_TYPE_WORD, 0x13, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4412, CAMERA_SENSOR_I2C_TYPE_WORD, 0x01, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4407, CAMERA_SENSOR_I2C_TYPE_WORD, 0x0c, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4400, CAMERA_SENSOR_I2C_TYPE_WORD, 0x11, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
		},
		.memory_map_size = 11,
	},
	{
		.slave_addr = 0x6c,
		.mem_settings =
		{
			{0x36b0, CAMERA_SENSOR_I2C_TYPE_WORD, 0x4c, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//r0
			{0x36b1, CAMERA_SENSOR_I2C_TYPE_WORD, 0x28, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x36b2, CAMERA_SENSOR_I2C_TYPE_WORD, 0xc1, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},

			{0x4408, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//pag1
			{0x4409, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440a, CAMERA_SENSOR_I2C_TYPE_WORD, 0x07, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440b, CAMERA_SENSOR_I2C_TYPE_WORD, 0xff, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4401, CAMERA_SENSOR_I2C_TYPE_WORD, 0x13, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4412, CAMERA_SENSOR_I2C_TYPE_WORD, 0x01, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4407, CAMERA_SENSOR_I2C_TYPE_WORD, 0x0c, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4400, CAMERA_SENSOR_I2C_TYPE_WORD, 0x11, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
		},
		.memory_map_size = 11,
	},
	{
		.slave_addr = 0x6c,
		.mem_settings =
		{
			{0x36b0, CAMERA_SENSOR_I2C_TYPE_WORD, 0x4c, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//r0
			{0x36b1, CAMERA_SENSOR_I2C_TYPE_WORD, 0x38, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x36b2, CAMERA_SENSOR_I2C_TYPE_WORD, 0xc1, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},

			{0x4408, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//pag1
			{0x4409, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440a, CAMERA_SENSOR_I2C_TYPE_WORD, 0x07, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440b, CAMERA_SENSOR_I2C_TYPE_WORD, 0xff, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4401, CAMERA_SENSOR_I2C_TYPE_WORD, 0x13, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4412, CAMERA_SENSOR_I2C_TYPE_WORD, 0x01, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4407, CAMERA_SENSOR_I2C_TYPE_WORD, 0x0c, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4400, CAMERA_SENSOR_I2C_TYPE_WORD, 0x11, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
		},
		.memory_map_size = 11,
	}
};

struct sc520cs_eeprom_memory_map_init_write_params sc520cs_eeprom_pag2  ={
	.slave_addr = 0x6c,
		.mem_settings =
		{
			{0x4408, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4409, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440a, CAMERA_SENSOR_I2C_TYPE_WORD, 0x07, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440b, CAMERA_SENSOR_I2C_TYPE_WORD, 0xff, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4401, CAMERA_SENSOR_I2C_TYPE_WORD, 0x13, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4412, CAMERA_SENSOR_I2C_TYPE_WORD, 0x03, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4407, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4400, CAMERA_SENSOR_I2C_TYPE_WORD, 0x11, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
		},
		.memory_map_size = 8,
};


struct sc520cs_eeprom_memory_map_init_write_params sc520cs_eeprom_after_read  ={
	.slave_addr = 0x6c,
	.mem_settings =
	{
		{0x4408, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
		{0x4409, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
		{0x440a, CAMERA_SENSOR_I2C_TYPE_WORD, 0x07, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
		{0x440b, CAMERA_SENSOR_I2C_TYPE_WORD, 0xff, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
		{0x4401, CAMERA_SENSOR_I2C_TYPE_WORD, 0x13, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
		{0x4412, CAMERA_SENSOR_I2C_TYPE_WORD, 0x01, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
		{0x4407, CAMERA_SENSOR_I2C_TYPE_WORD, 0x0c, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
		{0x4400, CAMERA_SENSOR_I2C_TYPE_WORD, 0x11, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
	},
	.memory_map_size = 8,
};

struct  sc520cs_OtpCheckInfo sc520cs_checksum_info ={
	.groupInfo =
	{
		.IsAvailable = TRUE,
		.ItemNum = 5,
		.GroupFlag   = 0x01, //group flag valid value
		.CheckItemOffset = {0x10, 0x22, 0x28,0x42, 0x72c}, // Module info, AWB_5000, light, SN, LSC
	},
	.ItemInfo =
	{//   avaible, start, end, checksum
		{ TRUE, 0x0000, 0x000F, 0x0011}, //module info
		{ TRUE, 0x0012, 0x0021, 0x0023}, //AWB_5000
		{ TRUE, 0x0024, 0x0027, 0x0029}, //light info
		{ TRUE, 0x002A, 0x0041, 0x0043}, //SN info
		{ TRUE, 0x0044, 0x072b, 0x072d}, //lsc info
		//{ TRUE, 0x0000, 0x072E, 0x072F}, //lsc info
	},
};

struct sc520cs_PagReadOtpInfo Pag1OtpData = {
	.StartAddr = 0x80b6,
	.Length = 0x72f
};

struct sc520cs_PagReadOtpInfo Pag2OtpData = {
	.StartAddr = 0x8866,
	.Length = 0x72f
};

int sc520cs_otp_check(uint8_t *data) {
	int rc = 0;
	int sum = 0;
	int item_num = sc520cs_checksum_info.groupInfo.ItemNum;
	struct sc520cs_OtpGroupInfo *groupinfo = &sc520cs_checksum_info.groupInfo;
	struct sc520cs_OtpItemInfo *iteminfo = &sc520cs_checksum_info.ItemInfo[0];
	int i;
	int j;
	for (i=0; i < item_num; i++) {
		sum = 0;
		if (data[groupinfo->CheckItemOffset[i]] != groupinfo->GroupFlag){
			CAM_ERR(CAM_EEPROM, "data[%d] flag : addr[0x%x]:0x%x  invalid  ", i, groupinfo->CheckItemOffset[i], data[groupinfo->CheckItemOffset[i]]);
			return -1;
		}
		for (j=iteminfo[i].start_addr; j <= iteminfo[i].end_addr; j++){
			sum += data[j];
		}

		if(((sum%255) == data[iteminfo[i].checksum_addr])||((sum%255)+1 == data[iteminfo[i].checksum_addr])){
			CAM_INFO(CAM_EEPROM, "data[%d] checksum success 0x%x | 0x%x : 0x%x", i, (sum%255),(sum%255)+1, data[iteminfo[i].checksum_addr]);
		} else {
			CAM_ERR(CAM_EEPROM, "data[%d] checksum failed 0x%x : 0x%x", i, (sum%255), data[iteminfo[i].checksum_addr]);
			return -1;
		}
	}

	return rc;
}

int sc520cs_eeprom_memory_map_read_data(struct cam_eeprom_ctrl_t *e_ctrl,uint8_t *memptr){
	int eeprom_result = 0,j = 0;
	uint32_t  count_write,count_read;
	struct cam_sensor_i2c_reg_setting	i2c_reg_settings = {0};
	struct cam_sensor_i2c_reg_array	i2c_reg_array = {0};
	struct cam_eeprom_memory_map_t    *emap = e_ctrl->cal_data.map;
	uint8_t sc520cs_otpdata[sc520CS_MAP_SIZE] = {0};

	if (emap->mem.valid_size != Pag1OtpData.Length) {
		CAM_ERR(CAM_EEPROM, "Invalid data size, read failed. valid_size :%d ,data_size: %d", emap->mem.valid_size, Pag1OtpData.Length);
		return -1;
	}
	memset(&sc520cs_otpdata, 0, sizeof(sc520cs_otpdata));

	for(j = SC520CS_RETRY_GROUP_NUM-1; j >= 0; j--) {
		for(count_write=0;count_write< sc520cs_eeprom_pag1[j].memory_map_size; count_write++)
		{
			i2c_reg_settings.addr_type = sc520cs_eeprom_pag1[j].mem_settings[count_write].addr_type;
			i2c_reg_settings.data_type = sc520cs_eeprom_pag1[j].mem_settings[count_write].data_type;
			i2c_reg_settings.size = 1;
			i2c_reg_array.reg_addr = sc520cs_eeprom_pag1[j].mem_settings[count_write].reg_addr;
			i2c_reg_array.reg_data = sc520cs_eeprom_pag1[j].mem_settings[count_write].reg_data;
			i2c_reg_array.delay = sc520cs_eeprom_pag1[j].mem_settings[count_write].delay;
			i2c_reg_settings.reg_setting = &i2c_reg_array;
			eeprom_result = camera_io_dev_write(&e_ctrl->io_master_info, &i2c_reg_settings);
			if (eeprom_result) {
				CAM_ERR(CAM_EEPROM, "write init params failed eeprom_result %d", eeprom_result);
				return eeprom_result;
			}
		}
		mdelay(20);
		emap->mem.addr = Pag1OtpData.StartAddr;
		count_read = Pag1OtpData.Length;

		eeprom_result = camera_io_dev_read_seq(&e_ctrl->io_master_info,
		emap->mem.addr, &sc520cs_otpdata[0],
		emap->mem.addr_type,
		emap->mem.data_type,count_read);
		if (eeprom_result < 0) {
			CAM_ERR(CAM_EEPROM, "read failed eeprom_result %d",eeprom_result);
			return eeprom_result;
		}
		emap->mem.addr+=count_read;

		eeprom_result = sc520cs_otp_check(&sc520cs_otpdata[0]);
		if (eeprom_result < 0) {
			memset(&sc520cs_otpdata, 0, sizeof(sc520cs_otpdata));
			CAM_ERR(CAM_EEPROM, "failed to select group1 RETRY_GROUP_NUM=%d",j);
		}else {
			memcpy((void *)memptr, &sc520cs_otpdata[0], Pag1OtpData.Length);
			CAM_INFO(CAM_EEPROM, "select group1 success eeprom_result=%d,RETRY_GROUP_NUM=%d",eeprom_result,j);
			return eeprom_result;
		}
		for(count_write=0;count_write< sc520cs_eeprom_pag2.memory_map_size; count_write++)
		{
			i2c_reg_settings.addr_type = sc520cs_eeprom_pag2.mem_settings[count_write].addr_type;
			i2c_reg_settings.data_type = sc520cs_eeprom_pag2.mem_settings[count_write].data_type;
			i2c_reg_settings.size = 1;
			i2c_reg_array.reg_addr = sc520cs_eeprom_pag2.mem_settings[count_write].reg_addr;
			i2c_reg_array.reg_data = sc520cs_eeprom_pag2.mem_settings[count_write].reg_data;
			i2c_reg_array.delay = sc520cs_eeprom_pag2.mem_settings[count_write].delay;
			i2c_reg_settings.reg_setting = &i2c_reg_array;
			eeprom_result = camera_io_dev_write(&e_ctrl->io_master_info, &i2c_reg_settings);
			if (eeprom_result) {
				CAM_ERR(CAM_EEPROM, "write init params failed eeprom_result %d", eeprom_result);
				return eeprom_result;
			}
		}

		mdelay(20);
		emap->mem.addr = Pag2OtpData.StartAddr;
		count_read = Pag2OtpData.Length;
		eeprom_result = camera_io_dev_read_seq(&e_ctrl->io_master_info,
		emap->mem.addr, &sc520cs_otpdata[0],
		emap->mem.addr_type,
		emap->mem.data_type,count_read);
		if (eeprom_result < 0) {
			CAM_ERR(CAM_EEPROM, "read failed eeprom_result %d",eeprom_result);
			return eeprom_result;
		}
		emap->mem.addr+=count_read;

		eeprom_result = sc520cs_otp_check(&sc520cs_otpdata[0]);
		if (eeprom_result < 0) {
			memset(&sc520cs_otpdata, 0, sizeof(sc520cs_otpdata));
			CAM_ERR(CAM_EEPROM, "failed to select group2 RETRY_GROUP_NUM=%d",j);
		}else {
			memcpy((void *)memptr, &sc520cs_otpdata[0], Pag2OtpData.Length);
			CAM_INFO(CAM_EEPROM, "select group2 success eeprom_result=%d,RETRY_GROUP_NUM=%d",eeprom_result,j);
			return eeprom_result;
		}
	}
	return eeprom_result;
}
