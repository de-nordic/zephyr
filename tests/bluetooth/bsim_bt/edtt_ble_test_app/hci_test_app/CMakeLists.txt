# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2019 Oticon A/S

cmake_minimum_required(VERSION 3.13.1)

if (NOT DEFINED ENV{BSIM_COMPONENTS_PATH})
	message(FATAL_ERROR "This test requires the BabbleSim simulator. Please set\
 the environment variable BSIM_COMPONENTS_PATH to point to its components \
 folder. More information can be found in\
 https://babblesim.github.io/folder_structure_and_env.html")
endif()

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(hci_test_app)

target_sources(app PRIVATE
	src/main.c
	../common/edtt_driver_bsim.c
)

zephyr_library_include_directories(
  $ENV{ZEPHYR_BASE}/samples/bluetooth
  $ENV{ZEPHYR_BASE}/subsys/bluetooth
  ../common/
  $ENV{BSIM_COMPONENTS_PATH}/libUtilv1/src/
  $ENV{BSIM_COMPONENTS_PATH}/libPhyComv1/src/
)
