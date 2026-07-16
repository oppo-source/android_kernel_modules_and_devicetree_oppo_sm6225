
// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2017-2021, The Linux Foundation. All rights reserved.
 */
#include "cam_eeprom_dev.h"
#include "cam_req_mgr_dev.h"
#include "cam_eeprom_soc.h"
#include "cam_eeprom_core.h"
#include "cam_debug_util.h"
#include "oplus_cam_insensor_eeprom_sc800csa.h"
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/mm.h>

int threshold_num = 0;

struct sc800csa_camera_reg_settings_t threshold_initsettings[SC800CSA_EEPROM_THRESHOLD_PARAMS_NUM][SC800CSA_EEPROM_THRESHOLD_PARAMS_NUM] = {
	{
		{0x36b0, CAMERA_SENSOR_I2C_TYPE_WORD, 0x48, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//r0
		{0x36b1, CAMERA_SENSOR_I2C_TYPE_WORD, 0x38, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
		{0x36b2, CAMERA_SENSOR_I2C_TYPE_WORD, 0x41, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
	},
	{
		{0x36b0, CAMERA_SENSOR_I2C_TYPE_WORD, 0x48, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//Rn2
		{0x36b1, CAMERA_SENSOR_I2C_TYPE_WORD, 0x18, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
		{0x36b2, CAMERA_SENSOR_I2C_TYPE_WORD, 0x41, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
	},
	{
		{0x36b0, CAMERA_SENSOR_I2C_TYPE_WORD, 0x48, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//R1hf
		{0x36b1, CAMERA_SENSOR_I2C_TYPE_WORD, 0x58, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
		{0x36b2, CAMERA_SENSOR_I2C_TYPE_WORD, 0x41, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
	},
};


struct sc800csa_eeprom_memory_map_init_write_params sc800csa_eeprom_pag1[5] = {
	{
		.slave_addr = 0x21,
		.mem_settings =
		{
			{0x4408, CAMERA_SENSOR_I2C_TYPE_WORD, 0x82, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//pag1
			{0x4409, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440a, CAMERA_SENSOR_I2C_TYPE_WORD, 0x83, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440b, CAMERA_SENSOR_I2C_TYPE_WORD, 0xff, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4401, CAMERA_SENSOR_I2C_TYPE_WORD, 0x13, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4412, CAMERA_SENSOR_I2C_TYPE_WORD, 0x03, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4407, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4400, CAMERA_SENSOR_I2C_TYPE_WORD, 0x11, CAMERA_SENSOR_I2C_TYPE_BYTE, 20},
		},
		.memory_map_size = 8,
	},
	{
		.slave_addr = 0x21,
		.mem_settings =
		{
			{0x4408, CAMERA_SENSOR_I2C_TYPE_WORD, 0x84, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//pag2
			{0x4409, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440a, CAMERA_SENSOR_I2C_TYPE_WORD, 0x85, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440b, CAMERA_SENSOR_I2C_TYPE_WORD, 0xff, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4401, CAMERA_SENSOR_I2C_TYPE_WORD, 0x13, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4412, CAMERA_SENSOR_I2C_TYPE_WORD, 0x05, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4407, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4400, CAMERA_SENSOR_I2C_TYPE_WORD, 0x11, CAMERA_SENSOR_I2C_TYPE_BYTE, 20},
		},
		.memory_map_size = 8,
	},
	{
		.slave_addr = 0x21,
		.mem_settings =
		{
			{0x4408, CAMERA_SENSOR_I2C_TYPE_WORD, 0x86, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//pag3
			{0x4409, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440a, CAMERA_SENSOR_I2C_TYPE_WORD, 0x87, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440b, CAMERA_SENSOR_I2C_TYPE_WORD, 0xff, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4401, CAMERA_SENSOR_I2C_TYPE_WORD, 0x13, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4412, CAMERA_SENSOR_I2C_TYPE_WORD, 0x07, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4407, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4400, CAMERA_SENSOR_I2C_TYPE_WORD, 0x11, CAMERA_SENSOR_I2C_TYPE_BYTE, 20},
		},
		.memory_map_size = 8,
	},
	{
		.slave_addr = 0x21,
		.mem_settings =
		{
			{0x4408, CAMERA_SENSOR_I2C_TYPE_WORD, 0x88, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//pag4
			{0x4409, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440a, CAMERA_SENSOR_I2C_TYPE_WORD, 0x89, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440b, CAMERA_SENSOR_I2C_TYPE_WORD, 0xff, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4401, CAMERA_SENSOR_I2C_TYPE_WORD, 0x13, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4412, CAMERA_SENSOR_I2C_TYPE_WORD, 0x09, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4407, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4400, CAMERA_SENSOR_I2C_TYPE_WORD, 0x11, CAMERA_SENSOR_I2C_TYPE_BYTE, 20},
		},
		.memory_map_size = 8,
	},
	{
		.slave_addr = 0x21,
		.mem_settings =
		{
			{0x4408, CAMERA_SENSOR_I2C_TYPE_WORD, 0x8a, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//pag5
			{0x4409, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440a, CAMERA_SENSOR_I2C_TYPE_WORD, 0x8b, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440b, CAMERA_SENSOR_I2C_TYPE_WORD, 0xff, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4401, CAMERA_SENSOR_I2C_TYPE_WORD, 0x13, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4412, CAMERA_SENSOR_I2C_TYPE_WORD, 0x0b, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4407, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4400, CAMERA_SENSOR_I2C_TYPE_WORD, 0x11, CAMERA_SENSOR_I2C_TYPE_BYTE, 20},
		},
		.memory_map_size = 8,
	},
};

struct sc800csa_eeprom_memory_map_init_write_params sc800csa_eeprom_pag2[5] = {
	{
		.slave_addr = 0x21,
		.mem_settings =
		{
			{0x4408, CAMERA_SENSOR_I2C_TYPE_WORD, 0x8c, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//pag1
			{0x4409, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440a, CAMERA_SENSOR_I2C_TYPE_WORD, 0x8d, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440b, CAMERA_SENSOR_I2C_TYPE_WORD, 0xff, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4401, CAMERA_SENSOR_I2C_TYPE_WORD, 0x13, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4412, CAMERA_SENSOR_I2C_TYPE_WORD, 0x0d, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4407, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4400, CAMERA_SENSOR_I2C_TYPE_WORD, 0x11, CAMERA_SENSOR_I2C_TYPE_BYTE, 20},
		},
		.memory_map_size = 8,
	},
	{
		.slave_addr = 0x21,
		.mem_settings =
		{
			{0x4408, CAMERA_SENSOR_I2C_TYPE_WORD, 0x8e, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//pag2
			{0x4409, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440a, CAMERA_SENSOR_I2C_TYPE_WORD, 0x8f, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440b, CAMERA_SENSOR_I2C_TYPE_WORD, 0xff, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4401, CAMERA_SENSOR_I2C_TYPE_WORD, 0x13, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4412, CAMERA_SENSOR_I2C_TYPE_WORD, 0x0f, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4407, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4400, CAMERA_SENSOR_I2C_TYPE_WORD, 0x11, CAMERA_SENSOR_I2C_TYPE_BYTE, 20},
		},
		.memory_map_size = 8,
	},
	{
		.slave_addr = 0x21,
		.mem_settings =
		{
			{0x4408, CAMERA_SENSOR_I2C_TYPE_WORD, 0x90, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//pag3
			{0x4409, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440a, CAMERA_SENSOR_I2C_TYPE_WORD, 0x91, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440b, CAMERA_SENSOR_I2C_TYPE_WORD, 0xff, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4401, CAMERA_SENSOR_I2C_TYPE_WORD, 0x13, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4412, CAMERA_SENSOR_I2C_TYPE_WORD, 0x11, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4407, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4400, CAMERA_SENSOR_I2C_TYPE_WORD, 0x11, CAMERA_SENSOR_I2C_TYPE_BYTE, 20},
		},
		.memory_map_size = 8,
	},
	{
		.slave_addr = 0x21,
		.mem_settings =
		{
			{0x4408, CAMERA_SENSOR_I2C_TYPE_WORD, 0x92, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//pag4
			{0x4409, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440a, CAMERA_SENSOR_I2C_TYPE_WORD, 0x93, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440b, CAMERA_SENSOR_I2C_TYPE_WORD, 0xff, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4401, CAMERA_SENSOR_I2C_TYPE_WORD, 0x13, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4412, CAMERA_SENSOR_I2C_TYPE_WORD, 0x13, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4407, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4400, CAMERA_SENSOR_I2C_TYPE_WORD, 0x11, CAMERA_SENSOR_I2C_TYPE_BYTE, 20},
		},
		.memory_map_size = 8,
	},
	{
		.slave_addr = 0x21,
		.mem_settings =
		{
			{0x4408, CAMERA_SENSOR_I2C_TYPE_WORD, 0x94, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},//pag5
			{0x4409, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440a, CAMERA_SENSOR_I2C_TYPE_WORD, 0x95, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x440b, CAMERA_SENSOR_I2C_TYPE_WORD, 0xff, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4401, CAMERA_SENSOR_I2C_TYPE_WORD, 0x13, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4412, CAMERA_SENSOR_I2C_TYPE_WORD, 0x15, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4407, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
			{0x4400, CAMERA_SENSOR_I2C_TYPE_WORD, 0x11, CAMERA_SENSOR_I2C_TYPE_BYTE, 20},
		},
		.memory_map_size = 8,
	},
};

struct  sc800csa_OtpCheckInfo sc800csa_checksum_info ={
	.groupInfo =
	{
		.IsAvailable = TRUE,
		.ItemNum = 5,
		.GroupFlag   = 0x01, //group flag valid value
		.CheckItemOffset = {0x10, 0x22, 0x28,0x42,0x72c}, // Module info, AWB_5000, light, SN, LSC
	},
	.ItemInfo =
	{//   avaible, start, end, checksum
		{ TRUE, 0x01, 0x0f, 0x11}, //module info
		{ TRUE, 0x12, 0x21, 0x23}, //AWB_5000
		{ TRUE, 0x24, 0x27, 0x29}, //light info
		{ TRUE, 0x2a, 0x41, 0x43}, //SN info
		{ TRUE, 0x44, 0x72b, 0x72d}, //lsc info
	},
};


static struct sc800csa_PagReadOtpInfo Pag1OtpData[] ={
	{
		.StartAddr = 0x827a,
		.Length = 0x186
	},
	{
		.StartAddr = 0x847a,
		.Length = 0x186
	},
	{
		.StartAddr = 0x867a,
		.Length = 0x186
	},
	{
		.StartAddr = 0x887a,
		.Length = 0x186
	},
	{
		.StartAddr = 0x8A7a,
		.Length = 0x186
	}

};

static struct sc800csa_PagReadOtpInfo Pag1OtpData2[] ={
	{
		.StartAddr = 0x8c7a,
		.Length = 0x186
	},
	{
		.StartAddr = 0x8E7a,
		.Length = 0x186
	},
	{
		.StartAddr = 0x907a,
		.Length = 0x186
	},
	{
		.StartAddr = 0x927a,
		.Length = 0x186
	},
	{
		.StartAddr = 0x947a,
		.Length = 0x186
	}

};



int sc800csa_otp_check(uint8_t *data) {
	int rc = 0;
	int sum = 0;
	int item_num = sc800csa_checksum_info.groupInfo.ItemNum;
	struct sc800csa_OtpGroupInfo *groupinfo = &sc800csa_checksum_info.groupInfo;
	struct sc800csa_OtpItemInfo *iteminfo = &sc800csa_checksum_info.ItemInfo[0];
	int i;
	int j;
	for (i=0; i < item_num; i++) {
		sum = 0;
		if (data[groupinfo->CheckItemOffset[i]] != groupinfo->GroupFlag){
			CAM_ERR(CAM_EEPROM, "data[0x%x] flag : addr[0x%x]:0x%x  invalid  ", 0x8200+i, groupinfo->CheckItemOffset[i], data[groupinfo->CheckItemOffset[i]]);
			return -1;
		}
		for (j=iteminfo[i].start_addr; j <= iteminfo[i].end_addr; j++){
			sum += data[j];
		}

		if(((sum%255) == data[iteminfo[i].checksum_addr])||((sum%255)+1 == data[iteminfo[i].checksum_addr])){
			CAM_INFO(CAM_EEPROM, "data[0x%x] checksum success 0x%x | 0x%x : 0x%x", 0x8200+i, (sum%255),(sum%255)+1, data[iteminfo[i].checksum_addr]);
		} else {
			CAM_ERR(CAM_EEPROM, "data[0x%x] checksum failed 0x%x : 0x%x", 0x8200+i, (sum%255), data[iteminfo[i].checksum_addr]);
			return -1;
		}
	}

	return rc;
}



uint32_t get_insensor_eeprom_group_index_key(struct cam_eeprom_ctrl_t *e_ctrl)
{

    struct cam_eeprom_memory_map_t    *emap = e_ctrl->cal_data.map;
    uint32_t group = 0x0, count_write, eeprom_result;
	struct cam_sensor_i2c_reg_setting	i2c_reg_settings = {0};
	struct cam_sensor_i2c_reg_array	i2c_reg_array = {0};
	mdelay(20);
	emap->mem.addr = 0x827a;
	emap->mem.addr_type =CAMERA_SENSOR_I2C_TYPE_WORD;
	emap->mem.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE;

	for(count_write=0;count_write< sc800csa_eeprom_pag1[0].memory_map_size; count_write++)
	{
		i2c_reg_settings.addr_type = sc800csa_eeprom_pag1[0].mem_settings[count_write].addr_type;
		i2c_reg_settings.data_type = sc800csa_eeprom_pag1[0].mem_settings[count_write].data_type;
		i2c_reg_settings.size = 1;
		i2c_reg_array.reg_addr = sc800csa_eeprom_pag1[0].mem_settings[count_write].reg_addr;
		i2c_reg_array.reg_data = sc800csa_eeprom_pag1[0].mem_settings[count_write].reg_data;
		i2c_reg_array.delay = sc800csa_eeprom_pag1[0].mem_settings[count_write].delay;
		i2c_reg_settings.reg_setting = &i2c_reg_array;
		eeprom_result = camera_io_dev_write(&e_ctrl->io_master_info, &i2c_reg_settings);
		if (eeprom_result) {
			CAM_ERR(CAM_EEPROM, "write init params failed eeprom_result %d", eeprom_result);
			return eeprom_result;
		}
	}


	if(camera_io_dev_read(&(e_ctrl->io_master_info),
	emap->mem.addr, &group,
	emap->mem.addr_type,
	emap->mem.data_type))
	CAM_ERR(CAM_EEPROM, "eeprom index camera_io_dev_read fail");

	CAM_INFO(CAM_EEPROM, "selector group the key is 0x%x",group);

    return group;
}

int sc800csa_eeprom_memory_map_read_data_oem(struct cam_eeprom_ctrl_t *e_ctrl, uint8_t *memptr){
	return sc800csa_eeprom_memory_map_read_data_oem_retry(e_ctrl, memptr, &threshold_num);
}


int sc800csa_eeprom_memory_map_read_data_oem_retry(struct cam_eeprom_ctrl_t *e_ctrl, uint8_t *memptr, int* threshold_num)
{
	int eeprom_result = 0, j = 0 ,page_size = 0, key;
	uint32_t count_write, count_read;
	struct cam_sensor_i2c_reg_setting	i2c_reg_settings = {0};
	struct cam_sensor_i2c_reg_array	i2c_reg_array = {0};
	struct cam_eeprom_memory_map_t    *emap = e_ctrl->cal_data.map;
	uint8_t sc800csa_otpdata[sc800CSA_MAP_SIZE] = {0};


	if(emap->mem.valid_size != sc800CSA_MAP_SIZE) {
		CAM_ERR(CAM_EEPROM, "Invalid data size, read failed. valid_size :%d ,data_size: %d", emap->mem.valid_size, sc800CSA_MAP_SIZE);
		return -1;
	}

	key = get_insensor_eeprom_group_index_key(e_ctrl);

	retry:
	// init threshold params
	for(count_write = 0; count_write < SC800CSA_EEPROM_THRESHOLD_PARAMS_NUM; count_write++)
	{
		i2c_reg_settings.addr_type = threshold_initsettings[*threshold_num][count_write].addr_type;
		i2c_reg_settings.data_type = threshold_initsettings[*threshold_num][count_write].data_type;
		i2c_reg_settings.size = 1;
		i2c_reg_array.reg_addr = threshold_initsettings[*threshold_num][count_write].reg_addr;
		i2c_reg_array.reg_data = threshold_initsettings[*threshold_num][count_write].reg_data;
		i2c_reg_array.delay    = threshold_initsettings[*threshold_num][count_write].delay;
		i2c_reg_settings.reg_setting = &i2c_reg_array;
		eeprom_result = camera_io_dev_write(&e_ctrl->io_master_info, &i2c_reg_settings);
		if (eeprom_result) {
			CAM_ERR(CAM_EEPROM, "write init params failed eeprom_result %d", eeprom_result);
			return eeprom_result;
		}
	}
	if(key == 0x01)// use group 1
	{
		memset(&sc800csa_otpdata, 0, sizeof(sc800csa_otpdata));

		//init other params
		for(j = 0; j < sc800CSA_PAGNUM; j++) {
			for(count_write=0;count_write< sc800csa_eeprom_pag1[j].memory_map_size; count_write++)
			{
				i2c_reg_settings.addr_type = sc800csa_eeprom_pag1[j].mem_settings[count_write].addr_type;
				i2c_reg_settings.data_type = sc800csa_eeprom_pag1[j].mem_settings[count_write].data_type;
				i2c_reg_settings.size = 1;
				i2c_reg_array.reg_addr = sc800csa_eeprom_pag1[j].mem_settings[count_write].reg_addr;
				i2c_reg_array.reg_data = sc800csa_eeprom_pag1[j].mem_settings[count_write].reg_data;
				i2c_reg_array.delay = sc800csa_eeprom_pag1[j].mem_settings[count_write].delay;
				i2c_reg_settings.reg_setting = &i2c_reg_array;
				eeprom_result = camera_io_dev_write(&e_ctrl->io_master_info, &i2c_reg_settings);
				if (eeprom_result) {
					CAM_ERR(CAM_EEPROM, "write init params failed eeprom_result %d", eeprom_result);
					return eeprom_result;
				}

			}
			mdelay(20);
			emap->mem.addr = Pag1OtpData[j].StartAddr;
			count_read 	   = Pag1OtpData[j].Length;
			eeprom_result = camera_io_dev_read_seq(&e_ctrl->io_master_info,emap->mem.addr, &sc800csa_otpdata[page_size],
			emap->mem.addr_type,emap->mem.data_type,count_read);
			if (eeprom_result < 0) {
				CAM_ERR(CAM_EEPROM, "read failed eeprom_result %d",eeprom_result);
				return eeprom_result;
			}
			page_size+=count_read;

		}

		eeprom_result = sc800csa_otp_check(&sc800csa_otpdata[0]);

		if (eeprom_result < 0 && (*threshold_num) < 3) {
			CAM_ERR(CAM_EEPROM, "failed to use group1 RETRY_GROUP_NUM=%d  threshold_num = %d retry.....",j, *threshold_num);
			(*threshold_num)++;
			goto retry;

		}else if(eeprom_result < 0 && (*threshold_num) >= 3){
			memset(&sc800csa_otpdata, 0, sizeof(sc800csa_otpdata));
			(*threshold_num) = 0;
			CAM_ERR(CAM_EEPROM, "failed to use group1 RETRY_GROUP_NUM=%d",j);
			return eeprom_result;
		}else{
			(*threshold_num) = 0;
			memcpy((void *)memptr, &sc800csa_otpdata[0], sc800CSA_MAP_SIZE);
			CAM_INFO(CAM_EEPROM, "select group1 success eeprom_result=%d,RETRY_GROUP_NUM=%d",eeprom_result,j);
			return eeprom_result;
		}
	}else if (key == 0x13)//use group 2
	{
		memset(&sc800csa_otpdata, 0, sizeof(sc800csa_otpdata));
		page_size = 0;
		//init other params
		for(j = 0; j < sc800CSA_PAGNUM; j++) {
			for(count_write=0;count_write< sc800csa_eeprom_pag2[j].memory_map_size; count_write++)
			{
				i2c_reg_settings.addr_type = sc800csa_eeprom_pag2[j].mem_settings[count_write].addr_type;
				i2c_reg_settings.data_type = sc800csa_eeprom_pag2[j].mem_settings[count_write].data_type;
				i2c_reg_settings.size = 1;
				i2c_reg_array.reg_addr = sc800csa_eeprom_pag2[j].mem_settings[count_write].reg_addr;
				i2c_reg_array.reg_data = sc800csa_eeprom_pag2[j].mem_settings[count_write].reg_data;
				i2c_reg_array.delay = sc800csa_eeprom_pag2[j].mem_settings[count_write].delay;
				i2c_reg_settings.reg_setting = &i2c_reg_array;
				eeprom_result = camera_io_dev_write(&e_ctrl->io_master_info, &i2c_reg_settings);
				if (eeprom_result) {
					CAM_ERR(CAM_EEPROM, "write init params failed eeprom_result %d", eeprom_result);
					return eeprom_result;
				}

			}

			mdelay(20);
			emap->mem.addr = Pag1OtpData2[j].StartAddr;
			count_read 	   = Pag1OtpData2[j].Length;
			eeprom_result = camera_io_dev_read_seq(&e_ctrl->io_master_info,emap->mem.addr, &sc800csa_otpdata[page_size],
			emap->mem.addr_type,emap->mem.data_type,count_read);
			if (eeprom_result < 0) {
				CAM_ERR(CAM_EEPROM, "read failed eeprom_result %d",eeprom_result);
				return eeprom_result;
			}
			page_size+=count_read;

		}

		eeprom_result = sc800csa_otp_check(&sc800csa_otpdata[0]);

		if (eeprom_result < 0 && (*threshold_num) < 3) {
			CAM_ERR(CAM_EEPROM, "failed to use group1 RETRY_GROUP_NUM=%d  threshold_num = %d retry.....",j, *threshold_num);
			(*threshold_num)++;
			goto retry;

		}else if(eeprom_result < 0 && (*threshold_num) >= 3){
			memset(&sc800csa_otpdata, 0, sizeof(sc800csa_otpdata));
			(*threshold_num) = 0;
			CAM_ERR(CAM_EEPROM, "failed to use group1 RETRY_GROUP_NUM=%d",j);
			return eeprom_result;
		}else{
			(*threshold_num) = 0;
			memcpy((void *)memptr, &sc800csa_otpdata[0], sc800CSA_MAP_SIZE);
			CAM_INFO(CAM_EEPROM, "select group1 success eeprom_result=%d,RETRY_GROUP_NUM=%d",eeprom_result,j);
			return eeprom_result;
		}
	} else {
		key = get_insensor_eeprom_group_index_key(e_ctrl);
		goto retry;
    }

	CAM_ERR(CAM_EEPROM, "failed to use group  fail....");
	return eeprom_result;
}

