/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   frees.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brportos <brportos@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 19:18:26 by brportos          #+#    #+#             */
/*   Updated: 2026/09/07 19:24:55 by brportos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	free_memory(t_data *data)
{
	t_queue	*curr;
	t_queue	*next;

	curr = data->queue_ctrl.first;
	if (data->coder != NULL)
		free(data->coder);
	if (data->dongle != NULL)
		free(data->dongle);
	while (curr != NULL)
	{
		next = curr->next;
		free(curr);
		curr = next;
	}
}

int	remove_from_queue(t_queue_manager *manager)
{
	t_queue	*tmp;

	if (manager->first == NULL)
		return (1);
	tmp = manager->first;
	manager->first = tmp->next;
	if (manager->first == NULL)
		manager->last = NULL;
	free(tmp);
	return (0);
}

static void	destroy_mutex_cond(t_data *data)
{
	pthread_cond_destroy(&data->queue_ctrl.cond);
	pthread_cond_destroy(&data->heap_ctrl.cond);
}

void	destroy_mutex(t_data *data)
{
	int	i;

	i = 0;
	while (i != data->ncoder)
	{
		pthread_mutex_destroy(&data->dongle[i].lock);
		pthread_mutex_destroy(&data->coder[i].mutex_burnout);
		pthread_mutex_destroy(&data->coder[i].mutex_done);
		i++;
	}
	pthread_mutex_destroy(&data->mutex_print);
	pthread_mutex_destroy(&data->mutex_simul);
	pthread_mutex_destroy(&data->queue_ctrl.lock);
	pthread_mutex_destroy(&data->heap_ctrl.lock);
	destroy_mutex_cond(data);
}
