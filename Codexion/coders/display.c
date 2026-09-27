/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brportos <brportos@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 19:18:15 by brportos          #+#    #+#             */
/*   Updated: 2026/09/12 10:36:55 by brportos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	display_error(char *string, char *details, t_data *data)
{
	if (data != NULL)
		free_memory(data);
	fprintf(stderr, "\033[31mError\033[0m: %s", string);
	if (details != NULL)
		fprintf(stderr, "%s", details);
	fprintf(stderr, "\n");
	return (1);
}

void	display_log(int i, char *action, t_data *data)
{
	long	time;

	pthread_mutex_lock(&data->mutex_print);
	time = get_time_ms() - data->start_time;
	if (strcmp(action, "takedongle") == 0)
		printf("%ld %d has taken a dongle\n", time, i);
	else if (strcmp(action, "compile") == 0)
		printf("%ld %d is compiling\n", time, i);
	else if (strcmp(action, "debug") == 0)
		printf("%ld %d is debugging\n", time, i);
	else if (strcmp(action, "refactor") == 0)
		printf("%ld %d is refactoring\n", time, i);
	else if (strcmp(action, "burns_out") == 0)
		printf("%ld %d burned out\n", time, i);
	pthread_mutex_unlock(&data->mutex_print);
}
