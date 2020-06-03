// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2018-2020 NXP
 *
 * Brief   CAAM Controller Hardware Abstration Layer.
 *         Implementation of primitives to access HW.
 */
#include <caam_hal_ctrl.h>
#include <caam_io.h>
#include <caam_trace.h>
#include <config.h>
#include <registers/ctrl_regs.h>
#include <registers/version_regs.h>
#include <kernel/panic.h>

uint8_t caam_hal_ctrl_era(vaddr_t baseaddr)
{
	/* Read the number of instance */
	uint32_t val = io_caam_read32(baseaddr + CCBVID);

	return GET_CCBVID_CAAM_ERA(val);
}

uint8_t caam_hal_ctrl_jrnum(vaddr_t baseaddr)
{
	uint32_t val = 0;
	uint8_t jrnum = 0;

	if (caam_hal_ctrl_era(baseaddr) < 10) {
		val = io_caam_read32(baseaddr + CHANUM_MS);
		jrnum = GET_CHANUM_MS_JRNUM(val);
	} else {
		val = io_caam_read32(baseaddr + JR_VERSION);
		jrnum = GET_JR_VERSION_JRNUM(val);
	}

	return jrnum;
}

uint8_t caam_hal_ctrl_hash_limit(vaddr_t baseaddr)
{
	uint32_t val = 0;

	if (caam_hal_ctrl_era(baseaddr) < 10) {
		/* Read the number of instance */
		val = io_caam_read32(baseaddr + CHANUM_LS);

		if (GET_CHANUM_LS_MDNUM(val)) {
			/* Hashing is supported */
			val = io_caam_read32(baseaddr + CHAVID_LS);
			val &= BM_CHAVID_LS_MDVID;
			if (val == CHAVID_LS_MDVID_LP256)
				return TEE_MAIN_ALGO_SHA256;

			return TEE_MAIN_ALGO_SHA512;
		}
	} else {
		/* Read the number of instance */
		val = io_caam_read32(baseaddr + MDHA_VERSION);

		if (GET_MDHA_VERSION_MDNUM(val)) {
			/* Hashing is supported */
			val &= BM_MDHA_VERSION_MDVID;
			if (val == MDHA_VERSION_MDVID_LP256)
				return TEE_MAIN_ALGO_SHA256;

			return TEE_MAIN_ALGO_SHA512;
		}
	}

	return UINT8_MAX;
}

bool caam_hal_ctrl_splitkey_support(vaddr_t baseaddr)
{
	uint32_t val = io_caam_read32(baseaddr + CAAMVID_MS);

	return GET_CAAMVID_MS_MAJ_REV(val) >= 3;
}

uint8_t caam_hal_ctrl_pknum(vaddr_t baseaddr)
{
	uint32_t val = 0;
	uint8_t pknum = 0;

	if (caam_hal_ctrl_era(baseaddr) < 10) {
		val = io_caam_read32(baseaddr + CHANUM_LS);
		pknum = GET_CHANUM_LS_PKNUM(val);
	} else {
		val = io_caam_read32(baseaddr + PKHA_VERSION);
		pknum = GET_PKHA_VERSION_PKNUM(val);
	}

	return pknum;
}
