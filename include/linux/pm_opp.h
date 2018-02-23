/*
 * Generic OPP Interface
 *
 * Copyright (C) 2009-2010 Texas Instruments Incorporated.
 *	Nishanth Menon
 *	Romit Dasgupta
 *	Kevin Hilman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __LINUX_OPP_H__
#define __LINUX_OPP_H__

#include <linux/err.h>
#include <linux/notifier.h>
#include <linux/kref.h>

struct clk;
struct regulator;
struct device;
struct opp_table;

enum dev_pm_opp_event {
	OPP_EVENT_ADD, OPP_EVENT_REMOVE, OPP_EVENT_ENABLE, OPP_EVENT_DISABLE,
};

/**
 * struct dev_pm_opp - Generic OPP description structure
 * @node:	opp table node. The nodes are maintained throughout the lifetime
 *		of boot. It is expected only an optimal set of OPPs are
 *		added to the library by the SoC framework.
 *		IMPORTANT: the opp nodes should be maintained in increasing
 *		order.
 * @kref:	for reference count of the OPP.
 * @available:	true/false - marks if this OPP as available or not
 * @dynamic:	not-created from static DT entries.
 * @turbo:	true if turbo (boost) OPP
 * @suspend:	true if suspend OPP
 * @rate:	Frequency in hertz
 * @supplies:	Power supplies voltage/current values
 * @clock_latency_ns: Latency (in nanoseconds) of switching to this OPP's
 *		frequency from any other OPP's frequency.
 * @opp_table:	points back to the opp_table struct this opp belongs to
 * @np:		OPP's device node.
 * @dentry:	debugfs dentry pointer (per opp)
 *
 * This structure stores the OPP information for a given device.
 */
struct dev_pm_opp {
	struct list_head node;
	struct kref kref;

	bool available;
	bool dynamic;
	bool turbo;
	bool suspend;
	unsigned long rate;

	struct dev_pm_opp_supply *supplies;

	unsigned long clock_latency_ns;

	struct opp_table *opp_table;

	struct device_node *np;

#ifdef CONFIG_DEBUG_FS
	struct dentry *dentry;
#endif
};

/**
 * struct dev_pm_opp_supply - Power supply voltage/current values
 * @u_volt:	Target voltage in microvolts corresponding to this OPP
 * @u_volt_min:	Minimum voltage in microvolts corresponding to this OPP
 * @u_volt_max:	Maximum voltage in microvolts corresponding to this OPP
 * @u_amp:	Maximum current drawn by the device in microamperes
 *
 * This structure stores the voltage/current values for a single power supply.
 */
struct dev_pm_opp_supply {
	unsigned long u_volt;
	unsigned long u_volt_min;
	unsigned long u_volt_max;
	unsigned long u_amp;
};

/**
 * struct dev_pm_opp_info - OPP freq/voltage/current values
 * @rate:	Target clk rate in hz
 * @supplies:	Array of voltage/current values for all power supplies
 *
 * This structure stores the freq/voltage/current values for a single OPP.
 */
struct dev_pm_opp_info {
	unsigned long rate;
	struct dev_pm_opp_supply *supplies;
};

/**
 * struct dev_pm_set_opp_data - Set OPP data
 * @old_opp:	Old OPP info
 * @new_opp:	New OPP info
 * @regulators:	Array of regulator pointers
 * @regulator_count: Number of regulators
 * @clk:	Pointer to clk
 * @dev:	Pointer to the struct device
 *
 * This structure contains all information required for setting an OPP.
 */
struct dev_pm_set_opp_data {
	struct dev_pm_opp_info old_opp;
	struct dev_pm_opp_info new_opp;

	struct regulator **regulators;
	unsigned int regulator_count;
	struct clk *clk;
	struct device *dev;
};

#if defined(CONFIG_PM_OPP)

struct opp_table *dev_pm_opp_get_opp_table(struct device *dev);
void dev_pm_opp_put_opp_table(struct opp_table *opp_table);

unsigned long dev_pm_opp_get_voltage(struct dev_pm_opp *opp);

unsigned long dev_pm_opp_get_freq(struct dev_pm_opp *opp);

bool dev_pm_opp_is_turbo(struct dev_pm_opp *opp);

int dev_pm_opp_get_opp_count(struct device *dev);
unsigned long dev_pm_opp_get_max_clock_latency(struct device *dev);
unsigned long dev_pm_opp_get_max_volt_latency(struct device *dev);
unsigned long dev_pm_opp_get_max_transition_latency(struct device *dev);
unsigned long dev_pm_opp_get_suspend_opp_freq(struct device *dev);

struct dev_pm_opp *dev_pm_opp_find_freq_exact(struct device *dev,
					      unsigned long freq,
					      bool available);

struct dev_pm_opp *dev_pm_opp_find_freq_floor(struct device *dev,
					      unsigned long *freq);

struct dev_pm_opp *dev_pm_opp_find_freq_ceil(struct device *dev,
					     unsigned long *freq);
void dev_pm_opp_put(struct dev_pm_opp *opp);

int dev_pm_opp_add(struct device *dev, unsigned long freq,
		   unsigned long u_volt);
void dev_pm_opp_remove(struct device *dev, unsigned long freq);

int dev_pm_opp_enable(struct device *dev, unsigned long freq);

int dev_pm_opp_disable(struct device *dev, unsigned long freq);

int dev_pm_opp_register_notifier(struct device *dev, struct notifier_block *nb);
int dev_pm_opp_unregister_notifier(struct device *dev, struct notifier_block *nb);

struct opp_table *dev_pm_opp_set_supported_hw(struct device *dev, const u32 *versions, unsigned int count);
void dev_pm_opp_put_supported_hw(struct opp_table *opp_table);
struct opp_table *dev_pm_opp_set_prop_name(struct device *dev, const char *name);
void dev_pm_opp_put_prop_name(struct opp_table *opp_table);
struct opp_table *dev_pm_opp_set_regulators(struct device *dev, const char * const names[], unsigned int count);
void dev_pm_opp_put_regulators(struct opp_table *opp_table);
struct opp_table *dev_pm_opp_set_clkname(struct device *dev, const char * name);
void dev_pm_opp_put_clkname(struct opp_table *opp_table);
struct opp_table *dev_pm_opp_register_set_opp_helper(struct device *dev, int (*set_opp)(struct dev_pm_set_opp_data *data));
void dev_pm_opp_register_put_opp_helper(struct opp_table *opp_table);
int dev_pm_opp_set_rate(struct device *dev, unsigned long target_freq);
int dev_pm_opp_set_sharing_cpus(struct device *cpu_dev, const struct cpumask *cpumask);
int dev_pm_opp_get_sharing_cpus(struct device *cpu_dev, struct cpumask *cpumask);
void dev_pm_opp_remove_table(struct device *dev);
void dev_pm_opp_cpumask_remove_table(const struct cpumask *cpumask);
#else
static inline struct opp_table *dev_pm_opp_get_opp_table(struct device *dev)
{
	return ERR_PTR(-ENOTSUPP);
}

static inline void dev_pm_opp_put_opp_table(struct opp_table *opp_table) {}

static inline unsigned long dev_pm_opp_get_voltage(struct dev_pm_opp *opp)
{
	return 0;
}

static inline unsigned long dev_pm_opp_get_freq(struct dev_pm_opp *opp)
{
	return 0;
}

static inline bool dev_pm_opp_is_turbo(struct dev_pm_opp *opp)
{
	return false;
}

static inline int dev_pm_opp_get_opp_count(struct device *dev)
{
	return 0;
}

static inline unsigned long dev_pm_opp_get_max_clock_latency(struct device *dev)
{
	return 0;
}

static inline unsigned long dev_pm_opp_get_max_volt_latency(struct device *dev)
{
	return 0;
}

static inline unsigned long dev_pm_opp_get_max_transition_latency(struct device *dev)
{
	return 0;
}

static inline unsigned long dev_pm_opp_get_suspend_opp_freq(struct device *dev)
{
	return 0;
}

static inline struct dev_pm_opp *dev_pm_opp_find_freq_exact(struct device *dev,
					unsigned long freq, bool available)
{
	return ERR_PTR(-ENOTSUPP);
}

static inline struct dev_pm_opp *dev_pm_opp_find_freq_floor(struct device *dev,
					unsigned long *freq)
{
	return ERR_PTR(-ENOTSUPP);
}

static inline struct dev_pm_opp *dev_pm_opp_find_freq_ceil(struct device *dev,
					unsigned long *freq)
{
	return ERR_PTR(-ENOTSUPP);
}

static inline void dev_pm_opp_put(struct dev_pm_opp *opp) {}

static inline int dev_pm_opp_add(struct device *dev, unsigned long freq,
					unsigned long u_volt)
{
	return -ENOTSUPP;
}

static inline void dev_pm_opp_remove(struct device *dev, unsigned long freq)
{
}

static inline int dev_pm_opp_enable(struct device *dev, unsigned long freq)
{
	return 0;
}

static inline int dev_pm_opp_disable(struct device *dev, unsigned long freq)
{
	return 0;
}

static inline int dev_pm_opp_register_notifier(struct device *dev, struct notifier_block *nb)
{
	return -ENOTSUPP;
}

static inline int dev_pm_opp_unregister_notifier(struct device *dev, struct notifier_block *nb)
{
	return -ENOTSUPP;
}

static inline struct opp_table *dev_pm_opp_set_supported_hw(struct device *dev,
							    const u32 *versions,
							    unsigned int count)
{
	return ERR_PTR(-ENOTSUPP);
}

static inline void dev_pm_opp_put_supported_hw(struct opp_table *opp_table) {}

static inline struct opp_table *dev_pm_opp_register_set_opp_helper(struct device *dev,
			int (*set_opp)(struct dev_pm_set_opp_data *data))
{
	return ERR_PTR(-ENOTSUPP);
}

static inline void dev_pm_opp_register_put_opp_helper(struct opp_table *opp_table) {}

static inline struct opp_table *dev_pm_opp_set_prop_name(struct device *dev, const char *name)
{
	return ERR_PTR(-ENOTSUPP);
}

static inline void dev_pm_opp_put_prop_name(struct opp_table *opp_table) {}

static inline struct opp_table *dev_pm_opp_set_regulators(struct device *dev, const char * const names[], unsigned int count)
{
	return ERR_PTR(-ENOTSUPP);
}

static inline void dev_pm_opp_put_regulators(struct opp_table *opp_table) {}

static inline struct opp_table *dev_pm_opp_set_clkname(struct device *dev, const char * name)
{
	return ERR_PTR(-ENOTSUPP);
}

static inline void dev_pm_opp_put_clkname(struct opp_table *opp_table) {}

static inline int dev_pm_opp_set_rate(struct device *dev, unsigned long target_freq)
{
	return -ENOTSUPP;
}

static inline int dev_pm_opp_set_sharing_cpus(struct device *cpu_dev, const struct cpumask *cpumask)
{
	return -ENOTSUPP;
}

static inline int dev_pm_opp_get_sharing_cpus(struct device *cpu_dev, struct cpumask *cpumask)
{
	return -EINVAL;
}

static inline void dev_pm_opp_remove_table(struct device *dev)
{
}

static inline void dev_pm_opp_cpumask_remove_table(const struct cpumask *cpumask)
{
}

#endif		/* CONFIG_PM_OPP */

#if defined(CONFIG_PM_OPP) && defined(CONFIG_OF)
int dev_pm_opp_of_add_table(struct device *dev);
void dev_pm_opp_of_remove_table(struct device *dev);
int dev_pm_opp_of_cpumask_add_table(const struct cpumask *cpumask);
void dev_pm_opp_of_cpumask_remove_table(const struct cpumask *cpumask);
int dev_pm_opp_of_get_sharing_cpus(struct device *cpu_dev, struct cpumask *cpumask);
struct device_node *dev_pm_opp_of_get_opp_desc_node(struct device *dev);
#else
static inline int dev_pm_opp_of_add_table(struct device *dev)
{
	return -ENOTSUPP;
}

static inline void dev_pm_opp_of_remove_table(struct device *dev)
{
}

static inline int dev_pm_opp_of_cpumask_add_table(const struct cpumask *cpumask)
{
	return -ENOTSUPP;
}

static inline void dev_pm_opp_of_cpumask_remove_table(const struct cpumask *cpumask)
{
}

static inline int dev_pm_opp_of_get_sharing_cpus(struct device *cpu_dev, struct cpumask *cpumask)
{
	return -ENOTSUPP;
}

static inline struct device_node *dev_pm_opp_of_get_opp_desc_node(struct device *dev)
{
	return NULL;
}
#endif

#endif		/* __LINUX_OPP_H__ */
