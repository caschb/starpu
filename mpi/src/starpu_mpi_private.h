/* StarPU --- Runtime system for heterogeneous multicore architectures.
 *
 * Copyright (C) 2010, 2012  Université de Bordeaux 1
 * Copyright (C) 2010, 2011, 2012, 2013  Centre National de la Recherche Scientifique
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

#ifndef __STARPU_MPI_PRIVATE_H__
#define __STARPU_MPI_PRIVATE_H__

#include <starpu.h>
#include <common/config.h>
#include "starpu_mpi.h"
#include "starpu_mpi_fxt.h"
#include <common/list.h>
#include <common/utils.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define STARPU_MPI_VERBOSE	1

#ifdef STARPU_MPI_VERBOSE
static int _debug_rank=-1;
#endif

#ifdef STARPU_MPI_VERBOSE
#  define _STARPU_MPI_DEBUG(fmt, args ...) do { if (!getenv("STARPU_SILENT")) { \
	                                        if (_debug_rank == -1) MPI_Comm_rank(MPI_COMM_WORLD, &_debug_rank); \
                                                fprintf(stderr, "%*s[%d][starpu_mpi][%s] " fmt , (_debug_rank+1)*4, "", _debug_rank, __func__ ,##args); \
                                                fflush(stderr); }} while(0);
#else
#  define _STARPU_MPI_DEBUG(fmt, args ...)
#endif

#define _STARPU_MPI_DISP(fmt, args ...) do { if (!getenv("STARPU_SILENT")) { \
	       				     if (_debug_rank == -1) MPI_Comm_rank(MPI_COMM_WORLD, &_debug_rank); \
                                             fprintf(stderr, "%*s[%d][starpu_mpi][%s] " fmt , (_debug_rank+1)*4, "", _debug_rank, __func__ ,##args); \
                                             fflush(stderr); }} while(0);

#ifdef STARPU_MPI_VERBOSE0
#  define _STARPU_MPI_LOG_IN()             do { if (!getenv("STARPU_SILENT")) { \
                                               if (_debug_rank == -1) MPI_Comm_rank(MPI_COMM_WORLD, &_debug_rank);                        \
                                               fprintf(stderr, "%*s[%d][starpu_mpi][%s] -->\n", (_debug_rank+1)*4, "", _debug_rank, __func__ ); \
                                               fflush(stderr); }} while(0)
#  define _STARPU_MPI_LOG_OUT()            do { if (!getenv("STARPU_SILENT")) { \
                                               if (_debug_rank == -1) MPI_Comm_rank(MPI_COMM_WORLD, &_debug_rank);                        \
                                               fprintf(stderr, "%*s[%d][starpu_mpi][%s] <--\n", (_debug_rank+1)*4, "", _debug_rank, __func__ ); \
                                               fflush(stderr); }} while(0)
#else
#  define _STARPU_MPI_LOG_IN()
#  define _STARPU_MPI_LOG_OUT()
#endif

enum _starpu_mpi_request_type
{
	SEND_REQ=0,
	RECV_REQ=1,
	WAIT_REQ=2,
	TEST_REQ=3,
	BARRIER_REQ=4,
	PROBE_REQ=5
};

LIST_TYPE(_starpu_mpi_req,
	/* description of the data at StarPU level */
	starpu_data_handle_t data_handle;

	/* description of the data to be sent/received */
	MPI_Datatype datatype;
	void *ptr;
	ssize_t count;
	int user_datatype;

	/* who are we talking to ? */
	int srcdst;
	int mpi_tag;
	MPI_Comm comm;

	void (*func)(struct _starpu_mpi_req *);

	MPI_Status *status;
	MPI_Request request;
	int *flag;

	int ret;
	_starpu_pthread_mutex_t req_mutex;
	_starpu_pthread_cond_t req_cond;

	enum _starpu_mpi_request_type request_type; /* 0 send, 1 recv */

	unsigned submitted;
	unsigned completed;

	/* In the case of a Wait/Test request, we are going to post a request
	 * to test the completion of another request */
	struct _starpu_mpi_req *other_request;

	/* in the case of detached requests */
	unsigned detached;
	void *callback_arg;
	void (*callback)(void *);

        /* in the case of user-defined datatypes, we need to send the size of the data */
	MPI_Request size_req;
);

#ifdef __cplusplus
}
#endif

#endif // __STARPU_MPI_PRIVATE_H__
