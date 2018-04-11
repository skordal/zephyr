/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <metal/device.h>
#include <zephyr.h>
#include <ipm.h>

#include "platform.h"
#include "resource_table.h"

extern struct hil_platform_ops platform_ops;

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

static struct device *ipm_handle = NULL;

static void platform_ipm_callback(void *context, u32_t id, volatile void *data)
{
	printk("Virtqueue notified\n");
}

void _notify(struct virtqueue * vq)
{
	uint32_t dummy_data = 0x12345678; /* Some data must be provided */
	ipm_send(ipm_handle, 0, 0, &dummy_data, sizeof(dummy_data));
}

int platform_init(int role)
{
	int status;

	status = metal_register_generic_device(&shm_device);
	if (status != 0) {
		printk("metal_register_generic_device(): could not register shared memory device: error code %d\n", status);
		return status;
	}

	ipm_handle = device_get_binding("MAILBOX_0");
	if (!ipm_handle) {
		return -1;
	}

	ipm_register_callback(ipm_handle, platform_ipm_callback, NULL);
	return 0;
}

