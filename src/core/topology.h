/*
 * StarPU
 * Copyright (C) Université Bordeaux 1, CNRS 2008-2010 (see AUTHORS file)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU Lesser General Public License in COPYING.LGPL for more details.
 */

#ifndef __TOPOLOGY_H__
#define __TOPOLOGY_H__

#include <starpu.h>
#include <common/config.h>
#include <common/list.h>
#include <common/fxt.h>

/* TODO actually move this struct into this header */
struct starpu_machine_config_s;

/* Detect the number of memory nodes and where to bind the different workers. */
int _starpu_build_topology(struct starpu_machine_config_s *config);

/* Destroy all resources used to store the topology of the machine. */
void _starpu_destroy_topology(struct starpu_machine_config_s *config);

/* returns the number of physical cpus */
unsigned _starpu_topology_get_nhwcpu(struct starpu_machine_config_s *config);

/* Bind the current thread on the CPU logically identified by "cpuid". The
 * logical ordering of the processors is either that of hwloc (if available),
 * or the ordering exposed by the OS. */
void _starpu_bind_thread_on_cpu(struct starpu_machine_config_s *config, unsigned cpuid);

#endif // __TOPOLOGY_H__
