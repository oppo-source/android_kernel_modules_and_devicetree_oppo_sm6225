{
	.need_standby_mode = 1,
	.need_boost_check = 1,
	.flashprobeinfo =
	{
		.projID = {23311,23312,23313,23314,23315,23316,23317,23318,23319},
		.flash_name = "sc6607_AB4",
		.slave_write_address = 0x64,
		.chip_id_slave_address = 0x61,
		.use_i2c_smbus = 1,
		.flash_id_address = 0x00,
		.flash_id = 0x67,
		.addr_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
		.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
	},
	.cci_client =
	{
		.cci_i2c_master = MASTER_0,
		.i2c_freq_mode = I2C_FAST_PLUS_MODE,
		.sid = 0x64 >> 1,
	},
	.apply_setting = cam_flash_check_boost_i2c_apply_setting,
	.flashinitsettings =
	{
		.reg_setting =
		{
			{.reg_addr = 0x80, .reg_data = 0x01, .delay = 0x00, .data_mask = 0x00}, \
			{.reg_addr = 0x81, .reg_data = 0x52, .delay = 0x00, .data_mask = 0x00}, \
			{.reg_addr = 0x82, .reg_data = 0x32, .delay = 0x00, .data_mask = 0x00}, \
			{.reg_addr = 0x83, .reg_data = 0x9f, .delay = 0x00, .data_mask = 0x00}, \
			{.reg_addr = 0x84, .reg_data = 0x07, .delay = 0x00, .data_mask = 0x00}, \
			{.reg_addr = 0x85, .reg_data = 0x03, .delay = 0x00, .data_mask = 0x00}, \
			{.reg_addr = 0x86, .reg_data = 0x02, .delay = 0x00, .data_mask = 0x00}, \
			{.reg_addr = 0x8A, .reg_data = 0x00, .delay = 0x00, .data_mask = 0x00}, \
			{.reg_addr = 0x8B, .reg_data = 0x01, .delay = 0x00, .data_mask = 0x00}, \
		},
		.size = 9,
		.addr_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
		.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
		.delay = 1,
	},
	.flashhighsettings =
	{
		.reg_setting =
		{

			{.reg_addr = 0x80, .reg_data = 0xc1, .delay = 0x00, .data_mask = 0x00}, \
		},
		.size = 1,
		.addr_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
		.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
		.delay = 1,
	},
	.flashlowsettings =
	{
		.reg_setting =//100ma
		{
			{.reg_addr = 0x84, .reg_data = 0x06, .delay = 0x00, .data_mask = 0x00}, \
			{.reg_addr = 0x80, .reg_data = 0x31, .delay = 0x00, .data_mask = 0x00}, \
		},
		.size = 2,
		.addr_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
		.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
		.delay = 1,
	},
	.flashlowlevelsettings =
	{
		{
			.reg_setting =//75ma
			{
				{.reg_addr = 0x84, .reg_data = 0x04, .delay = 0x00, .data_mask = 0x00}, \
				{.reg_addr = 0x80, .reg_data = 0x31, .delay = 0x00, .data_mask = 0x00}, \
			},
			.size = 2,
			.addr_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
			.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
			.delay = 1,
		},
		{
			.reg_setting =//112.5ma
			{
				{.reg_addr = 0x84, .reg_data = 0x07, .delay = 0x00, .data_mask = 0x00}, \
				{.reg_addr = 0x80, .reg_data = 0x31, .delay = 0x00, .data_mask = 0x00}, \
			},
			.size = 2,
			.addr_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
			.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
			.delay = 1,
		},
		{
			.reg_setting =//150 ma
			{
				{.reg_addr = 0x84, .reg_data = 0x0A, .delay = 0x00, .data_mask = 0x00}, \
				{.reg_addr = 0x80, .reg_data = 0x31, .delay = 0x00, .data_mask = 0x00}, \
			},
			.size = 2,
			.addr_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
			.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
			.delay = 1,
		},
		{
			.reg_setting =//200ma
			{
				{.reg_addr = 0x84, .reg_data = 0x0E, .delay = 0x00, .data_mask = 0x00}, \
				{.reg_addr = 0x80, .reg_data = 0x31, .delay = 0x00, .data_mask = 0x00}, \
			},
			.size = 2,
			.addr_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
			.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
			.delay = 1,
		},
	},
	.flashoffsettings =
	{
		.reg_setting =
		{
			{.reg_addr = 0x80, .reg_data = 0x01, .delay = 0x00, .data_mask = 0x00}, \
		},
		.size = 1,
		.addr_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
		.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
		.delay = 1,
	},
	.flashpowerupsetting =
	{
		.single_power =
		{
			{
				.seq_type = SENSOR_VIO,
				.config_val = 0,
				.delay = 5,
			},
		},
		.size = 1,
	},
	.flashpowerdownsetting =
	{
		.single_power =
		{
			{
				.seq_type = SENSOR_VIO,
				.config_val = 0,
				.delay = 1,
			},
		},
		.size = 1,
	},
},
