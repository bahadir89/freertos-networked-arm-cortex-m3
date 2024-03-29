#******************************************************************************
#
# Makefile - Rules for building the Ethernet boot_demo example.
#
# Copyright (c) 2009-2010 Texas Instruments Incorporated.  All rights reserved.
# Software License Agreement
# 
# Texas Instruments (TI) is supplying this software for use solely and
# exclusively on TI's microcontroller products. The software is owned by
# TI and/or its suppliers, and is protected under applicable copyright
# laws. You may not combine this software with "viral" open-source
# software in order to form a larger program.
# 
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
# CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
# 
# This is part of revision 6075 of the EK-LM3S8962 Firmware Package.
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=LM3S8962

#
# The base directory for StellarisWare.
#
ROOT=../../..

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find source files that do not live in this directory.
#
VPATH=../drivers
VPATH+=../../../utils

#
# Where to find header files that do not live in the source directory.
#
IPATH=.
IPATH+=..
IPATH+=../../..
IPATH+=../../../third_party/lwip-1.3.2/apps
IPATH+=../../../third_party/bget
IPATH+=../../../third_party/lwip-1.3.2/ports/stellaris/include
IPATH+=../../../third_party/lwip-1.3.2/src/include
IPATH+=../../../third_party/lwip-1.3.2/src/include/ipv4
IPATH+=../../../third_party

#
# The default rule, which causes the Ethernet boot_demo example to be built.
#
all: ${COMPILER}
all: ${COMPILER}/boot_demo_eth.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir ${COMPILER}

#
# Rules for building the Ethernet boot_demo example.
#
${COMPILER}/boot_demo_eth.axf: ${COMPILER}/boot_demo_eth.o
${COMPILER}/boot_demo_eth.axf: ${COMPILER}/lwiplib.o
${COMPILER}/boot_demo_eth.axf: ${COMPILER}/rit128x96x4.o
${COMPILER}/boot_demo_eth.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/boot_demo_eth.axf: ${COMPILER}/swupdate.o
${COMPILER}/boot_demo_eth.axf: ${COMPILER}/ustdlib.o
${COMPILER}/boot_demo_eth.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
${COMPILER}/boot_demo_eth.axf: boot_demo_eth.ld
SCATTERgcc_boot_demo_eth=boot_demo_eth.ld
ENTRY_boot_demo_eth=ResetISR

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
