/* StarPU --- Runtime system for heterogeneous multicore architectures.
 *
 * Copyright (C) 2013-2017  Université de Bordeaux
 * Copyright (C) 2013, 2017  Inria
 * Copyright (C) 2013  Simon Archipoff
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

#include <starpu_sched_component.h>
#include "sched_component.h"
#include <starpu_perfmodel.h>
#include "helper_mct.h"
#include <float.h>
#include <core/sched_policy.h>
#include <core/task.h>

static int mct_push_task(struct starpu_sched_component * component, struct starpu_task * task)
{
	STARPU_ASSERT(component && task && starpu_sched_component_is_mct(component));
	struct _starpu_mct_data * d = component->data;
	struct starpu_sched_component * best_component = NULL;

	/* Estimated task duration for each child */
	double estimated_lengths[component->nchildren];
	/* Estimated transfer duration for each child */
	double estimated_transfer_length[component->nchildren];
	/* Estimated transfer+task termination for each child */
	double estimated_ends_with_task[component->nchildren];

	unsigned i;
	for(i=0; i < component->nchildren; i++)
	{
		estimated_lengths[i] = 0.0;
		estimated_transfer_length[i] = 0.0;
		estimated_ends_with_task[i] = 0.0;

	}
	/* Minimum transfer+task termination on all children */
	double min_exp_end_with_task = DBL_MAX;
	/* Maximum transfer+task termination on all children */
	double max_exp_end_with_task = 0.0;

	unsigned suitable_components[component->nchildren];
	unsigned nsuitable_components = 0;

	nsuitable_components = starpu_mct_compute_execution_times(component, task,
								  estimated_lengths, estimated_transfer_length, suitable_components);

	/* If no suitable components were found, it means that the perfmodel of
	 * the task had been purged since it has been pushed on the mct component.
	 * We should send a push_fail message to its parent so that it will
	 * be able to reschedule the task properly. */
	if(nsuitable_components == 0)
		return 1;


	/* Entering critical section to make sure no two workers
	   make scheduling decisions at the same time */
	STARPU_COMPONENT_MUTEX_LOCK(&d->scheduling_mutex);


	starpu_mct_compute_expected_times(component, task, estimated_lengths, estimated_transfer_length,
					  estimated_ends_with_task, &min_exp_end_with_task, &max_exp_end_with_task, suitable_components, nsuitable_components);

	double best_fitness = DBL_MAX;
	int best_icomponent = -1;
	for(i = 0; i < nsuitable_components; i++)
	{
		int icomponent = suitable_components[i];
#ifdef STARPU_DEVEL
#warning FIXME: take energy consumption into account
#endif
		double tmp = starpu_mct_compute_fitness(d,
					     estimated_ends_with_task[icomponent],
					     min_exp_end_with_task,
					     max_exp_end_with_task,
					     estimated_transfer_length[icomponent],
					     0.0);

		if(tmp < best_fitness)
		{
			best_fitness = tmp;
			best_icomponent = icomponent;
		}
	}

	/* If no best component is found, it means that the perfmodel of
	 * the task had been purged since it has been pushed on the mct component.
	 * We should send a push_fail message to its parent so that it will
	 * be able to reschedule the task properly. */
	if(best_icomponent == -1)
	{
		STARPU_COMPONENT_MUTEX_UNLOCK(&d->scheduling_mutex);
		return 1;
	}

	best_component = component->children[best_icomponent];

	task->predicted = estimated_lengths[best_icomponent];
	task->predicted_transfer = estimated_transfer_length[best_icomponent];

	if(starpu_sched_component_is_worker(best_component))
	{
		best_component->can_pull(best_component);
		STARPU_COMPONENT_MUTEX_UNLOCK(&d->scheduling_mutex);

		return 1;
	}

	_STARPU_TASK_BREAK_ON(task, sched);
	int ret = starpu_sched_component_push_task(component, best_component, task);

	/* I can now exit the critical section: Pushing the task below ensures that its execution
	   time will be taken into account for subsequent scheduling decisions */
	STARPU_COMPONENT_MUTEX_UNLOCK(&d->scheduling_mutex);

	return ret;
}

static void mct_component_deinit_data(struct starpu_sched_component * component)
{
	STARPU_ASSERT(starpu_sched_component_is_mct(component));
	struct _starpu_mct_data * d = component->data;
	STARPU_PTHREAD_MUTEX_DESTROY(&d->scheduling_mutex);
	free(d);
}

int starpu_sched_component_is_mct(struct starpu_sched_component * component)
{
	return component->push_task == mct_push_task;
}

struct starpu_sched_component * starpu_sched_component_mct_create(struct starpu_sched_tree *tree, struct starpu_sched_component_mct_data * params)
{
	struct starpu_sched_component * component = starpu_sched_component_create(tree, "mct");
	struct _starpu_mct_data *data = starpu_mct_init_parameters(params);

	component->data = data;
	STARPU_PTHREAD_MUTEX_INIT(&data->scheduling_mutex, NULL);

	component->push_task = mct_push_task;
	component->deinit_data = mct_component_deinit_data;

	return component;
}
