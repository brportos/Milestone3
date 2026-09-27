/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brportos <brportos@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 19:19:04 by brportos          #+#    #+#             */
/*   Updated: 2026/09/17 08:45:38 by brportos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	ispriority(t_data *data, t_coder *coder)
{
	int		lelf_i;
	int		right_i;
	long	my_time;

	lelf_i = (coder->id - 2 + data->ncoder) % data->ncoder;
	right_i = coder->id % data->ncoder;
	my_time = get_burnout(coder);
	if (get_have_done(&data->coder[lelf_i]) == 0
		&& get_burnout(&data->coder[lelf_i]) < my_time)
		return (0);
	if (get_have_done(&data->coder[right_i]) == 0
		&& get_burnout(&data->coder[right_i]) < my_time)
		return (0);
	return (1);
}

int	do_action(t_coder *coder, char *action)
{
	if (get_simulation(coder->data) == 0)
		return (0);
	else if (strcmp(action, "compile") == 0)
	{
		set_burnout(coder);
		display_log(coder->id, "compile", coder->data);
		usleep(coder->data->time_compile * 1000);
		pthread_mutex_lock(&coder->mutex_done);
		coder->coder_compiled += 1;
		pthread_mutex_unlock(&coder->mutex_done);
		release_dongles(coder, coder->data);
	}
	else if (strcmp(action, "debug") == 0)
	{
		display_log(coder->id, "debug", coder->data);
		usleep(coder->data->time_debug * 1000);
	}
	else if (strcmp(action, "refactor") == 0)
	{
		display_log(coder->id, "refactor", coder->data);
		usleep(coder->data->time_refactor * 1000);
	}
	return (0);
}

void	join_thread(t_data *data)
{
	int	i;

	i = 0;
	pthread_join(data->monitoring_id, NULL);
	while (i != data->ncoder)
	{
		pthread_join(data->coder[i].thread, NULL);
		i++;
	}
}

int	get_have_done(t_coder *coder)
{
	int	done;

	done = 0;
	pthread_mutex_lock(&coder->mutex_done);
	done = coder->have_done;
	pthread_mutex_unlock(&coder->mutex_done);
	return (done);
}

void	set_done(t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex_done);
	coder->have_done = 1;
	pthread_mutex_unlock(&coder->mutex_done);
}
