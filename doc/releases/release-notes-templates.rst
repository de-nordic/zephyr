:orphan:

.. Start of Instruction
.. ####################
..
.. NOTE: Remove all commented text, with .., between and including ".. Start of Instruction
.. and ".. End of Instruction"
..
.. Starting
.. ********
.. Copy the template to new release file and replace `template` in name of the file with
.. version in format <major>.<minor>. For example to create template for release 3.3 do
..
..   cp release-notes-template.rst release-notes-3.3.rst
.. 
.. In section of newly created release file change ocurences of <zvmajor>.<zvminor> to
.. current version, for example to 3.3.
..
.. End of Instruction

.. _zephyr_<zvmajor>.<zvminor>:

Zephyr <zvmajor>.<zvminor>.0
############

The following sections provide detailed lists of changes by component.

API Changes
***********

Changes in this release
=======================

* CAN

Removed APIs in this release
============================

Deprecated in this release
==========================

Stable API changes in this release
==================================

New APIs in this release
========================

* CAN

* MIPI-DSI

* SDHC API

* Util


Bluetooth
*********

* Audio

* Direction Finding

* Mesh

* Controller

* HCI Driver

* Host

Kernel
******

Architectures
*************

* ARC

* ARM

  * AARCH32

  * AARCH64

* Xtensa

* x64_64

Boards & SoC Support
********************

* Added support for these SoC series:

* Made these changes in other SoC series:

* Changes for ARC boards:

* Added support for these ARM boards:

* Added support for these ARM64 boards:

* Added support for these RISC-V boards:

* Made these changes in other boards:

* Added support for these following shields:

Drivers and Sensors
*******************

* ADC

* CAN

* Clock_control

* Counter

* DAC

* Disk

* Display

* DMA

* Entropy

* Ethernet

* Flash

* GPIO

* HWINFO

* I2C

* I2S

* MEMC

* Pin control

  * Platform support was added for:

* PWM

* Reset

* Sensor

* Serial

* SPI

* Timer

* USB

Networking
**********

* CoAP:

* Ethernet:

* HTTP:

* LwM2M:

* Misc:

* MQTT:

* OpenThread:

* Sockets:

* TCP:

* TLS:

USB
***

Build System
************

Devicetree
**********

* API

* Bindings

Libraries / Subsystems
**********************

* C Library

  * Minimal libc

  * Newlib

* C++ Subsystem

* Management

  * MCUmgr

  * Hawkbit

* Storage

  * SD Subsystem

* Power management

* IPC

* Logging

* Shell

HALs
****

* Atmel

* GigaDevice

* Intel

* Nordic Semiconductor

* STM32:

MCUboot
*******

Trusted Firmware-m
******************

Documentation
*************

Tests and Samples
*****************

.. This chapter is dedicated to changes done to test an

Issue summary
*************

Security Vulnerability Related
==============================

The following CVEs are addressed by this release:

 * CVE-

More detailed information can be found in:
https://docs.zephyrproject.org/latest/security/vulnerabilities.html

Known bugs
==========

* :github:`<issue_num>` - <Title>
* ...

Addressed issues
================

* :github:`<issue_num>` - <Title>
* ...
