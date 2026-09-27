/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brportos <brportos@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 19:19:00 by brportos          #+#    #+#             */
/*   Updated: 2026/09/16 12:56:30 by brportos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (1);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

long	get_simul_time(t_data *data)
{
	return (get_time_ms() - data->start_time);
}
