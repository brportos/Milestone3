/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brportos <brportos@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 19:18:52 by brportos          #+#    #+#             */
/*   Updated: 2026/09/21 12:55:02 by brportos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	fifo_add_queue(t_data *data, t_coder *coder);

int	isfifo(t_data *data)
{
	if (strcmp(FIFO, data->scheduler) == 0)
		return (1);
	return (0);
}

int	scheduler_fifo(t_data *data, t_coder *coder, char *action)
{
	pthread_mutex_lock(&data->queue_ctrl.lock);
	if (strcmp(action, "add_queue") == 0)
		fifo_add_queue(data, coder);
	else if (strcmp(action, "remove_queue") == 0)
	{
		remove_from_queue(&data->queue_ctrl);
		pthread_cond_broadcast(&data->queue_ctrl.cond);
	}
	pthread_mutex_unlock(&data->queue_ctrl.lock);
	return (0);
}

void	scheduler_edf_add(t_data *data, t_coder *coder)
{
	t_heap	*heap;

	heap = &data->heap_ctrl;
	pthread_mutex_lock(&heap->lock);
	heap_push(heap, coder);
	pthread_cond_broadcast(&heap->cond);
	pthread_mutex_unlock(&heap->lock);
	while (get_simulation(data) == 1)
	{
		if (ispriority(data, coder))
		{
			if (take_dongle(coder) == 0)
				break ;
		}
		usleep(00);
	}
	pthread_mutex_lock(&heap->lock);
	heap_pop(heap, coder);
	pthread_cond_broadcast(&heap->cond);
	pthread_mutex_unlock(&heap->lock);
}

static void	fifo_add_queue(t_data *data, t_coder *coder)
{
	add_to_queue(&data->queue_ctrl, coder);
	while ((get_simulation(data) == 1)
		&& (data->queue_ctrl.first->coder != coder || take_dongle(coder) == 1))
	{
		if (get_simulation(data) == 1 && data->queue_ctrl.first->coder == coder)
		{
			pthread_mutex_unlock(&data->queue_ctrl.lock);
			usleep(300);
			pthread_mutex_lock(&data->queue_ctrl.lock);
		}
		else
			pthread_cond_wait(&data->queue_ctrl.cond, &data->queue_ctrl.lock);
	}
}
