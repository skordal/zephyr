/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <metal/device.h>
#include <zephyr.h>

#include "platform.h"
#include "resource_table.h"

extern struct remoteproc_ops platform_ops;

static metal_phys_addr_t shm_physmap[] = { SHM_START_ADDRESS };
static struct metal_device shm_device = {
	.name = SHM_DEVICE_NAME,
	.bus = NULL,
	.num_regions = 1,
	{
		{
			.virt       = (void *) SHM_START_ADDRESS,
			.physmap    = shm_physmap,
			.size       = SHM_SIZE,
			.page_shift = 0xffffffff,
			.page_mask  = 0xffffffff,
			.mem_flags  = 0,
			.ops        = { NULL },
		},
	},
	.node = { NULL },
	.irq_num = 0,
	.irq_info = NULL
};

struct remoteproc* platform_init(int role)
{
	int status;

	status = metal_register_generic_device(&shm_device);
	if (status != 0) {
		printk("metal_register_generic_device(): could not register shared memory device: error code %d\n", status);
		return NULL;
	}

	return remoteproc_init(&platform_ops, NULL);
}

