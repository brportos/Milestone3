/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simul.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brportos <brportos@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 19:18:56 by brportos          #+#    #+#             */
/*   Updated: 2026/09/08 08:33:38 by brportos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	stop_simulation(t_data *data)
{
	pthread_mutex_lock(&data->mutex_simul);
	data->active_simulation = 0;
	pthread_mutex_unlock(&data->mutex_simul);
	pthread_mutex_lock(&data->queue_ctrl.lock);
	pthread_cond_broadcast(&data->queue_ctrl.cond);
	pthread_mutex_unlock(&data->queue_ctrl.lock);
	pthread_mutex_lock(&data->heap_ctrl.lock);
	pthread_cond_broadcast(&data->heap_ctrl.cond);
	pthread_mutex_unlock(&data->heap_ctrl.lock);
}

int	get_simulation(t_data *data)
{
	int	simul;

	simul = 0;
	pthread_mutex_lock(&data->mutex_simul);
	simul = data->active_simulation;
	pthread_mutex_unlock(&data->mutex_simul);
	return (simul);
}
