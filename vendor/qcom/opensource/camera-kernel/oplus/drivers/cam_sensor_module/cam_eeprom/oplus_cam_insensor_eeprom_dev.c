#include "cam_debug_util.h"
#include "cam_eeprom_dev.h"
#include "oplus_cam_sensor_dev.h"
#include "oplus_cam_insensor_eeprom_dev.h"
#include "oplus_cam_insensor_eeprom_sc520cs.h"
#include "oplus_cam_insensor_eeprom_gc50a2.h"

struct eeprom_info all_eeprom_info[EEPROM_MAX_SENSOR] = {
    {
        .isProbe = false,
        .sensorIDAddr = 0x6c,
        .sensorIdRegAddr = 0x3107,
        .currCamid = SC520CS_SENSOR_ID,
        .read_insensor_eeprom_fun = sc520cs_eeprom_memory_map_read_data
    },
    {
        .isProbe = false,
        .sensorIDAddr = 0x6e,
        .sensorIdRegAddr = 0x03f0,
        .currCamid = GC05A2_SENSOR_ID,
        .read_insensor_eeprom_fun = gc50a2_eeprom_memory_map_read_data,
    }
};

int get_insensor_eeprom_index(struct cam_eeprom_ctrl_t *e_ctrl)
{
    struct cam_eeprom_memory_map_t    *emap = e_ctrl->cal_data.map;
    uint32_t chipid = 0;
    uint32_t i = 0;

    for(i = 0;i < EEPROM_MAX_SENSOR;i++)
    {
        emap->mem.addr = all_eeprom_info[i].sensorIdRegAddr;
        emap->mem.addr_type =CAMERA_SENSOR_I2C_TYPE_WORD;
        emap->mem.data_type = CAMERA_SENSOR_I2C_TYPE_WORD;

        camera_io_dev_read(&(e_ctrl->io_master_info),
        emap->mem.addr, &chipid,
        emap->mem.addr_type,
        emap->mem.data_type);

        CAM_DBG(CAM_EEPROM, "chipid 0x%x",chipid);
        if((emap->saddr == all_eeprom_info[i].sensorIDAddr)
            && (all_eeprom_info[i].currCamid == chipid))
        {
            all_eeprom_info[i].isProbe = true;
            CAM_DBG(CAM_EEPROM, "get_insensor_eeprom_index %d", i);
            return i;
        }
    }

    return -1;
}

int read_insensor_eeprom_data(struct cam_eeprom_ctrl_t *e_ctrl,uint8_t *memptr)
{
    enum eeprom_num index = EEPROM_MAX_SENSOR;
    int ret = -1;
    struct cam_eeprom_ctrl_t *ctrl = e_ctrl;
    uint8_t *ptr = memptr;
    index = get_insensor_eeprom_index(ctrl);

    if(0 <= index && index < EEPROM_MAX_SENSOR)
    {
        ret = all_eeprom_info[index].read_insensor_eeprom_fun(ctrl,ptr);
    }
    return ret;
}
