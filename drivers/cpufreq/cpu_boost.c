/*
 * Copyright (c) 2014, Sultanxda <sultanxda@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/cpu.h>
#include <linux/cpu_boost.h>
#include <linux/cpufreq.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

static struct delayed_work boost_work;

static DECLARE_COMPLETION(cpu_boost_no_timeout);

static unsigned int enabled = 1;
static unsigned int boost_duration_ms = 0;
static unsigned int boost_freq_khz = 0;
static unsigned int boost_override = 0;
static unsigned int cpu_boosted = 0;
static unsigned int init_done = 0;
static unsigned int maxfreq_orig = 0;
static unsigned int minfreq_orig = 0;

void cpu_boost_timeout(unsigned int freq_mhz, unsigned int duration_ms)
{
	if (init_done && enabled) {
		if (cpu_boosted) {
			cpu_boosted = 0;
			boost_override = 1;
			cancel_delayed_work(&boost_work);
		}

		boost_freq_khz = freq_mhz * 1000;
		boost_duration_ms = duration_ms;
		schedule_delayed_work(&boost_work, 0);
	}
}

void cpu_boost(unsigned int freq_mhz)
{
	if (init_done && enabled) {
		if (cpu_boosted) {
			cpu_boosted = 0;
			boost_override = 1;
			cancel_delayed_work(&boost_work);
		}

		init_completion(&cpu_boost_no_timeout);
		boost_freq_khz = freq_mhz * 1000;
		schedule_delayed_work(&boost_work, 0);
	}
}

void cpu_unboost(void)
{
	if (init_done && enabled)
		complete(&cpu_boost_no_timeout);
}

static void save_original_cpu_limits(void)
{
	struct cpufreq_policy *policy = cpufreq_cpu_get(0);

	minfreq_orig = policy->user_policy.min;
	maxfreq_orig = policy->user_policy.max;

	cpufreq_cpu_put(policy);
}

static void set_new_minfreq(struct cpufreq_policy *policy,
	unsigned int minfreq)
{
	policy->user_policy.min = minfreq;
}

static void restore_original_minfreq(void)
{
	struct cpufreq_policy *policy = NULL;
	unsigned int cpu = 0;

	for_each_online_cpu(cpu) {
		policy = cpufreq_cpu_get(cpu);
		set_new_minfreq(policy, minfreq_orig);
		cpufreq_update_policy(cpu);
		cpufreq_cpu_put(policy);
	}

	boost_duration_ms = 0;
	cpu_boosted = 0;
	boost_override = 0;
}

static void __cpuinit cpu_boost_main(struct work_struct *work)
{
	struct cpufreq_policy *policy = NULL;
	unsigned int cpu = 0, minfreq = 0, wait_ms = 0;

	if (cpu_boosted) {
		restore_original_minfreq();
		return;
	}

	if (!boost_override)
		save_original_cpu_limits();

	if (boost_freq_khz) {
		if (boost_freq_khz >= maxfreq_orig) {
			if (maxfreq_orig <= 486000) {
				boost_duration_ms = 0;
				boost_override = 0;
				return;
			} else
				minfreq = maxfreq_orig - 108000;
		} else
			minfreq = boost_freq_khz;

		/* boost online CPUs */
		for_each_online_cpu(cpu) {
			policy = cpufreq_cpu_get(cpu);
			set_new_minfreq(policy, minfreq);
			cpufreq_update_policy(cpu);
			cpufreq_cpu_put(policy);
		}
		cpu_boosted = 1;
	}

	if (boost_duration_ms)
		wait_ms = boost_duration_ms;
	else
		wait_for_completion(&cpu_boost_no_timeout);


	schedule_delayed_work(&boost_work,
				msecs_to_jiffies(wait_ms));
}

void cpu_boost_shutdown(void){
	enabled = 0;
}

void cpu_boost_startup(void){
	enabled = 1;
}

static ssize_t cpu_boost_enabled_status_read(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", enabled);
}

static ssize_t cpu_boost_enabled_status_write(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned int data;
	if(sscanf(buf, "%u\n", &data) == 1) {
		if (data == 1) cpu_boost_startup();
		else if (data == 0) cpu_boost_shutdown();
	}
	return size;
}

static struct kobj_attribute cpu_boost_enabled = __ATTR(enabled, 0666, cpu_boost_enabled_status_read, cpu_boost_enabled_status_write);


static struct attribute *cpu_boost_attributes[] = {
	&cpu_boost_enabled.attr,
	NULL
};

static struct attribute_group cpu_boost_attr_group = {
    .attrs = cpu_boost_attributes,
};

struct kobject *cpu_boost_kobject;


static int __init cpu_boost_init(void)
{
	int retval;

	INIT_DELAYED_WORK(&boost_work, cpu_boost_main);

	cpu_boost_kobject = kobject_create_and_add("cpu_boost", kernel_kobj);
	if (cpu_boost_kobject) retval = sysfs_create_group(cpu_boost_kobject, &cpu_boost_attr_group);
	if (retval) kobject_put(cpu_boost_kobject);
	init_done = 1;

	return 0;
}
late_initcall(cpu_boost_init);

MODULE_AUTHOR("Sultanxda <sultanxda@gmail.com>");
MODULE_DESCRIPTION("CPU-boost framework");
MODULE_LICENSE("GPLv2");
