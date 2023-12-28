/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2012-2023, The Linux Foundation. All rights reserved.
 */
#ifndef __OPLUS_OPVE_ERR_FB_H__
#define __OPLUS_OPVE_ERR_FB_H__
#include <sound/soc.h>
#include <dsp/q6voice.h>

#define OPVE_TX_MODULE_ID            (0x10080B0B)  /* 268962571 */
#define OPVE_RX_MODULE_ID            (0x10090B0B)  /* 269028107 */
/* OPVE Tx 1-mic Topology ID */
#define VOICE_TOPOLOGY_OPVE_TX_SM    (0x1000BFF0)  /* 268484592 */
/* OPVE Tx 2-mic Topology ID */
#define VOICE_TOPOLOGY_OPVE_TX_DM    (0x1000BFF1)  /* 268484593 */
/* OPVE Tx 3-mic Topology ID */
#define VOICE_TOPOLOGY_OPVE_TX_QM    (0x1000BFF3)  /* 268484595 */
/* VOICE OPVE Rx Topology ID */
#define VOICE_TOPOLOGY_OPVE_RX       (0x1000BFF2)  /* 268484594 */
/* AUDIO OPVE Rx Topology ID */
#define AUDIO_TOPOLOGY_OPVE_RX       (0x1000BFF4)  /* 268484596 */

#define VOICE_PARAM_OPVE_GET_ERR     (0x10080B2B)  /* 268962603 */

#define VOICE_OR_VOIP_APP_TYPE         (0x0001113A)  /* 69946 */

void oplus_opve_err_fb_add_controls(struct snd_soc_component *component);
void oplus_opve_copy_voice_err_result(int port_type, void *presult, uint32_t len);
int oplus_adm_get_opve_err_fb(int port_id, int copp_idx);
int oplus_voice_get_opve_err_fb(void *apr_cvp, struct voice_data *v);

#endif /* __OPLUS_OPVE_ERR_FB_H__ */
