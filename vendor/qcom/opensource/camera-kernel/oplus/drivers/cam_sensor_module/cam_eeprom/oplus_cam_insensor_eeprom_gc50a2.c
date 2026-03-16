
// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2017-2021, The Linux Foundation. All rights reserved.
 */
#include "cam_eeprom_dev.h"
#include "cam_req_mgr_dev.h"
#include "cam_eeprom_soc.h"
#include "cam_eeprom_core.h"
#include "cam_debug_util.h"
#include "oplus_cam_insensor_eeprom_gc50a2.h"
#include <linux/delay.h>

struct gc50a2_OtpGroupMap groupmap = {

    .GroupFlag   = 0x01, //group flag valid value
    .group = {
        {.StartAddr = 0x2000, .EndAddr = 0x5970},
        {.StartAddr = 0x5978, .EndAddr = 0x92e8},
        {.StartAddr = 0x92f0, .EndAddr = 0xcc60}
    },
    .flagOffset = {0x10, 0x22, 0x28,0x42, 0x72c}, // Module info, AWB_5000, light, SN, LSC
};

struct gc50a2_OtpItemInfo ItemInfo[GC50A2_CHECK_ITEM_NUM] = {//   avaible, start, end, checksum
    { .IsAvailable = true, .start_addr = 0x0000, .end_addr = 0x000f, .checksum_addr = 0x0011}, //module info
    { .IsAvailable = true, .start_addr = 0x0012, .end_addr = 0x0021, .checksum_addr = 0x0023}, //AWB_5000
    { .IsAvailable = true, .start_addr = 0x0024, .end_addr = 0x0027, .checksum_addr = 0x0029}, //light info
    { .IsAvailable = true, .start_addr = 0x002A, .end_addr = 0x0041, .checksum_addr = 0x0043}, //SN info
    { .IsAvailable = true, .start_addr = 0x0044, .end_addr = 0x072b, .checksum_addr = 0x072d}  //lsc info
};

struct gc50a2_otp_setting_params gc50a2_otp_init_setting_first  = {
    .slave_addr = 0x6e,
    .mem_settings =
    {
        {0x031c, CAMERA_SENSOR_I2C_TYPE_WORD, 0x60, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0315, CAMERA_SENSOR_I2C_TYPE_WORD, 0x80, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0af4, CAMERA_SENSOR_I2C_TYPE_WORD, 0x01, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0af6, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0b90, CAMERA_SENSOR_I2C_TYPE_WORD, 0x10, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0b91, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0b92, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0ba0, CAMERA_SENSOR_I2C_TYPE_WORD, 0x17, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0ba1, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0ba2, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0ba4, CAMERA_SENSOR_I2C_TYPE_WORD, 0x03, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0ba5, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0ba6, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0ba8, CAMERA_SENSOR_I2C_TYPE_WORD, 0x40, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0ba9, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0baa, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0bac, CAMERA_SENSOR_I2C_TYPE_WORD, 0x40, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0bad, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0bae, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0bb0, CAMERA_SENSOR_I2C_TYPE_WORD, 0x02, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0bb1, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0bb2, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0bb8, CAMERA_SENSOR_I2C_TYPE_WORD, 0x02, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0bb9, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0bba, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0a70, CAMERA_SENSOR_I2C_TYPE_WORD, 0x80, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0a71, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0a72, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0a66, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0a67, CAMERA_SENSOR_I2C_TYPE_WORD, 0x80, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0a4d, CAMERA_SENSOR_I2C_TYPE_WORD, 0x0e, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0a45, CAMERA_SENSOR_I2C_TYPE_WORD, 0x02, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0a47, CAMERA_SENSOR_I2C_TYPE_WORD, 0x02, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0a50, CAMERA_SENSOR_I2C_TYPE_WORD, 0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
        {0x0a4f, CAMERA_SENSOR_I2C_TYPE_WORD, 0x0c, CAMERA_SENSOR_I2C_TYPE_BYTE, 0}
    },
    .memory_map_size = 35,
};

int gc50a2_init_setting(struct cam_eeprom_ctrl_t *e_ctrl)
{
    struct cam_sensor_i2c_reg_setting    i2c_reg_settings = {0};
    struct cam_sensor_i2c_reg_array    i2c_reg_array = {0};
    int i = 0, ret = 0;

    for(i=0;i< gc50a2_otp_init_setting_first.memory_map_size; i++)
    {
        i2c_reg_settings.addr_type = gc50a2_otp_init_setting_first.mem_settings[i].addr_type;
        i2c_reg_settings.data_type = gc50a2_otp_init_setting_first.mem_settings[i].data_type;
        i2c_reg_settings.size = 1;
        i2c_reg_array.reg_addr = gc50a2_otp_init_setting_first.mem_settings[i].reg_addr;
        i2c_reg_array.reg_data = gc50a2_otp_init_setting_first.mem_settings[i].reg_data;
        i2c_reg_array.delay = gc50a2_otp_init_setting_first.mem_settings[i].delay;
        i2c_reg_settings.reg_setting = &i2c_reg_array;
        ret = camera_io_dev_write(&(e_ctrl->io_master_info), &i2c_reg_settings);
        if (ret < 0) {
            CAM_ERR(CAM_EEPROM, "write init params failed eeprom_result %d", ret);
            return ret;
        }
    }
    return ret;
}

int g50a2_otp_check(uint8_t *data) {

    int item_sum = 0,total_sum = 0;
    int i = 0, j = 0;

    for (i=0; i < GC50A2_CHECK_ITEM_NUM; i++) {
        item_sum = 0;
        /*flag check*/
        if (data[groupmap.flagOffset[i]] != groupmap.GroupFlag){
            CAM_ERR(CAM_EEPROM, "data[%d] flag : addr[0x%x]:0x%x  invalid  ", i, groupmap.flagOffset[i], data[groupmap.flagOffset[i]]);
            return GC50A2_OTP_FAIL;
        }

        for (j=ItemInfo[i].start_addr; j <= ItemInfo[i].end_addr; j++){
            item_sum += data[j];
        }

        total_sum += (item_sum + data[ItemInfo[i].checksum_addr] + data[groupmap.flagOffset[i]]);

        /*item check sum*/
        if((item_sum%255) == data[ItemInfo[i].checksum_addr]){
            CAM_INFO(CAM_EEPROM, "data[%d] checksum success 0x%x : 0x%x", i, (item_sum%255),data[ItemInfo[i].checksum_addr]);
        } else {
            CAM_ERR(CAM_EEPROM, "data[%d] checksum failed 0x%x : 0x%x", i, (item_sum%255), data[ItemInfo[i].checksum_addr]);
            return GC50A2_OTP_FAIL;
        }
    }

    /*total check sum*/
    if((total_sum%255) == data[GC50A2_MAP_SIZE-1]){
        CAM_INFO(CAM_EEPROM, "data[%d] total check sum success 0x%x : 0x%x", i, data[GC50A2_MAP_SIZE-1],(total_sum%255));
    } else {
        CAM_INFO(CAM_EEPROM, "data[%d] total check sum failed 0x%x : 0x%x", i, data[GC50A2_MAP_SIZE-1],(total_sum%255));
        return GC50A2_OTP_FAIL;
    }

    return GC50A2_OTP_SUCCESS;
}

int gc50a2_eeprom_memory_map_read_data(struct cam_eeprom_ctrl_t *e_ctrl,uint8_t *memptr){

    int i = 0, j = 0, ret = 0;
    uint32_t  count_read, read_start_ddr;
    struct cam_sensor_i2c_reg_setting         i2c_reg_settings = {0};
    struct cam_sensor_i2c_reg_array           i2c_reg_array = {0};
    uint8_t gc50a2_otpdata[GC50A2_MAP_SIZE] = {0};
    struct cam_eeprom_ctrl_t          *ctrl = e_ctrl;
    struct cam_eeprom_memory_map_t    *emap = ctrl->cal_data.map;

    if (emap->mem.valid_size != GC50A2_MAP_SIZE) {
        CAM_ERR(CAM_EEPROM, "Invalid data size, read failed. valid_size :%d",emap->mem.valid_size);
        return GC50A2_OTP_FAIL;
    }

    memset(gc50a2_otpdata, 0, sizeof(gc50a2_otpdata));

    ret = gc50a2_init_setting(ctrl);
    if(ret < 0)
    {
        CAM_ERR(CAM_EEPROM, "gc50a2_init_setting failed. ret :%d",ret);
        return GC50A2_OTP_FAIL;
    }

    mdelay(10);

    for(i = 0;i < GC50A2_GROUP_NUM;i++)
    {
        read_start_ddr = groupmap.group[i].StartAddr;
        CAM_INFO(CAM_EEPROM, "group num %d, first read addr 0x%x",i, read_start_ddr);
        for(j = 0;j < GC50A2_MAP_SIZE; j++)
        {
            i2c_reg_settings.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
            i2c_reg_settings.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE;
            i2c_reg_settings.size = 1;
            i2c_reg_array.reg_addr = 0x0a69;
            i2c_reg_array.reg_data = (read_start_ddr >> 8) & 0xff;;
            i2c_reg_array.delay = 1;
            i2c_reg_settings.reg_setting = &i2c_reg_array;
            ret = camera_io_dev_write(&e_ctrl->io_master_info, &i2c_reg_settings);
            if (ret < 0) {
                CAM_ERR(CAM_EEPROM, "write init params failed ret %d", ret);
                return ret;
            }

            i2c_reg_settings.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
            i2c_reg_settings.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE;
            i2c_reg_settings.size = 1;
            i2c_reg_array.reg_addr = 0x0a6a;
            i2c_reg_array.reg_data = read_start_ddr & 0xff;
            i2c_reg_array.delay = 1;
            i2c_reg_settings.reg_setting = &i2c_reg_array;
            ret = camera_io_dev_write(&e_ctrl->io_master_info, &i2c_reg_settings);
            if (ret < 0) {
                CAM_ERR(CAM_EEPROM, "write init params failed ret %d", ret);
                return ret;
            }

            i2c_reg_settings.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
            i2c_reg_settings.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE;
            i2c_reg_settings.size = 1;
            i2c_reg_array.reg_addr = READ_ENABLE_ADDR;
            i2c_reg_array.reg_data = READ_ENABLE_VALUE;
            i2c_reg_array.delay = 1;
            i2c_reg_settings.reg_setting = &i2c_reg_array;
            ret = camera_io_dev_write(&e_ctrl->io_master_info, &i2c_reg_settings);
            if (ret < 0) {
                CAM_ERR(CAM_EEPROM, "write init params failed ret %d", ret);
                return ret;
            }

            /*read data*/
            emap->mem.addr = DATA_OUTPUT_ADDR;
            count_read = 1;
            emap->mem.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
            emap->mem.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE;

            ret = camera_io_dev_read_seq(&e_ctrl->io_master_info,
            emap->mem.addr, &gc50a2_otpdata[j],
            emap->mem.addr_type,
            emap->mem.data_type,count_read);
            if (ret < 0) {
                CAM_ERR(CAM_EEPROM, "read init params failed ret %d", ret);
                return ret;
            }
            read_start_ddr += DATA_ADDR_OFFSET;
        }

        /*check data*/
        ret = g50a2_otp_check(gc50a2_otpdata);
        if (ret == GC50A2_OTP_SUCCESS)
        {
            memcpy((void *)memptr, &gc50a2_otpdata[0], GC50A2_MAP_SIZE);
            CAM_ERR(CAM_EEPROM, "group %d selected ,flag %d %d %d %d %d",
                     i,
                     gc50a2_otpdata[groupmap.flagOffset[0]],
                     gc50a2_otpdata[groupmap.flagOffset[1]],
                     gc50a2_otpdata[groupmap.flagOffset[2]],
                     gc50a2_otpdata[groupmap.flagOffset[3]],
                     gc50a2_otpdata[groupmap.flagOffset[4]]);
            return GC50A2_OTP_SUCCESS;
        }
    }

    return GC50A2_OTP_FAIL;
}