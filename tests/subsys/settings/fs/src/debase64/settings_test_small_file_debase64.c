/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "settings/settings.h"
#include "settings_test.h"
#include "settings/settings_file.h"
#include "settings_priv_debase64.h"

void test_config_small_file_debase64(void)
{
	int rc;
	struct settings_file cf_mfg;

	config_wipe_srcs();

	cf_mfg.cf_name = TEST_CONFIG_DIR "/mfg";
	cf_mfg.cf_b64_marker = false;

	rc = settings_file_src(&cf_mfg);
	zassert_true(rc == 0, "can't register FS as configuration source");

	/* No data, marker only */
	val8 = -1;

	EXPECT_NOERR(fsutil_write_file(TEST_CONFIG_DIR "/mfg",
		STR_SIZE_TUPLE(CF_MARKER_ONLY)), "can't write to file");
	settings_load();
	zassert_false(test_set_called, "the SET handler WAS called");
	zassert_true(val8 == (unsigned char)-1,
		     "SET handler: was called with wrong parameters");

	/* Only one BASE64 encoded value, no marker */
	val8 = -1;

	EXPECT_NOERR(fsutil_write_file(TEST_CONFIG_DIR "/mfg",
		STR_SIZE_TUPLE(CF_NO_MARKER_B64)), "can't write to file");
	settings_load();
	zassert_true(test_set_called, "the SET handler WAS NOT called");
	zassert_true(val8 == 1U,
		     "SET handler: was called with wrong parameters");

	ctest_clear_call_state();

	/* Marker followed by one value */
	val8 = -1;

	EXPECT_NOERR(fsutil_write_file(TEST_CONFIG_DIR "/mfg",
		STR_SIZE_TUPLE(CF_WITH_MARKER)), "can't write to file");
	EXPECT_NOERR(settings_load(), "failed to load settings");
	zassert_true(test_set_called, "the SET handler WAS NOT called");
	zassert_true(val8 == 1U,
		     "SET handler: was called with wrong parameters");

	/* Marker followed by two different values for one vairable */
	ctest_clear_call_state();
	val8 = -1;

	EXPECT_NOERR(fsutil_write_file(TEST_CONFIG_DIR "/mfg",
		STR_SIZE_TUPLE(CF_WITH_MARKER_8)), "can't write to file");
	EXPECT_NOERR(settings_load(), "failed to load settings");
	zassert_true(test_set_called, "the SET handler WAS NOT called");
	zassert_true(val8 == 8U,
		     "SET handler: was called with wrong parameters");

	/* Base64 value followed by marker and non-base64 update to the
	   same variable */
	ctest_clear_call_state();
	val8 = -1;

	EXPECT_NOERR(fsutil_write_file(TEST_CONFIG_DIR "/mfg",
		STR_SIZE_TUPLE(CF_VAL_MARKER_VAL)), "can't write to file");
	EXPECT_NOERR(settings_load(), "failed to load settings");
	zassert_true(test_set_called, "the SET handler WAS NOT called");
	zassert_true(val8 == 8U,
		     "SET handler: was called with wrong parameters");

	ctest_clear_call_state();
}
