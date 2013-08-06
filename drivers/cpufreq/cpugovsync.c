 /* This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/kobject.h>
#include <linux/sysfs.h>

int force_cpu_gov_sync;

/* sysfs interface */
static ssize_t show_force_cpu_gov_sync(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
return sprintf(buf, "%d\n", force_cpu_gov_sync);
}

static ssize_t store_force_cpu_gov_sync(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
sscanf(buf, "%du", &force_cpu_gov_sync);
return count;
}

static struct kobj_attribute force_cpu_gov_sync_attr =
__ATTR(force_cpu_gov_sync, 0666, show_force_cpu_gov_sync, store_force_cpu_gov_sync);

static struct attribute *attrs[] = {
&force_cpu_gov_sync_attr.attr,
NULL,
};

static struct attribute_group attr_group = {
.attrs = attrs,
};

static struct kobject *force_cpu_gov_sync_kobj;

int force_cpu_gov_sync_init(void)
{
	int retval;

	force_cpu_gov_sync = 1;

        force_cpu_gov_sync_kobj = kobject_create_and_add("cpu_gov_sync", kernel_kobj);
        if (!force_cpu_gov_sync_kobj) {
                return -ENOMEM;
        }
        retval = sysfs_create_group(force_cpu_gov_sync_kobj, &attr_group);
        if (retval)
                kobject_put(force_cpu_gov_sync_kobj);
        return retval;
}
/* end sysfs interface */

void force_cpu_gov_sync_exit(void)
{
	kobject_put(force_cpu_gov_sync_kobj);
}

module_init(force_cpu_gov_sync_init);
module_exit(force_cpu_gov_sync_exit);
