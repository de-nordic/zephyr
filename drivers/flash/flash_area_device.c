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
	const struct device *const real_dev;
	off_t offset;
	size_t size;
};

struct flash_area_device_priv {
	struct flash_parameters parameters;
	size_t page_count;
};

#define FAD(fadp) ((const struct flash_area_device *)fadp)
#define FAD_PRIV(fadp) ((struct flash_area_device_priv *)fadp)
#define FAD_SIZE(dev) (FAD(dev->config)->size)
#define FAD_PAGE_COUNT(dev) (FAD_PRIV(dev->data)->page_count)
#define FAD_OFFSET(dev, change) (FAD(dev->config)->offset + (change))
#define FAD_PARAMETERS(dev) (FAD_PRIV(dev->data)->parameters)
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
	struct flash_page_info pi;
	int ret_code = 0;
	off_t test_offset = FAD_OFFSET(dev, 0);		/* This is offset on real device */
	const struct flash_parameters *real_dev_params = flash_get_parameters(FAD_REAL_DEV(dev));

	/* Check if device's start offset is aligned with flash page start */
	ret_code = FLASH_API_CALL(FAD_REAL_DEV(dev), get_page_info, test_offset, &pi);
	if (ret_code != 0) {
		/* TODO: Log error, page is not aligned with real device page */
		return ret_code;
	}

	FAD_PARAMETERS(dev).erase_value = real_dev_params->erase_value;
	FAD_PARAMETERS(dev).write_block_size = real_dev_params->write_block_size;

	if (!(real_dev_params->flags & FPF_NON_UNIFORM_LAYOUT)) {
		const size_t real_max_page_size = real_dev_params->max_page_size;

		if (FAD_SIZE(dev) % real_max_page_size) {
			/* TODO: Log error, page is not aligned with real device page */
			return -ERANGE;
		}

		FAD_PARAMETERS(dev).max_page_size = real_max_page_size;
		FAD_PARAMETERS(dev).flags = 0;
		FAD_PAGE_COUNT(dev) = FAD_SIZE(dev) / real_max_page_size;

	} else {
		/* Check if can read last page of the area */
		off_t end_offset = test_offset + FAD_SIZE(dev);
		struct flash_page_info end_pi;

		ret_code = FLASH_API_CALL(FAD_REAL_DEV(dev), get_page_info, end_offset, &end_pi);

		if (ret_code != 0) {
			/* TODO: Log error, probably passing past the device */
			return ret_code;
		}
		/* Check area size is aligned to last page */
		if ((test_offset + FAD_SIZE(dev)) != (end_pi.offset  + end_pi.size)) {
			return -ERANGE;
		}

		/* One page already found */
		FAD_PAGE_COUNT(dev) = 1;
		test_offset += pi.size;

		FAD_PARAMETERS(dev).max_page_size = pi.size;

		do {
			ret_code = FLASH_API_CALL(FAD_REAL_DEV(dev), get_page_info, test_offset, &pi);

			if (ret_code != 0) {
				/* TODO: Log error, page is not aligned with real device page */
				return ret_code;
			}

			test_offset = pi.offset + pi.size;

			if (pi.size > FAD_PARAMETERS(dev).max_page_size) {
				FAD_PARAMETERS(dev).max_page_size = pi.size;
			}

			FAD_PAGE_COUNT(dev)++;

		}  while (FAD_OFFSET(dev, -test_offset) < FAD_SIZE(dev));
	}


	return 0;
}

static int fad_write(const struct device *dev, off_t offset, const void *data, size_t len)
{
	if (is_within_fad_range(dev, offset, len)) {
		return FLASH_API_CALL(FAD_REAL_DEV(dev), write, FAD_OFFSET(dev, offset), data, len);
	}
	return -EINVAL;
}

static int fad_read(const struct device *dev, off_t offset, void *data, size_t len)
{
	if (is_within_fad_range(dev, offset, len)) {
		return FLASH_API_CALL(FAD_REAL_DEV(dev), read, FAD_OFFSET(dev, offset), data, len);
	}

	return -EINVAL;
}

static int fad_erase(const struct device *dev, off_t offset, size_t len)
{
	if (is_within_fad_range(dev, offset, len)) {
		return 0;
	}

	return -EINVAL;
}

static int fad_get_page_info(const struct device *dev, off_t offset, struct flash_page_info *fpi)
{
	if (is_within_fad_range(dev, offset, 1)) {
		return FLASH_API_CALL(FAD_REAL_DEV(dev), get_page_info, FAD_OFFSET(dev, offset),
				      fpi);
	}

	return -EINVAL;
}

static size_t fad_get_size(const struct device *dev)
{
	return FAD_SIZE(dev);
}

static size_t fad_get_page_count(const struct device *dev)
{
	return FAD_PAGE_COUNT(dev);
}

static const struct flash_parameters *fad_get_parameters(const struct device *dev)
{
	return &FAD_PARAMETERS(dev);
}

static const struct flash_driver_api fad_api = {
	.read = fad_read,
	.write = fad_write,
	.erase = fad_erase,
	.get_parameters = fad_get_parameters,
	.get_page_info = fad_get_page_info,
	.get_page_count = fad_get_page_count,
	.get_size = fad_get_size,
};

#define FLASH_AREA_DEVICE_GEN(node)							\
	static struct flash_area_device_priv fad_priv_##node;				\
	static const struct flash_area_device fad_##node = {				\
		.real_dev = DEVICE_DT_GET(DT_MTD_FROM_FIXED_PARTITION(node)),		\
		.offset = DT_REG_ADDR(node),						\
		.size = DT_REG_SIZE(node),						\
	};										\
	DEVICE_DT_DEFINE(node, fad_init, NULL, &fad_priv_##node, &fad_##node, POST_KERNEL,	\
			 CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &fad_api);

#define FOREACH_PARTITION(n) DT_FOREACH_CHILD(DT_DRV_INST(n), FLASH_AREA_DEVICE_GEN)

DT_INST_FOREACH_STATUS_OKAY(FOREACH_PARTITION);
