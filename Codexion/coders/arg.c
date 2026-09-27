/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brportos <brportos@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 19:17:53 by brportos          #+#    #+#             */
/*   Updated: 2026/09/25 12:42:47 by brportos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	parse_data(t_data *data, int *argc);

static int	is_valid_digit_string(char *str)
{
	int	i;

	i = 0;
	if (!str || str[0] == '\0')
		return (0);
	if (str[0] == '+')
		i++;
	if (str[i] == '\0')
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	isargs_valid(t_data *data, char **argv)
{
	int	i;
	int	parsed[8];

	i = 1;
	while (i < 8)
	{
		if (!is_valid_digit_string(argv[i]))
			return (display_error("Invalid: ", argv[i], data));
		parsed[i - 1] = atoi(argv[i]);
		if ((i == 1 || i == 2) && parsed[i - 1] < 1)
			return (display_error("Invalid ", argv[i], data));
		if (i == 1 && parsed[i - 1] >= MAX_CODERS)
			return (display_error("Invalid ", argv[i], data));
		if (i >= 3 && i <= 7 && parsed[i - 1] < 0)
			return (display_error("Invalid ", argv[i], data));
		i++;
	}
	if (parse_data(data, parsed) || init_struct(data))
		return (1);
	if (strcmp(FIFO, argv[i]) == 0 || strcmp(EDT, argv[i]) == 0)
		data->scheduler = argv[i];
	else
		return (display_error("Invalid argument ", argv[i], data));
	return (0);
}

static int	parse_data(t_data *data, int *parsed_argv)
{
	data->coder = malloc(sizeof(t_coder) * parsed_argv[0]);
	if (!data->coder)
		return (display_error("Can't allocate ", NULL, data));
	data->dongle = malloc(sizeof(t_dongle) * parsed_argv[0]);
	if (!data->dongle)
		return (display_error("Can't allocate ", NULL, data));
	data->ncoder = parsed_argv[0];
	data->ndongle = parsed_argv[0];
	data->max_burnout = parsed_argv[1];
	data->time_compile = parsed_argv[2];
	data->time_debug = parsed_argv[3];
	data->time_refactor = parsed_argv[4];
	data->required_compile = parsed_argv[5];
	data->dongle_cooldown = parsed_argv[6];
	return (0);
}

int	add_to_queue(t_queue_manager *manager, t_coder *coder)
{
	t_queue	*queue;

	queue = malloc(sizeof(t_queue));
	if (!queue)
		return (1);
	queue->coder = coder;
	queue->next = NULL;
	if (manager->first == NULL)
	{
		manager->first = queue;
		manager->last = queue;
	}
	else
	{
		manager->last->next = queue;
		manager->last = queue;
	}
	return (0);
}
