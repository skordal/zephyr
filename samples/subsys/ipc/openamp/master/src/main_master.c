/*
 * Copyright (c) 2018, NXP
 * Copyright (c) 2018, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <misc/printk.h>
#include <device.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openamp/open_amp.h>

#include "platform.h"
#include "resource_table.h"

#define APP_TASK_STACK_SIZE (512)
K_THREAD_STACK_DEFINE(thread_stack, APP_TASK_STACK_SIZE);
static struct k_thread thread_data;

static struct virtio_device vdev;
static struct rpmsg_virtio rpmsg_vdev;

void app_task(void *arg1, void *arg2, void *arg3)
{
	ARG_UNUSED(arg1);
	ARG_UNUSED(arg2);
	ARG_UNUSED(arg3);

	printk("\r\nOpenAMP demo started\r\n");

	struct metal_init_params metal_params = METAL_INIT_DEFAULTS;
	metal_init(&metal_params);

	rpmsg_vdev.role = RPMSG_MASTER;
	int status = rpmsg_vdev_init(&rpmsg_vdev, &vdev, (void *) SHM_START_ADDRESS, SHM_SIZE);
	if(status != RPMSG_SUCCESS)
	{
		printk("Error: rpmsg_vdev_init() failed with status %d\n", status);
		goto _cleanup;
	}

#if 0
	proc = platform_init(RPMSG_MASTER);
	if (proc == NULL) {
		printk("platform_init() failed\n");
		goto _cleanup;
	}

	resource_table_init((void **) &rsc_info.rsc_tab, &rsc_info.size);

	struct remote_proc *rproc_ptr = NULL;
	int status = remoteproc_resource_init(&rsc_info, proc,
					      rpmsg_channel_created, rpmsg_channel_deleted,
					      rpmsg_recv_callback, &rproc_ptr, RPMSG_MASTER);
	if (status != 0) {
		printk("remoteproc_resource_init() failed with status %d\n", status);
		goto _cleanup;
	}

	while (k_sem_take(&channel_created, K_NO_WAIT) != 0)
		hil_poll(proc, 0);

	unsigned int message = 0;
	status = send_message(message);
	if (status < 0) {
		printk("send_message(%d) failed with status %d\n", message, status);
		goto _cleanup;
	}

	while (message <= 100) {
		message = receive_message();
		printk("Primary core received a message: %d\n", message);

		message++;
		status = send_message(message);
		if (status < 0) {
			printk("send_message(%d) failed with status %d\n", message, status);
			goto _cleanup;
		}
	}

_cleanup:
	if (rproc_ptr) {
		remoteproc_resource_deinit(rproc_ptr);
	}
	metal_finish();
#endif
_cleanup:
	metal_finish();
	printk("OpenAMP demo ended.\n");
}

void main(void)
{
	printk("Starting application thread!\n");
	k_thread_create(&thread_data, thread_stack, APP_TASK_STACK_SIZE,
			(k_thread_entry_t)app_task,
			NULL, NULL, NULL, K_PRIO_COOP(7), 0, 0);
}
