// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2012-2023, The Linux Foundation. All rights reserved.
 */
#include <linux/time.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/jiffies.h>
#include <linux/kobject.h>
#include <dsp/apr_audio-v2.h>
#include <dsp/q6adm-v2.h>
#include <dsp/q6audio-v2.h>
#include <dsp/q6afe-v2.h>
#include <dsp/q6core.h>
#include <dsp/q6voice.h>
#include <dsp/q6common.h>
#include <ipc/apr.h>
#include "adsp_err.h"

#include "dsp/oplus_opve_err_fb.h"
#include "feedback/oplus_audio_kernel_fb.h"

#define AUDIO_EVENTID_OPVE_ERR               10004
#define OPVE_ERR_VERSION                     "1.0.0"

#define CHECK_OPVE_LIMIT_MS                  3000

#define TIMEOUT_MS                           300
#define CMD_STATUS_SUCCESS                   0
#define CMD_STATUS_FAIL                      1

#define TX_ERR_NUM                           3
#define RX_ERR_NUM                           3
#define MAX_ERR_NUM                          (TX_ERR_NUM + RX_ERR_NUM)

#define MAX_PAYLOAD_DATASIZE 512

#define SetFdBuf(buf, max_len, arg, ...) \
	do { \
		snprintf(buf + strlen(buf), max_len - strlen(buf) - 1, arg, ##__VA_ARGS__); \
	} while (0)

enum {
	INVALID_FILE_ID = 0,
	OPVE_FILE,
	UTIL_FILE,
	FILE_ID_MAX
};

static const char * const OpveFileTable[FILE_ID_MAX] = {
	[INVALID_FILE_ID] = "invalid",
	[OPVE_FILE] = "capi_v2_opve.c",
	[UTIL_FILE] = "capi_v2_utils_props.c",
};

typedef enum
{
	OPVE_SUCCESS = 0,                               // 0    Sucessful return
	OPVE_FAILURE,                                   // 1    Common process failure
	OPVE_FAILURE_SET_SYSTEM_PROPERTIES,             // 2    Failure in setting system properties
	OPVE_FAILURE_INIT,                              // 3    Failure in init
	OPVE_FAILURE_SET_PARAM,                         // 4    Failure in set param
	OPVE_NULLADDRESS,                               // 5    Null address allocation
	OPVE_INVALID_PARAMLENGTH,                       // 6    Invalid parameter file length
	OPVE_INVALID_SAMPLERATE,                        // 7    Invalid sample rate
	OPVE_INVALID_CHANNELNUM,                        // 8    Invalid channel num
	OPVE_INVALID_BPS,                               // 9    Invalid bits per sample
	OPVE_INVALID_FRAMESIZE,                         // 10   Invalid frame size
	OPVE_MAXENUM
} OPVE_RUNTIME_FLAG_ID;

static const char * const OpveErrTable[] = {
	[OPVE_SUCCESS] = "SUCCESS",
	[OPVE_FAILURE] = "FAILURE",
	[OPVE_FAILURE_SET_SYSTEM_PROPERTIES] = "FAILURE_SET_SYSTEM_PROPERTIES",
	[OPVE_FAILURE_INIT] = "FAILURE_INIT",
	[OPVE_FAILURE_SET_PARAM] = "FAILURE_SET_PARAM",
	[OPVE_NULLADDRESS] = "NULLADDRESS",
	[OPVE_INVALID_PARAMLENGTH] = "INVALID_PARAMLENGTH",
	[OPVE_INVALID_SAMPLERATE] = "INVALID_SAMPLERATE",
	[OPVE_INVALID_CHANNELNUM] = "INVALID_CHANNELNUM",
	[OPVE_INVALID_BPS] = "INVALID_BPS",
	[OPVE_INVALID_FRAMESIZE] = "INVALID_FRAMESIZE",
	[OPVE_MAXENUM] = "MAXENUM",
};

typedef struct
{
	uint16_t file;
	uint16_t line;
	uint16_t err;
	uint16_t cnt;
	int rxtx; //1 for rx, 0 for tx
} opve_err_info_t;

static int enable_flag = 0;
static opve_err_info_t voice_err_buf[MAX_ERR_NUM] = {0};

static int oplus_opve_check_ctrl_set(struct snd_kcontrol *kcontrol,
						struct snd_ctl_elem_value *ucontrol)
{
	enable_flag = ucontrol->value.integer.value[0];
	pr_info("%s : set: %d\n", __func__, enable_flag);

	return 0;
}

static int oplus_opve_check_ctrl_get(struct snd_kcontrol *kcontrol,
						struct snd_ctl_elem_value *ucontrol)
{
	pr_info("%s : get: %d\n", __func__, enable_flag);
	ucontrol->value.integer.value[0] = enable_flag;

	return 0;
}

static const struct snd_kcontrol_new oplus_opve_err_fb_controls[] = {
	SOC_SINGLE_EXT("OPVE_CHECK_FEEDBACK", SND_SOC_NOPM, 0, 1, 0, \
			oplus_opve_check_ctrl_get, oplus_opve_check_ctrl_set),
};

void oplus_opve_err_fb_add_controls(struct snd_soc_component *component)
{
	snd_soc_add_component_controls(component, oplus_opve_err_fb_controls,
			ARRAY_SIZE(oplus_opve_err_fb_controls));
	pr_info("%s: event_id=%u, version:%s\n", __func__, AUDIO_EVENTID_OPVE_ERR, OPVE_ERR_VERSION);
}
EXPORT_SYMBOL(oplus_opve_err_fb_add_controls);

void oplus_opve_copy_voice_err_result(int port_type, void *presult, uint32_t len)
{
	uint32_t copylen = 0;

	if (!enable_flag) {
		return;
	}

	if (!presult) {
		pr_err("%s: presult is NULL\n", __func__);
		return;
	}

	if (MSM_AFE_PORT_TYPE_RX == port_type) {
		copylen = RX_ERR_NUM * sizeof(opve_err_info_t);
		copylen = (copylen > len) ? len : copylen;
		memcpy(voice_err_buf, presult, copylen);
	} else {
		copylen = TX_ERR_NUM * sizeof(opve_err_info_t);
		copylen = (copylen > len) ? len : copylen;
		memcpy(&voice_err_buf[RX_ERR_NUM], presult, copylen);
	}

	pr_info("%s: port_type=%d, len=%u, copylen=%u\n", __func__, port_type, len, copylen);
}
EXPORT_SYMBOL(oplus_opve_copy_voice_err_result);

static int oplus_parse_err(opve_err_info_t *p_err, int num, char *pbuf)
{
	int flag = 0;
	int i = 0;

	if (!pbuf || !p_err || (num <= 0)) {
		pr_err("%s: invalid param, num=%d or pbuf or p_err is NULL\n", __func__, num);
		return 0;
	}

	for (i = 0; i < num; i++) {
		if (OPVE_FILE == p_err[i].file || UTIL_FILE == p_err[i].file) {
			if ((p_err[i].err <= OPVE_SUCCESS) || (p_err[i].err >= OPVE_MAXENUM)) {
				SetFdBuf(pbuf, MAX_PAYLOAD_DATASIZE, "%s:%u,error=%u,cnt=%u,dir=%s;", \
					OpveFileTable[p_err[i].file], p_err[i].line, p_err[i].err, p_err[i].cnt,
					(p_err[i].rxtx == 1) ? "RX" : "TX");
			} else {
				SetFdBuf(pbuf, MAX_PAYLOAD_DATASIZE, "%s:%u,error=%s,cnt=%u,dir=%s;", \
					OpveFileTable[p_err[i].file], p_err[i].line, OpveErrTable[p_err[i].err], p_err[i].cnt,
					(p_err[i].rxtx == 1) ? "RX" : "TX");
			}
			flag = 1;
		}
	}

	if (flag == 1) {
		pr_info("%s: %s\n", __func__, pbuf);
	}

	return flag;
}

static void oplus_voip_parse_fb_err(int port_type, char *presult)
{
	opve_err_info_t opve_buf[MAX_ERR_NUM] = {0};
	char err_buf[MAX_PAYLOAD_DATASIZE] = {0};
	char fb_buf[MAX_PAYLOAD_DATASIZE] = {0};
	int ret = 0;

	if (!presult) {
		pr_err("%s: presult is NULL\n", __func__);
		return;
	}

	if (MSM_AFE_PORT_TYPE_RX == port_type) {
		memcpy(opve_buf, presult, sizeof(opve_err_info_t) * RX_ERR_NUM);
		ret = oplus_parse_err(opve_buf, RX_ERR_NUM, err_buf);
	} else {
		memcpy(opve_buf, presult, sizeof(opve_err_info_t) * TX_ERR_NUM);
		ret = oplus_parse_err(opve_buf, TX_ERR_NUM, err_buf);
	}

	if (ret == 1) {
		SetFdBuf(fb_buf, MAX_PAYLOAD_DATASIZE, "payload@@%s$$type@@voip", err_buf);
		mm_fb_audio_kevent_named(AUDIO_EVENTID_OPVE_ERR, MM_FB_KEY_RATELIMIT_30MIN, fb_buf);
	}
}

static void oplus_voice_parse_fb_err()
{
	char err_buf[MAX_PAYLOAD_DATASIZE] = {0};
	char fb_buf[MAX_PAYLOAD_DATASIZE] = {0};

	pr_info("%s: enter.\n", __func__);
	if (1 == oplus_parse_err(voice_err_buf, MAX_ERR_NUM, err_buf)) {
		SetFdBuf(fb_buf, MAX_PAYLOAD_DATASIZE, "payload@@%s$$type@@voice", err_buf);
		mm_fb_audio_kevent_named(AUDIO_EVENTID_OPVE_ERR, MM_FB_KEY_RATELIMIT_30MIN, fb_buf);
	}

	memset(voice_err_buf, 0, sizeof(voice_err_buf));
}

int oplus_adm_get_opve_err_fb(int port_id, int copp_idx)
{
	int ret = 0;
	char *param_value;
	uint32_t param_size;
	struct param_hdr_v3 param_hdr;
	int port_type = 0;
	static ktime_t s_tx_time = 0;
	static ktime_t s_rx_time = 0;

	if (!enable_flag) {
		return 0;
	}

	pr_info("%s: enter port_id=%d, copp_idx=%d\n", __func__, port_id, copp_idx);

	if (apr_get_q6_state() != APR_SUBSYS_LOADED) {
		pr_err("%s: adsp state is %d\n", __func__, apr_get_q6_state());
		return -EBUSY;
	}

	if (port_id < 0) {
		pr_err("%s: Invalid port id: 0x%x", __func__, port_id);
		return -EINVAL;
	}

	port_type = afe_get_port_type(port_id);
	if (((MSM_AFE_PORT_TYPE_TX == port_type) && \
			(ktime_before(ktime_get(), ktime_add_ms(s_tx_time, CHECK_OPVE_LIMIT_MS)))) || \
		((MSM_AFE_PORT_TYPE_RX == port_type) && \
			(ktime_before(ktime_get(), ktime_add_ms(s_rx_time, CHECK_OPVE_LIMIT_MS))))) {
		pr_info("%s: %s check time limit", __func__, (MSM_AFE_PORT_TYPE_TX == port_type) ? "TX" : "RX");
		return -EINVAL;
	}

	param_size = MAX_ERR_NUM * sizeof(opve_err_info_t) + sizeof(struct param_hdr_v3);
	param_value = kzalloc(param_size, GFP_KERNEL);
	if (!param_value) {
		pr_err("%s: kzalloc failed\n", __func__);
		return -ENOMEM;
	}

	if (MSM_AFE_PORT_TYPE_TX == port_type) {
		param_hdr.module_id = OPVE_TX_MODULE_ID;
		param_hdr.instance_id = 0x8000;
	} else {
		param_hdr.module_id = OPVE_RX_MODULE_ID;
		param_hdr.instance_id = INSTANCE_ID_0;
	}
	param_hdr.param_id = VOICE_PARAM_OPVE_GET_ERR;
	param_hdr.param_size = param_size;

	ret = adm_get_pp_params(port_id, copp_idx,
				ADM_CLIENT_ID_DEFAULT, NULL, &param_hdr,
				param_value);
	if (ret) {
		pr_err("%s: get parameters failed ret:%d\n", __func__, ret);
		ret = -EINVAL;
		goto done;
	}

	oplus_voip_parse_fb_err(port_type, param_value);
	if (MSM_AFE_PORT_TYPE_TX == port_type) {
		s_tx_time = ktime_get();
	} else {
		s_rx_time = ktime_get();
	}

done:
	kfree(param_value);
	return ret;
}
EXPORT_SYMBOL(oplus_adm_get_opve_err_fb);

int oplus_voice_get_opve_err_fb(void *apr_cvp, struct voice_data *v)
{
	struct vss_icommon_cmd_get_param get_param;
	uint32_t pkt_size = sizeof(struct vss_icommon_cmd_get_param);
	int ret = 0;
	int i = 0;

	if (!enable_flag) {
		return 0;
	}

	pr_info("%s: enter\n", __func__);

	if (!q6common_is_instance_id_supported()) {
		return 0;
	}

	if (!v || !apr_cvp) {
		pr_err("%s: v or apr_cvp is NULL\n", __func__);
		return -EINVAL;
	}

	if (apr_get_q6_state() != APR_SUBSYS_LOADED) {
		pr_err("%s: adsp state is %d\n", __func__, apr_get_q6_state());
		return -EBUSY;
	}

	pkt_size = sizeof(struct vss_icommon_cmd_get_param);
	for (i = 0; i < 2; i++) {
		get_param.apr_hdr.hdr_field =
			APR_HDR_FIELD(APR_MSG_TYPE_SEQ_CMD, APR_HDR_LEN(APR_HDR_SIZE), APR_PKT_VER);
		get_param.apr_hdr.pkt_size =
			APR_PKT_SIZE(APR_HDR_SIZE, pkt_size - APR_HDR_SIZE);
		get_param.apr_hdr.src_svc = 0;
		get_param.apr_hdr.src_domain = APR_DOMAIN_APPS;
		get_param.apr_hdr.src_port = voice_get_idx_for_session(v->session_id);
		get_param.apr_hdr.dest_svc = 0;
		get_param.apr_hdr.dest_domain = APR_DOMAIN_ADSP;
		get_param.apr_hdr.dest_port = v->cvp_handle;
		get_param.apr_hdr.token = VOC_SET_MEDIA_FORMAT_PARAM_TOKEN;
		get_param.apr_hdr.opcode = VSS_ICOMMON_CMD_GET_PARAM_V3;
		get_param.mem_hdr.mem_address = 0;
		get_param.mem_hdr.mem_handle = 0;

		get_param.reserved = 0;
		get_param.param_id = VOICE_PARAM_OPVE_GET_ERR;
		get_param.payload_size = MAX_ERR_NUM * sizeof(opve_err_info_t) + sizeof(struct param_hdr_v3);
		if (i == 0) {
			get_param.module_id = OPVE_RX_MODULE_ID;
			get_param.instance_id = INSTANCE_ID_0;
		} else {
			get_param.module_id = OPVE_TX_MODULE_ID;
			get_param.instance_id = 0x8000;
		}

		v->cvp_state = CMD_STATUS_FAIL;
		v->async_err = 0;
		ret = apr_send_pkt(apr_cvp, (u32 *) &get_param);
		if (ret < 0) {
			pr_err_fb("payload@@oplus_opve_err_fb.c:Failed to send apr packet, error %d", ret);
			goto done;
		}

		ret = wait_event_timeout(v->cvp_wait,
					 v->cvp_state == CMD_STATUS_SUCCESS,
					 msecs_to_jiffies(TIMEOUT_MS));
		pr_info("%s: send wait done\n", __func__);

		if (!ret) {
			pr_err_fb("payload@@oplus_opve_err_fb.c:wait_event timeout, ret=%d", ret);
			ret = -ETIMEDOUT;
			goto done;
		}

		if (v->async_err > 0) {
			ret = adsp_err_get_lnx_err_code(v->async_err);
			pr_err_fb("payload@@oplus_opve_err_fb.c:DSP returned error[%s], ret=%d", \
					adsp_err_get_err_str(v->async_err), ret);
			goto done;
		}
	}
	oplus_voice_parse_fb_err();

done:
	return ret;
}
EXPORT_SYMBOL(oplus_voice_get_opve_err_fb);

