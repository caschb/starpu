/* StarPU --- Runtime system for heterogeneous multicore architectures.
 *
 * Copyright (C) 2009, 2010  Université de Bordeaux 1
 * Copyright (C) 2010, 2011  Centre National de la Recherche Scientifique
 *
 * StarPU is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 *
 * StarPU is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU Lesser General Public License in COPYING.LGPL for more details.
 */

#ifndef __STARPU_MPI_H__
#define __STARPU_MPI_H__

#include <starpu.h>
#include <mpi.h>

typedef void *starpu_mpi_req;

int starpu_mpi_isend(starpu_data_handle data_handle, starpu_mpi_req *req, int dest, int mpi_tag, MPI_Comm comm);
int starpu_mpi_irecv(starpu_data_handle data_handle, starpu_mpi_req *req, int source, int mpi_tag, MPI_Comm comm);
int starpu_mpi_send(starpu_data_handle data_handle, int dest, int mpi_tag, MPI_Comm comm);
int starpu_mpi_recv(starpu_data_handle data_handle, int source, int mpi_tag, MPI_Comm comm, MPI_Status *status);
int starpu_mpi_isend_detached(starpu_data_handle data_handle, int dest, int mpi_tag, MPI_Comm comm, void (*callback)(void *), void *arg);
int starpu_mpi_irecv_detached(starpu_data_handle data_handle, int source, int mpi_tag, MPI_Comm comm, void (*callback)(void *), void *arg);
int starpu_mpi_wait(starpu_mpi_req *req, MPI_Status *status);
int starpu_mpi_test(starpu_mpi_req *req, int *flag, MPI_Status *status);
int starpu_mpi_barrier(MPI_Comm comm);
int starpu_mpi_initialize(void);
int starpu_mpi_initialize_extended(int *rank, int *world_size);
int starpu_mpi_shutdown(void);

int starpu_mpi_insert_task(MPI_Comm comm, starpu_codelet *codelet, ...);
void starpu_mpi_get_data_on_node(MPI_Comm comm, starpu_data_handle data_handle, int node);

/* Some helper functions */

/* When the transfer is completed, the tag is unlocked */
int starpu_mpi_isend_detached_unlock_tag(starpu_data_handle data_handle, int dest, int mpi_tag, MPI_Comm comm, starpu_tag_t tag);
int starpu_mpi_irecv_detached_unlock_tag(starpu_data_handle data_handle, int source, int mpi_tag, MPI_Comm comm, starpu_tag_t tag);

/* Asynchronously send an array of buffers, and unlocks the tag once all of
 * them are transmitted. */
int starpu_mpi_isend_array_detached_unlock_tag(unsigned array_size,
		starpu_data_handle *data_handle, int *dest, int *mpi_tag,
		MPI_Comm *comm, starpu_tag_t tag);
int starpu_mpi_irecv_array_detached_unlock_tag(unsigned array_size,
		starpu_data_handle *data_handle, int *source, int *mpi_tag,
		MPI_Comm *comm, starpu_tag_t tag);

#endif // __STARPU_MPI_H__
