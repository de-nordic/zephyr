/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT fixed_partitions

/*
 * The flash area device is not a real device, this is range checker over real flash device.
 */

#include <device.h>
#include <drivers/flash.h>
#include <zephyr/types.h>
#include <init.h>
#include <kernel.h>
#include <sys/util.h>

struct flash_area_device {
	const struct device *real_dev;
	off_t offset;
	size_t size;
};

#define FAD(fadp) ((const struct flash_area_device *)fadp)
#define FAD_SIZE(dev) (FAD(dev->config)->size)
#define FAD_OFFSET(dev) (FAD(dev->config)->offset)
#define FAD_REAL_DEV(dev) (FAD(dev->config)->real_dev)
#define FLASH_API(dev) ((const struct flash_driver_api *)dev->api)

#define FLASH_API_CALL(dev, ...) \
	FLASH_API(dev)->GET_ARG_N(1, __VA_ARGS__)(dev, GET_ARGS_LESS_N(1, __VA_ARGS__))

static inline bool is_within_fad_range(const struct device *dev, off_t offset, size_t len)
{
	return (offset >= 0) && ((offset + len) <= FAD_SIZE(dev));
}

static int fad_init(const struct device *dev)
{
	return 0;
}

static int fad_write(const struct device *dev, const off_t offset, const void *data,
		     const size_t len)
{
	if (is_within_fad_range(dev, offset, len)) {
		return FLASH_API_CALL(FAD_REAL_DEV(dev), write, offset, data, len);
	}
	return -EINVAL;
}

static int fad_read(const struct device *dev, const off_t offset, void *data, const size_t len)
{
	if (is_within_fad_range(dev, offset, len)) {
		return FLASH_API_CALL(FAD_REAL_DEV(dev), read, offset, data, len);
	}
	return -EINVAL;
}


static int fad_erase(const struct device *dev, const off_t offset, const size_t len)
{
	if (is_within_fad_range(dev, offset, len)) {
		return 0;
	}
	return -EINVAL;
}

static void fad_get_layout(const struct device *dev, const struct flash_pages_layout **layout,
			   size_t *layout_size)
{
	return;
}

static const struct flash_parameters *fad_get_parameters(const struct device *dev)
{
	return FLASH_API(FAD_REAL_DEV(dev))->get_parameters(FAD_REAL_DEV(dev));
}

static const struct flash_driver_api fad_api = {
	.read = fad_read,
	.write = fad_write,
	.erase = fad_erase,
	.get_parameters = fad_get_parameters,
#ifdef CONFIG_FLASH_PAGE_LAYOUT
	.page_layout = fad_get_layout
#endif
};

#define FLASH_AREA_DEVICE_GEN(node)							\
	static const struct flash_area_device fad_##node = {				\
		.real_dev = DEVICE_DT_GET(DT_MTD_FROM_FIXED_PARTITION(node)),		\
		.offset = DT_REG_ADDR(node),						\
		.size = DT_REG_SIZE(node)						\
	};										\
	DEVICE_DT_DEFINE(node, fad_init, NULL, NULL, &fad_##node, POST_KERNEL,		\
			 CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &fad_api);

#define FOREACH_PARTITION(n) DT_FOREACH_CHILD(DT_DRV_INST(n), FLASH_AREA_DEVICE_GEN)

DT_INST_FOREACH_STATUS_OKAY(FOREACH_PARTITION);
