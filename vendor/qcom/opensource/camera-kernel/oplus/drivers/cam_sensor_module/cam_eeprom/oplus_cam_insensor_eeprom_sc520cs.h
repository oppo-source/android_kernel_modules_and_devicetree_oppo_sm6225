#ifndef EEPROM_WRITE_INIT_PARAMS_BEFORE_SC520CS_H_
#define EEPROM_WRITE_INIT_PARAMS_BEFORE_SC520CS_H_

#include "oplus_cam_sensor_dev.h"

#define SC520CS_CHECK_NUM_MAX 5
#define SC520CS_EEPROM_MEMORY_MAP_MAX_SIZE  300
#define SC520CS_RETRY_GROUP_NUM 3

struct sc520cs_OtpGroupInfo
{
	uint32_t IsAvailable;
	uint32_t ItemNum;
	uint32_t CheckItemOffset[SC520CS_CHECK_NUM_MAX];
	uint32_t GroupFlag;
	uint32_t SelectGroupNum;
};
struct sc520cs_OtpItemInfo
{
	uint32_t IsAvailable;
	uint32_t start_addr;
	uint32_t end_addr;
	uint32_t checksum_addr;
};

struct sc520cs_OtpCheckInfo
{
	struct sc520cs_OtpGroupInfo groupInfo;
	struct sc520cs_OtpItemInfo ItemInfo[SC520CS_CHECK_NUM_MAX];
};

struct sc520cs_camera_reg_settings_t{
	uint32_t reg_addr;
	enum camera_sensor_i2c_type addr_type;
	uint32_t reg_data;
	enum camera_sensor_i2c_type data_type;
	uint32_t delay;
};

struct sc520cs_eeprom_memory_map_init_write_params{
	uint32_t slave_addr;
	struct sc520cs_camera_reg_settings_t mem_settings[SC520CS_EEPROM_MEMORY_MAP_MAX_SIZE];
	uint32_t memory_map_size;
};

struct sc520cs_PagReadOtpInfo
{
	uint32_t  StartAddr;
	uint32_t  Length;
};

int sc520cs_eeprom_memory_map_read_data(struct cam_eeprom_ctrl_t *e_ctrl,uint8_t *memptr);

#endif
