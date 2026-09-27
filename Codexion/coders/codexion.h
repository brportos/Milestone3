/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brportos <brportos@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 19:18:11 by brportos          #+#    #+#             */
/*   Updated: 2026/09/16 07:43:45 by brportos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# define _DEFAULT_SOURCE
# define _XOPEN_SOURCE 600

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

# define MAX_CODERS 400
# define FIFO "fifo"
# define EDT "edf"

typedef struct s_dongle
{
	long			cooldown;
	pthread_mutex_t	lock;
}					t_dongle;

typedef struct s_coder
{
	struct s_data	*data;
	t_dongle		*ldongle;
	t_dongle		*rdongle;
	long			time_burnout;
	int				id;
	int				coder_compiled;
	int				have_done;
	pthread_t		thread;
	pthread_mutex_t	mutex_burnout;
	pthread_mutex_t	mutex_done;
}					t_coder;

typedef struct s_queue
{
	t_coder			*coder;
	struct s_queue	*next;
}					t_queue;

typedef struct s_heap
{
	t_coder			*tree[MAX_CODERS];
	int				size;
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
}					t_heap;

typedef struct s_queue_manager
{
	t_queue			*first;
	t_queue			*last;
	pthread_mutex_t	lock;
	pthread_cond_t	cond;

}					t_queue_manager;

typedef struct s_data
{
	t_coder			*coder;
	t_dongle		*dongle;
	t_heap			heap_ctrl;
	t_queue_manager	queue_ctrl;
	long			max_burnout;
	long			time_compile;
	long			time_debug;
	long			time_refactor;
	long			dongle_cooldown;
	long			start_time;
	char			*scheduler;
	int				ncoder;
	int				ndongle;
	int				required_compile;
	int				active_simulation;
	pthread_t		monitoring_id;
	pthread_mutex_t	mutex_print;
	pthread_mutex_t	mutex_simul;
}					t_data;
void				display_log(int i, char *action, t_data *data);
int					display_error(char *string, char *details, t_data *data);

long				get_time_ms(void);
long				get_simul_time(t_data *data);

long				get_burnout(t_coder *coder);
int					check_burnout(t_data *data, int *done);
void				set_burnout(t_coder *coder);

void				heap_pop(t_heap *heap, t_coder *coder);
void				heap_push(t_heap *heap, t_coder *coder);

void				scheduler_edf_add(t_data *data, t_coder *coder);
int					scheduler_fifo(t_data *data, t_coder *coder, char *action);
int					isfifo(t_data *data);

int					take_dongle(t_coder *coder);
void				release_dongles(t_coder *coder, t_data *data);
void				init_dongles_mutex(t_data *data);

int					isargs_valid(t_data *data, char **argv);
int					add_to_queue(t_queue_manager *manager, t_coder *coder);
int					ispriority(t_data *data, t_coder *coder);
void				join_thread(t_data *data);
int					do_action(t_coder *coder, char *action);
int					get_have_done(t_coder *coder);
void				set_done(t_coder *coder);

void				create_coders_and_dongles(t_data *data);

void				init_mutex(t_data *data);
void				init_pthread(t_data *data);
int					init_struct(t_data *data);
void				init_basic_data(t_data *data, t_coder *coder, int count);

void				stop_simulation(t_data *data);
int					get_simulation(t_data *data);

void				*coder_start_routine(void *arg);
void				*monitoring_simulation(void *arg);

void				destroy_mutex(t_data *data);
void				free_memory(t_data *data);
int					remove_from_queue(t_queue_manager *manager);
#endif
