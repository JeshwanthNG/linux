/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * PTP hardware clock driver for the IDT ClockMatrix(TM) family of timing and
 * synchronization devices.
 *
 * Copyright (C) 2019 Integrated Device Technology, Inc., a Renesas Company.
 */
#ifndef PTP_IDTCLOCKMATRIX_H
#define PTP_IDTCLOCKMATRIX_H

#include <linux/ktime.h>
#include <linux/mfd/idt8a340_reg.h>
#include <linux/ptp_clock.h>
#include <linux/regmap.h>

#define FW_FILENAME	"idtcm.bin"
#define MAX_TOD		(4)
#define MAX_PLL		(8)
#define MAX_REF_CLK	(16)

#define MAX_ABS_WRITE_PHASE_NANOSECONDS (107374182L)
#define MAX_FFO_PPB (244000)

#define PHASE_PULL_IN_THRESHOLD_NS_DEPRECATED	(150000)
#define PHASE_PULL_IN_THRESHOLD_NS		(15000)
#define TOD_WRITE_OVERHEAD_COUNT_MAX		(2)
#define TOD_BYTE_COUNT				(11)

#define LOCK_TIMEOUT_MS			(2000)
#define LOCK_POLL_INTERVAL_MS		(10)

#define PHASE_PULL_IN_MAX_PPB		(144000)
#define PHASE_PULL_IN_MIN_THRESHOLD_NS	(2)

/* PTP PLL Mode */
enum ptp_pll_mode {
	PTP_PLL_MODE_MIN = 0,
	PTP_PLL_MODE_WRITE_FREQUENCY = PTP_PLL_MODE_MIN,
	PTP_PLL_MODE_WRITE_PHASE,
	PTP_PLL_MODE_UNSUPPORTED,
	PTP_PLL_MODE_MAX = PTP_PLL_MODE_UNSUPPORTED,
};

struct idtcm;

struct idtcm_channel {
	struct ptp_clock_info	caps;
	struct ptp_clock	*ptp_clock;
	struct idtcm		*idtcm;
	u32			dpll_phase;
	u32			dpll_freq;
	u32			dpll_n;
	u32			dpll_ctrl_n;
	u32			dpll_phase_pull_in;
	u32			tod_read_primary;
	u32			tod_read_secondary;
	u32			tod_write;
	u32			tod_n;
	u32			hw_dpll_n;
	u8			sync_src;
	enum ptp_pll_mode	mode;
	int			(*configure_write_frequency)(struct idtcm_channel *channel);
	int			(*configure_write_phase)(struct idtcm_channel *channel);
	int			(*do_phase_pull_in)(struct idtcm_channel *channel,
						    s32 offset_ns, u32 max_ffo_ppb);
	s32			current_freq_scaled_ppm;
	bool			phase_pull_in;
	u32			dco_delay;
	/* last input trigger for extts */
	u8			refn;
	u8			pll;
	u8			tod;
	u16			output_mask;
};

struct idtcm {
	struct idtcm_channel	channel[MAX_TOD];
	struct device		*dev;
	u8			tod_mask;
	char			version[16];
	enum fw_version		fw_ver;
	/* Polls for external time stamps */
	u8			extts_mask;
	bool			extts_single_shot;
	struct delayed_work	extts_work;
	/* Remember the ptp channel to report extts */
	struct idtcm_channel	*event_channel[MAX_TOD];
	/* Mutex to protect operations from being interrupted */
	struct mutex		*lock;
	struct device		*mfd;
	struct regmap		*regmap;
	/* Overhead calculation for adjtime */
	u8			calculate_overhead_flag;
	s64			tod_write_overhead_ns;
	ktime_t			start_time;
};

#endif /* PTP_IDTCLOCKMATRIX_H */
