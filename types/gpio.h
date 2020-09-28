/*
 * Copyright (c) 2020 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef STRACE_TYPES_GPIO_H
# define STRACE_TYPES_GPIO_H

# include <inttypes.h>
# include <linux/ioctl.h>
# ifdef HAVE_LINUX_GPIO_H
#  include <linux/gpio.h>
# endif

# ifndef GPIOHANDLES_MAX
#  define GPIOHANDLES_MAX 64
# endif

/* added in Linux v5.5 */
# ifndef GPIOHANDLE_SET_CONFIG_IOCTL
#  define GPIOHANDLE_SET_CONFIG_IOCTL _IOWR(0xB4, 0x0A, struct_gpiohandle_config)
# endif

/* added in Linux v5.7 */
# ifndef GPIO_GET_LINEINFO_WATCH_IOCTL
#  define GPIO_GET_LINEINFO_WATCH_IOCTL _IOWR(0xB4, 0x0B, struct gpioline_info)
#  define GPIO_GET_LINEINFO_UNWATCH_IOCTL _IOWR(0xB4, 0x0C, uint32_t)
# endif

typedef struct {
	uint32_t flags;
	uint8_t default_values[GPIOHANDLES_MAX];
	uint32_t padding[4];
} struct_gpiohandle_config;

#endif /* STRACE_TYPES_GPIO_H */
