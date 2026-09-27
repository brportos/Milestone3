/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brportos <brportos@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 19:18:20 by brportos          #+#    #+#             */
/*   Updated: 2026/09/18 08:43:38 by brportos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	try_take_dongle(t_dongle *dongle, t_data *data)
{
	pthread_mutex_lock(&dongle->lock);
	if (get_simul_time(data) >= dongle->cooldown)
		return (0);
	pthread_mutex_unlock(&dongle->lock);
	return (1);
}

int	take_dongle(t_coder *coder)
{
	if (try_take_dongle(coder->ldongle, coder->data) == 0)
	{
		if (coder->rdongle == NULL)
		{
			pthread_mutex_unlock(&coder->ldongle->lock);
			return (1);
		}
		if (try_take_dongle(coder->rdongle, coder->data) == 0)
		{
			display_log(coder->id, "takedongle", coder->data);
			display_log(coder->id, "takedongle", coder->data);
			return (0);
		}
		else
		{
			pthread_mutex_unlock(&coder->ldongle->lock);
			return (1);
		}
	}
	return (1);
}

void	release_dongles(t_coder *coder, t_data *data)
{
	long	curr_time;

	curr_time = get_simul_time(data);
	coder->ldongle->cooldown = curr_time + data->dongle_cooldown;
	if (coder->rdongle != NULL)
		coder->rdongle->cooldown = curr_time + data->dongle_cooldown;
	pthread_mutex_unlock(&coder->ldongle->lock);
	if (coder->rdongle != NULL)
		pthread_mutex_unlock(&coder->rdongle->lock);
	if (isfifo(data))
	{
		pthread_mutex_lock(&data->queue_ctrl.lock);
		pthread_cond_broadcast(&data->queue_ctrl.cond);
		pthread_mutex_unlock(&data->queue_ctrl.lock);
	}
	else
	{
		pthread_mutex_lock(&data->heap_ctrl.lock);
		pthread_cond_broadcast(&data->heap_ctrl.cond);
		pthread_mutex_unlock(&data->heap_ctrl.lock);
	}
}

void	init_dongles_mutex(t_data *data)
{
	int	i;

	i = 0;
	while (i != data->ncoder)
	{
		pthread_mutex_init(&data->dongle[i].lock, NULL);
		pthread_mutex_init(&data->coder[i].mutex_burnout, NULL);
		pthread_mutex_init(&data->coder[i].mutex_done, NULL);
		i++;
	}
}
