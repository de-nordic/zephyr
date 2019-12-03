# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2020 Nordic Semiconductor ASA

cmake_minimum_required(VERSION 3.13.1)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(nffs)

add_subdirectory(../src nffs_test_bindir)
target_link_libraries(app PRIVATE settings_nffs_test)

# The code is in the library common to several tests.
target_sources(app PRIVATE main.c)

if(TEST)
	target_compile_definitions(app PRIVATE
		-DTEST_${TEST}
		)
endif()
