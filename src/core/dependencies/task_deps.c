/* StarPU --- Runtime system for heterogeneous multicore architectures.
 *
 * Copyright (C) 2010-2011  Université de Bordeaux 1
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

#include <starpu.h>
#include <common/config.h>
#include <common/utils.h>
#include <core/dependencies/tags.h>
#include <core/dependencies/htable.h>
#include <core/jobs.h>
#include <core/task.h>
#include <core/sched_policy.h>
#include <core/dependencies/data_concurrency.h>
#include <profiling/bound.h>

static struct _starpu_cg *create_cg_task(unsigned ntags, struct _starpu_job *j)
{
	struct _starpu_cg *cg = (struct _starpu_cg *) malloc(sizeof(struct _starpu_cg));
	STARPU_ASSERT(cg);

	cg->ntags = ntags;
	cg->remaining = ntags;
	cg->cg_type = STARPU_CG_TASK;

	cg->succ.job = j;
	j->job_successors.ndeps++;

	return cg;
}

/* the job lock must be taken */
static void _starpu_task_add_succ(struct _starpu_job *j, struct _starpu_cg *cg)
{
	STARPU_ASSERT(j);

	_starpu_add_successor_to_cg_list(&j->job_successors, cg);

	if (j->terminated)
	{
		/* the task was already completed sooner */
		_starpu_notify_cg(cg);
	}
}

void _starpu_notify_task_dependencies(struct _starpu_job *j)
{
	_starpu_notify_cg_list(&j->job_successors);
}

/* task depends on the tasks in task array */
void starpu_task_declare_deps_array(struct starpu_task *task, unsigned ndeps, struct starpu_task *task_array[])
{
	if (ndeps == 0)
		return;

	struct _starpu_job *job;

	job = _starpu_get_job_associated_to_task(task);

	_STARPU_PTHREAD_MUTEX_LOCK(&job->sync_mutex);

	struct _starpu_cg *cg = create_cg_task(ndeps, job);

	unsigned i;
	for (i = 0; i < ndeps; i++)
	{
		struct starpu_task *dep_task = task_array[i];

		struct _starpu_job *dep_job;
		dep_job = _starpu_get_job_associated_to_task(dep_task);
		STARPU_ASSERT(dep_job != job && "A task must not depend on itself.");

		_STARPU_TRACE_TASK_DEPS(dep_job, job);
		_starpu_bound_task_dep(job, dep_job);

		_STARPU_PTHREAD_MUTEX_LOCK(&dep_job->sync_mutex);
		_starpu_task_add_succ(dep_job, cg);
		_STARPU_PTHREAD_MUTEX_UNLOCK(&dep_job->sync_mutex);
	}

	_STARPU_PTHREAD_MUTEX_UNLOCK(&job->sync_mutex);
}
