/*
 * StarPU
 * Copyright (C) INRIA 2008-2009 (see AUTHORS file)
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

#ifndef __DATA_INTERFACE_H__
#define __DATA_INTERFACE_H__

#include <starpu.h>
#include <common/config.h>

#ifdef STARPU_USE_GORDON
/* to get the gordon_strideSize_t data structure from gordon */
#include <gordon.h>
#endif

struct starpu_data_interface_ops_t {
	void (*register_data_handle)(starpu_data_handle handle,
					uint32_t home_node, void *interface);
	size_t (*allocate_data_on_node)(starpu_data_handle handle, uint32_t node);
	void (*liberate_data_on_node)(void *interface, uint32_t node);
	const struct starpu_copy_data_methods_s *copy_methods;
	size_t (*get_size)(starpu_data_handle handle);
	uint32_t (*footprint)(starpu_data_handle handle);
	void (*display)(starpu_data_handle handle, FILE *f);
#ifdef STARPU_USE_GORDON
	int (*convert_to_gordon)(void *interface, uint64_t *ptr, gordon_strideSize_t *ss); 
#endif
	/* an identifier that is unique to each interface */
	unsigned interfaceid;
	size_t interface_size;
};

void starpu_register_data_handle(starpu_data_handle *handleptr, uint32_t home_node,
				void *interface,
				struct starpu_data_interface_ops_t *ops);

#endif // __DATA_INTERFACE_H__
