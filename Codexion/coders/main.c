/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brportos <brportos@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 19:18:43 by brportos          #+#    #+#             */
/*   Updated: 2026/09/29 14:19:55 by brportos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_data	data;

	memset(&data, 0, sizeof(t_data));
	if (argc != 9)
		return (display_error("Argument invalid", NULL, &data));
	init_mutex(&data);
	if (isargs_valid(&data, argv) == 1)
	{
		destroy_mutex(&data);
		return (1);
	}
	init_pthread(&data);
	join_thread(&data);
	destroy_mutex(&data);
	free_memory(&data);
	return (0);
}
