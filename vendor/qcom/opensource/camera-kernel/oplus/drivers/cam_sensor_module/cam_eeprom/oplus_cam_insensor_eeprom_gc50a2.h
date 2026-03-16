#ifndef EEPROM_WRITE_INIT_PARAMS_BEFORE_GC50A2_H_
#define EEPROM_WRITE_INIT_PARAMS_BEFORE_GC50A2_H_

#include "oplus_cam_sensor_dev.h"


#define GC50A2_CHECK_ITEM_NUM   5
#define GC50A2_GROUP_NUM        3
#define GC50A2_OTP_SETTING_SIZE 40

#define GC50A2_MAP_SIZE         0x72f
#define sc520CS_MAP_ADDR_OFFSER 0x8

#define READ_ENABLE_ADDR        0x0a66
#define READ_ENABLE_VALUE       0x20
#define DATA_ADDR_OFFSET        0x08
#define DATA_OUTPUT_ADDR        0x0a6c

#define GC50A2_OTP_SUCCESS           (0)
#define GC50A2_OTP_FAIL              (-1)

struct gc50a2_GroupInfo
{
    uint32_t  StartAddr;
    uint32_t  EndAddr;
};

struct gc50a2_OtpGroupMap
{
    uint32_t GroupFlag;
    struct gc50a2_GroupInfo group[GC50A2_GROUP_NUM];
    uint32_t flagOffset[GC50A2_CHECK_ITEM_NUM];
};

struct gc50a2_camera_reg_settings_t{
    uint32_t reg_addr;
    enum camera_sensor_i2c_type addr_type;
    uint32_t reg_data;
    enum camera_sensor_i2c_type data_type;
    uint32_t delay;
};

struct gc50a2_OtpItemInfo
{
    bool     IsAvailable;
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t checksum_addr;
};

struct gc50a2_otp_setting_params{
    uint32_t slave_addr;
    struct gc50a2_camera_reg_settings_t mem_settings[GC50A2_OTP_SETTING_SIZE];
    uint32_t memory_map_size;
};

int gc50a2_eeprom_memory_map_read_data(struct cam_eeprom_ctrl_t *e_ctrl,uint8_t *memptr);

#endif
