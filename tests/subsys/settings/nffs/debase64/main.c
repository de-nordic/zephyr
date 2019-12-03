/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>

#include "settings_test_fs.h"
#include "settings_test.h"
#include "settings_priv.h"

void config_setup_nffs(void);

void test_main(void)
{
	ztest_test_suite(test_config,
		/* Config tests */
		ztest_unit_test(config_empty_lookups),
		ztest_unit_test(test_config_insert),

		/* Littlefs as backing storage. */
		ztest_unit_test(config_setup_nffs),
		ztest_unit_test(test_config_empty_file),
		ztest_unit_test(test_config_small_file_debase64),
		ztest_unit_test(test_config_write_debase64),
		ztest_unit_test(test_config_compress_debase64)
	);

	ztest_run_test_suite(test_config);
}
