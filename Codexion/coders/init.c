/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brportos <brportos@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 19:18:39 by brportos          #+#    #+#             */
/*   Updated: 2026/09/07 19:28:19 by brportos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	init_mutex_cond(t_data *data);

void	init_pthread(t_data *data)
{
	int	i;

	i = 0;
	pthread_create(&data->monitoring_id, NULL, &monitoring_simulation, data);
	while (i != data->ncoder)
	{
		pthread_create(&data->coder[i].thread, NULL, &coder_start_routine,
			&data->coder[i]);
		i++;
	}
}

void	init_mutex(t_data *data)
{
	pthread_mutex_init(&data->mutex_print, NULL);
	pthread_mutex_init(&data->mutex_simul, NULL);
	pthread_mutex_init(&data->queue_ctrl.lock, NULL);
	pthread_mutex_init(&data->heap_ctrl.lock, NULL);
	init_dongles_mutex(data);
	init_mutex_cond(data);
}

int	init_struct(t_data *data)
{
	int	i;

	data->active_simulation = 1;
	data->start_time = get_time_ms();
	data->queue_ctrl.first = NULL;
	data->queue_ctrl.last = NULL;
	data->heap_ctrl.size = 0;
	i = 0;
	while (i < MAX_CODERS)
	{
		data->heap_ctrl.tree[i] = NULL;
		i++;
	}
	create_coders_and_dongles(data);
	return (0);
}

void	init_basic_data(t_data *data, t_coder *coder, int count)
{
	coder->id = count + 1;
	coder->time_burnout = get_time_ms();
	coder->coder_compiled = 0;
	coder->have_done = 0;
	coder->data = data;
	coder->ldongle = NULL;
	coder->rdongle = NULL;
}

static void	init_mutex_cond(t_data *data)
{
	pthread_cond_init(&data->queue_ctrl.cond, NULL);
	pthread_cond_init(&data->heap_ctrl.cond, NULL);
}
