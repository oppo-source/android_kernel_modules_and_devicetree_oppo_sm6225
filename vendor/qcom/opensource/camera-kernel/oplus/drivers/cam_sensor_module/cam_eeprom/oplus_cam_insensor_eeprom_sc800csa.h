#ifndef EEPROM_WRITE_INIT_PARAMS_BEFORE_SC800CS2_H_
#define EEPROM_WRITE_INIT_PARAMS_BEFORE_SC800CS2_H_

#include "oplus_cam_sensor_dev.h"

#define SC800CSA_CHECK_NUM_MAX 5
#define SC800CSA_EEPROM_MEMORY_MAP_MAX_SIZE  300
#define SC800CSA_RETRY_GROUP_NUM 6

struct sc800csa_OtpGroupInfo
{
	uint32_t IsAvailable;
	uint32_t ItemNum;
	uint32_t CheckItemOffset[SC800CSA_CHECK_NUM_MAX];
	uint32_t GroupFlag;
	uint32_t SelectGroupNum;
};
struct sc800csa_OtpItemInfo
{
	uint32_t IsAvailable;
	uint32_t start_addr;
	uint32_t end_addr;
	uint32_t checksum_addr;
};

struct sc800csa_OtpCheckInfo
{
	struct sc800csa_OtpGroupInfo groupInfo;
	struct sc800csa_OtpItemInfo ItemInfo[SC800CSA_CHECK_NUM_MAX];
};

struct sc800csa_camera_reg_settings_t{
	uint32_t reg_addr;
	enum camera_sensor_i2c_type addr_type;
	uint32_t reg_data;
	enum camera_sensor_i2c_type data_type;
	uint32_t delay;
};

struct sc800csa_eeprom_memory_map_init_write_params{
	uint32_t slave_addr;
	struct sc800csa_camera_reg_settings_t mem_settings[SC800CSA_EEPROM_MEMORY_MAP_MAX_SIZE];
	uint32_t memory_map_size;
};

struct sc800csa_PagReadOtpInfo
{
	uint32_t  StartAddr;
	uint32_t  Length;
};

int sc800csa_eeprom_memory_map_read_data(struct cam_eeprom_ctrl_t *e_ctrl,uint8_t *memptr);
int sc800csa_eeprom_memory_map_read_data_oem(struct cam_eeprom_ctrl_t *e_ctrl,uint8_t *memptr);
#endif
