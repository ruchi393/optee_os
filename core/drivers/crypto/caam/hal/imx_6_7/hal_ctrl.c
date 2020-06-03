// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2018-2019 NXP
 *
 * Brief   CAAM Controller Hardware Abstration Layer.
 *         Implementation of primitives to access HW
 */
#include <caam_hal_ctrl.h>
#include <caam_io.h>
#include <caam_pwr.h>
#include <registers/ctrl_regs.h>

/*
 * List of control registers to save/restore
 */
static const struct reglist ctrl_backup[] = {
	BACKUP_REG(MCFGR, 1, 0, 0),
#ifdef CFG_NXP_CAAM_MP_DRV
	BACKUP_REG(SCFGR, 1, BM_SCFGR_MPMRL | BM_SCFGR_MPCURVE, 0),
#else
	/* For device not supporting MP (bits not defined) */
	BACKUP_REG(SCFGR, 1, 0, 0),
#endif
};

void caam_hal_ctrl_init(vaddr_t baseaddr)
{
	/* Enable DECO watchdogs */
	io_setbits32(baseaddr + MCFGR, MCFGR_WDE);

	/*
	 * ERRATA: mx6 devices have an issue wherein AXI bus transactions
	 * may not occur in the correct order. This isn't a problem running
	 * single descriptors, but can be if running multiple concurrent
	 * descriptors. Reworking the driver to throttle to single requests
	 * is impractical, thus the workaround is to limit the AXI pipeline
	 * to a depth of 1 (default depth is 4) to preclude this situation
	 * from occurring.
	 *
	 * mx7 devices, this bit has no effect.
	 */
	io_mask32(baseaddr + MCFGR, MCFGR_AXIPIPE(1), BM_MCFGR_AXIPIPE);

	caam_pwr_add_backup(baseaddr, ctrl_backup, ARRAY_SIZE(ctrl_backup));
}

#define PRIBLOB_MASK	GENMASK_32(1, 0)

void caam_hal_ctrl_inc_priblob(vaddr_t baseaddr)
{
	uint32_t val = 0;
	uint32_t blob = 0;

	if (!IS_ENABLED(CFG_CAAM_INC_PRIBLOB))
		return;

	val = io_caam_read32(baseaddr + SCFGR);
	val &= PRIBLOB_MASK;
	CTRL_TRACE("Reading CAAM PRIBLOB: 0x%"PRIx32, val);

	if (val == 0 || val == 2)
		blob = val + 1;
	else if (val == 1)
		blob = val + 2;
	else
		panic("Error locking PRIBLOB, PRIBLOB =3");

	CTRL_TRACE("New CAAM PRIBLOB value: 0x%"PRIx32, blob);

	val = io_caam_read32(baseaddr + SCFGR);
	val |= blob;
	io_caam_write32(baseaddr + SCFGR, val);

	val = io_caam_read32(baseaddr + SCFGR);
	val &= PRIBLOB_MASK;
	CTRL_TRACE("Checking: CAAM PRIBLOB: 0x%"PRIx32 " want: 0x%"PRIx32, val,
		   blob);
	if (val != blob)
		panic("Written PRIBLOB and read PRIBLOB do not match!");
}
