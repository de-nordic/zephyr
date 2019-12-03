/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define CF_B64_MYFOO_MYBAR_1	"\x10\x00myfoo/mybar=AQ=="
#define CF_B64_MYFOO_MYBAR_8	"\x10\x00myfoo/mybar=CA=="
#define CF_B64_MYFOO_MYBAR_14	"\x10\x00myfoo/mybar=Dg=="
#define CF_B64_MYFOO_MYBAR_15	"\x10\x00myfoo/mybar=DW=="
#define CF_MYFOO_MYBAR_1	"\x0D\x00myfoo/mybar=\x01"
#define CF_MYFOO_MYBAR_5	"\x0D\x00myfoo/mybar=\x05"
#define CF_MYFOO_MYBAR_8	"\x0D\x00myfoo/mybar=\x08"
#define CF_MYFOO_MYBAR_14	"\x0D\x00myfoo/mybar=\x0e"
#define CF_MYFOO_MYBAR_15	"\x0D\x00myfoo/mybar=\x0f"

#define CF_NOB64_MARKER		"\x0C\x00_sys/nob64=\x01"

#define CF_MARKER_ONLY		CF_NOB64_MARKER

#define CF_NO_MARKER_B64	CF_B64_MYFOO_MYBAR_1


#define CF_WITH_MARKER_BAD	CF_NOB64_MARKER\
				CF_B64_MYFOO_MYBAR_1

#define CF_WITH_MARKER		CF_NOB64_MARKER\
				CF_MYFOO_MYBAR_1

#define CF_WITH_MARKER_8	CF_NOB64_MARKER\
				CF_MYFOO_MYBAR_1\
				CF_MYFOO_MYBAR_8

#define CF_VAL_MARKER_VAL	CF_B64_MYFOO_MYBAR_1\
				CF_NOB64_MARKER\
				CF_MYFOO_MYBAR_8

#define STR_SIZE_TUPLE(name) name, (sizeof(name) - 1)
#define EXPECT_NOERR(exp, msg) (zassert_equal(0, (exp), msg))

