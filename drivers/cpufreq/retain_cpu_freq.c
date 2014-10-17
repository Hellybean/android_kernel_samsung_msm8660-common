/*
 * Copyright (c) 2014, Emmanuel Utomi <emmanuelutomi@gmail.com>
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/cpufreq.h>
#include <linux/retain_cpu_freq.h>

struct cpufreq_user_policy user_policy[CONFIG_NR_CPUS];

void retain_cpu_freq_policy(struct cpufreq_policy *policy)
{
	if(policy != NULL){
		user_policy[policy->cpu].min = policy->min;
		user_policy[policy->cpu].max = policy->max;
		user_policy[policy->cpu].set = true;
	}
	else user_policy[policy->cpu].set = false;
}

bool retained_cpu_freq_policy(int cpu)
{
	return user_policy[cpu].set;
}

unsigned int get_retained_min_cpu_freq(int cpu)
{
	return user_policy[cpu].min;
}

unsigned int get_retained_max_cpu_freq(int cpu)
{
	return user_policy[cpu].max;
}

int retain_cpu_freq_init(void)
{
	int cpu;
	for_each_possible_cpu(cpu) user_policy[cpu].set = false;
	
	return 0;
}

module_init(retain_cpu_freq_init);
MODULE_AUTHOR("Emmanuel Utomi <emmanuelutomi@gmail.com>");
