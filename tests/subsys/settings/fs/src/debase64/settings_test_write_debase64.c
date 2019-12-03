/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "settings_test.h"
#include "settings/settings_file.h"
#include "settings_priv_debase64.h"

static int test_config_save_one_byte_value(const char *name, u8_t val)
{
	return settings_save_one(name, &val, 1);
}

void test_config_write_debase64(void)
{
	int rc;
	struct settings_file cf;

	config_wipe_srcs();
	rc = fs_mkdir(TEST_CONFIG_DIR);
	zassert_true(rc == 0 || rc == -EEXIST, "can't create directory");

	cf.cf_name = TEST_CONFIG_DIR "/write";
	cf.cf_maxlines = 1000;
	cf.cf_lines = 0; /* normally fetched while loading, but this is test */
	cf.cf_b64_marker = false;

	rc = settings_file_src(&cf);
	zassert_true(rc == 0, "can't register FS as configuration source");

	rc = settings_file_dst(&cf);
	zassert_true(rc == 0,
		     "can't register FS as configuration destination");

	/* Write original line BASE64 encoded */
	val8 = 0;
	EXPECT_NOERR(fsutil_write_file(cf.cf_name,
		STR_SIZE_TUPLE(CF_B64_MYFOO_MYBAR_1)), "can't write to file");

	/* Read BASE64 encoded value */
	EXPECT_NOERR(settings_load(), "fs readout error");
	zassert_equal(val8, 1U, "bad value read");

	/* Add new value, non-BASE64 encoded, preceded by marker */
	val8 = 33U;
	EXPECT_NOERR(settings_save(), "fs write error");
	val8 = 0U;
	EXPECT_NOERR(settings_load(), "fs readout error");
	zassert_equal(val8, 33U, "bad value read");

	/* Add one more value afterwards, check if latest get read */
	EXPECT_NOERR(test_config_save_one_byte_value("myfoo/mybar", 42),
		"fs one item write error");
	val8 = 0;
	EXPECT_NOERR(settings_load(), "fs redout error");
	zassert_equal(val8, 42U, "bad value read");
}
