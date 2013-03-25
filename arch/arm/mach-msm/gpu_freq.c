/*
 * Author: Dave Carey <carvsdriver@gmail.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Purpose: This interface allows for user control over the GPU
 * clock settings.  This assumes that the appropriate modifications
 * have been made to suppor GPU over clocking in multiple phases.
 *
 * To do: Document over clock settings here ...
 */

#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/gpu_freq.h>
#include <linux/slab.h>
#include <linux/module.h>

int gpu_2d_freq_phase;
int gpu_3d_freq_phase;

/* sysfs interface - allow for independent control of 2D & 3D frequencies */
static ssize_t gpu_freq_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	if(strcmp(attr->attr.name, "gpu_3d_freq_phase") == 0) {
		return sprintf(buf, "%d\n", gpu_3d_freq_phase);
	} else {
		return sprintf(buf, "%d\n", gpu_2d_freq_phase);
	}
}

static ssize_t gpu_freq_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	/*
	 * Do some basic sanity checks on the values being
	 * captured so we don't run out of the expected
	 * range for either frequency table
	 */

	if (strcmp(attr->attr.name, "gpu_3d_freq_phase") == 0) {
		sscanf(buf, "%du", &gpu_3d_freq_phase);
		if(gpu_3d_freq_phase > KGSL_3D_MIN_PHASE)
			gpu_3d_freq_phase = KGSL_3D_MIN_PHASE;

		if(gpu_3d_freq_phase < 0)
			gpu_3d_freq_phase = 0;
	} else {
                sscanf(buf, "%du", &gpu_2d_freq_phase);
                if(gpu_2d_freq_phase > KGSL_2D_MIN_PHASE)
                        gpu_2d_freq_phase = KGSL_2D_MIN_PHASE;
        
                if(gpu_3d_freq_phase < 0)
                        gpu_2d_freq_phase = 0;
	}
	return count;
}

static struct kobj_attribute gpu_2d_attribute =
	__ATTR(gpu_2d_freq_phase, 0666, gpu_freq_show, gpu_freq_store);

static struct kobj_attribute gpu_3d_attribute =
	__ATTR(gpu_3d_freq_phase, 0666, gpu_freq_show, gpu_freq_store);

static struct attribute *gpu_control_attributes[] = {
	&gpu_2d_attribute.attr,
	&gpu_3d_attribute.attr,
	NULL,
};

static struct attribute_group gpu_control_attributes_group = {
	.attrs = gpu_control_attributes,
};

static struct kobject *gpu_control_kobj;

static int __init gpu_control_init(void)
{
	int retval;

	gpu_2d_freq_phase = KGSL_2D_MAX_PHASE;
	gpu_3d_freq_phase = KGSL_3D_MAX_PHASE;

        gpu_control_kobj = kobject_create_and_add("gpu_control", kernel_kobj);
        if (!gpu_control_kobj) {
                return -ENOMEM;
        }
        retval = sysfs_create_group(gpu_control_kobj, &gpu_control_attributes_group);
        if (retval)
                kobject_put(gpu_control_kobj);
        return retval;
}
/* end sysfs interface */

static void __exit gpu_control_exit(void)
{
	kobject_put(gpu_control_kobj);
}

module_init(gpu_control_init);
module_exit(gpu_control_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dave Carey <carvsdriver@gmail.com>");
