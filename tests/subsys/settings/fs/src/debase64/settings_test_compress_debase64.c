/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "settings_test.h"
#include "settings/settings_file.h"
#include "settings_priv_debase64.h"

#define MULTIPLE 	CF_B64_MYFOO_MYBAR_1\
			CF_B64_MYFOO_MYBAR_1\
			CF_B64_MYFOO_MYBAR_1\
			CF_B64_MYFOO_MYBAR_1\
			CF_B64_MYFOO_MYBAR_1\
			CF_B64_MYFOO_MYBAR_1\
			CF_B64_MYFOO_MYBAR_1\
			CF_B64_MYFOO_MYBAR_1\
			CF_B64_MYFOO_MYBAR_1

void test_config_compress_debase64(void)
{
	int rc;
	struct settings_file cf;

	config_wipe_srcs();
	rc = fs_mkdir(TEST_CONFIG_DIR);
	zassert_true(rc == 0 || rc == -EEXIST, "can't create directory");

	cf.cf_name = TEST_CONFIG_DIR "/compress";
	cf.cf_maxlines = 10;
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
		STR_SIZE_TUPLE(MULTIPLE)), "can't write to file");

	/* Read BASE64 encoded value */
	EXPECT_NOERR(settings_load(), "fs readout error");
	zassert_equal(val8, 1U, "bad val8 read");
	zassert_equal(cf.cf_lines, 9U, "unexpected number of lines");

	/* Add new value, hopefully non-BASE64 encoded */
	/* This will be written with marker, so two lines + there are
	 * val16 and val64 also written, so 4 lines.
	 */
	val8 = 33U;
	val16 = 666U;
	val64 = 6400640000640064U;
	EXPECT_NOERR(settings_save(), "fs write error");
	/* Writing 4 lines crosses cf.cf_maxlines, so file should get
	 * compacted to 4 lines.
	 */
	zassert_equal(cf.cf_lines, 4U, "unexpected number of lines");

	val8 = 0U;
	val16 = 0U;
	val64 = 0U;
	EXPECT_NOERR(settings_load(), "fs readout error");
	zassert_equal(val8, 33U, "bad val8 read");
	zassert_equal(val16, 666U, "bad val16 read");
	zassert_equal(val64, 6400640000640064U, "bad val64 read");
}
