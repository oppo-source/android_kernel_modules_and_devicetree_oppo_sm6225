#ifndef OPLUS_CAM_INSENSOR_EEPROM_UTIL_H
#define OPLUS_CAM_INSENSOR_EEPROM_UTIL_H

#include "cam_eeprom_dev.h"

struct eeprom_info
{
	bool      isProbe;
	uint8_t   sensorIDAddr;
	uint32_t  sensorIdRegAddr;
	uint32_t  currCamid;
	int (*read_insensor_eeprom_fun)(struct cam_eeprom_ctrl_t *e_ctrl,uint8_t *memptr);
};

enum eeprom_num {
	EEPROM_SC520CS_SENSOR,
	EEPROM_GC05A2_SENSOR,
	EEPROM_MAX_SENSOR
};

int  get_insensor_eeprom_index(struct cam_eeprom_ctrl_t *e_ctrl);
int  read_insensor_eeprom_data(struct cam_eeprom_ctrl_t *e_ctrl,uint8_t *memptr);

#endif