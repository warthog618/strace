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

# ifndef GPIO_V2_GET_LINEINFO_IOCTL
/* added in Linux v5.10 */
#  define GPIO_V2_GET_LINEINFO_IOCTL _IOWR(0xB4, 0x05, struct_gpio_v2_line_info)
#  define GPIO_V2_GET_LINEINFO_WATCH_IOCTL _IOWR(0xB4, 0x06, struct_gpio_v2_line_info)
#  define GPIO_V2_GET_LINE_IOCTL _IOWR(0xB4, 0x07, struct_gpio_v2_line_request)
#  define GPIO_V2_LINE_SET_CONFIG_IOCTL _IOWR(0xB4, 0x0D, struct_gpio_v2_line_config)
#  define GPIO_V2_LINE_GET_VALUES_IOCTL _IOWR(0xB4, 0x0E, struct_gpio_v2_line_values)
#  define GPIO_V2_LINE_SET_VALUES_IOCTL _IOWR(0xB4, 0x0F, struct_gpio_v2_line_values)

#  define GPIO_MAX_NAME_SIZE 32
#  define GPIO_V2_LINES_MAX 64
#  define GPIO_V2_LINE_NUM_ATTRS_MAX 10

enum gpio_v2_line_flag {
	GPIO_V2_LINE_FLAG_ACTIVE_LOW		= (1ULL << 1),
	GPIO_V2_LINE_FLAG_BIAS_PULL_UP		= (1ULL << 8),
};

enum gpio_v2_line_attr_id {
	GPIO_V2_LINE_ATTR_ID_FLAGS		= 1,
	GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES	= 2,
	GPIO_V2_LINE_ATTR_ID_DEBOUNCE		= 3,
};

# endif /* GPIO_V2_GET_LINEINFO_IOCTL */

typedef struct {
	uint64_t bits;
	uint64_t mask;
} struct_gpio_v2_line_values;

typedef struct {
	uint32_t id;
	uint32_t padding;
	union {
		uint64_t flags;
		uint64_t values;
		uint32_t debounce_period_us;
	};
} struct_gpio_v2_line_attribute;

typedef struct {
	struct_gpio_v2_line_attribute attr;
	uint64_t mask;
} struct_gpio_v2_line_config_attribute;

typedef struct {
	uint64_t flags;
	uint32_t num_attrs;
	uint32_t padding[5];
	struct_gpio_v2_line_config_attribute attrs[GPIO_V2_LINE_NUM_ATTRS_MAX];
} struct_gpio_v2_line_config;

typedef struct {
	uint32_t offsets[GPIO_V2_LINES_MAX];
	char consumer[GPIO_MAX_NAME_SIZE];
	struct_gpio_v2_line_config config;
	uint32_t num_lines;
	uint32_t event_buffer_size;
	uint32_t padding[5];
	int32_t fd;
} struct_gpio_v2_line_request;

typedef struct {
	char name[GPIO_MAX_NAME_SIZE];
	char consumer[GPIO_MAX_NAME_SIZE];
	uint32_t offset;
	uint32_t num_attrs;
	uint64_t flags;
	struct_gpio_v2_line_attribute attrs[GPIO_V2_LINE_NUM_ATTRS_MAX];
	uint32_t padding[4];
} struct_gpio_v2_line_info;

#endif /* STRACE_TYPES_GPIO_H */
