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
 */


#ifndef _LINUX_GPUCONTROL_H
#define _LINUX_GPUCONTROL_H

extern int gpu_2d_freq_phase;
extern int gpu_3d_freq_phase;

// initialize at max power
#define KGSL_3D_MAX_PHASE 0
#define KGSL_2D_MAX_PHASE 0

#define KGSL_3D_MIN_PHASE 6
#define KGSL_2D_MIN_PHASE 3

#endif
