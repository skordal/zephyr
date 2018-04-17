/*
 * Copyright (c) 2018, NXP
 * Copyright (c) 2018 Nordic Semiconductor ASA
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

void app_task(void *arg1, void *arg2, void *arg3)
{
	ARG_UNUSED(arg1);
	ARG_UNUSED(arg2);
	ARG_UNUSED(arg3);

	struct metal_init_params metal_params = METAL_INIT_DEFAULTS;
	metal_init(&metal_params);

	struct remoteproc *remote = platform_init(RPMSG_REMOTE);
	if(remote == NULL)
	{
		printk("Error: platform_init() failed\n");
		goto _cleanup;
	}

	struct rpmsg_virtio_device rvdev;

	struct virtio_device vdev;
	vdev.func = &remoteproc_virtio_dispatch_funcs;
	vdev.role = RPMSG_REMOTE;

	int status = rpmsg_init_vdev(&rvdev, &vdev, (void *) SHM_START_ADDRESS, SHM_SIZE);
#if 0
	memset(&rpmsg_vdev, 0, sizeof(struct rpmsg_virtio));
	rpmsg_vdev.role = RPMSG_REMOTE;
	int status = rpmsg_init_vdev(&rpmsg_vdev, &vdev, NULL, 0);
	if(status != RPMSG_SUCCESS)
	{
		printk("Error: rpmsg_vdev_init() failed with status %d\n", status);
		goto _cleanup;
	}

	proc = platform_init(RPMSG_REMOTE);
	if (proc == NULL) {
		goto _cleanup;
	}

	resource_table_init((void **) &rsc_info.rsc_tab, &rsc_info.size);

	struct remote_proc *rproc_ptr = NULL;
	int status = remoteproc_resource_init(&rsc_info, proc,
					      rpmsg_channel_created, rpmsg_channel_deleted,
					      rpmsg_recv_callback, &rproc_ptr, RPMSG_REMOTE);
	if (status != 0) {
		goto _cleanup;
	}

	while (k_sem_take(&channel_created, K_NO_WAIT) != 0)
		hil_poll(proc, 0);

	unsigned int message = 0;
	while (message <= 100) {
		message = receive_message();
		message++;
		status = send_message(message);
		if (status <= 0) {
			goto _cleanup;
		}
	}

	if (rproc_ptr) {
		remoteproc_resource_deinit(rproc_ptr);
	}
#endif
_cleanup:
	metal_finish();
}

void main(void)
{
	k_thread_create(&thread_data, thread_stack, APP_TASK_STACK_SIZE,
			(k_thread_entry_t)app_task,
			NULL, NULL, NULL, K_PRIO_COOP(7), 0, 0);
}
