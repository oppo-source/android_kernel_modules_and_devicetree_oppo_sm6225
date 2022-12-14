/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2020 Oplus. All rights reserved.
 */


#ifndef _OPLUS_WORKQUEUE_H_
#define _OPLUS_WORKQUEUE_H_

#ifdef OPLUS_FEATURE_SCHED_ASSIST

struct worker;
#ifdef CONFIG_OPLUS_SYSTEM_KERNEL_QCOM
int is_uxwork(struct work_struct *work);
inline int set_uxwork(struct work_struct *work);
inline int unset_uxwork(struct work_struct *work);
inline void set_ux_worker_task(struct task_struct *task);
inline void reset_ux_worker_task(struct task_struct *task);
#else /* CONFIG_OPLUS_SYSTEM_KERNEL_QCOM */
static inline int is_uxwork(struct work_struct *work) { return false; }
static inline int set_uxwork(struct work_struct *work) { return false; }
static inline int unset_uxwork(struct work_struct *work) { return false; }
static inline void set_ux_worker_task(struct task_struct *task) {}
static inline void reset_ux_worker_task(struct task_struct *task) {}
#endif /* CONFIG_OPLUS_SYSTEM_KERNEL_QCOM */

#endif /* OPLUS_FEATURE_SCHED_ASSIST */
#endif /* _OPLUS_WORKQUEUE_H_ */
