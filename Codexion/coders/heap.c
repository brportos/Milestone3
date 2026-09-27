/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brportos <brportos@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 19:17:25 by brportos          #+#    #+#             */
/*   Updated: 2026/09/07 19:28:02 by brportos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	heap_compare(t_coder *curr, t_coder *coder);
static void	heap_check_deadline(t_heap *heap, int i);
static void	heap_swap(t_coder **tree, int i, int j);

void	heap_pop(t_heap *heap, t_coder *coder)
{
	int	i;

	i = 0;
	while (i < heap->size)
	{
		if (heap->tree[i] == coder)
			break ;
		i++;
	}
	if (i == heap->size)
		return ;
	heap->size--;
	heap->tree[i] = heap->tree[heap->size];
	while (i > 0 && heap_compare(heap->tree[i], heap->tree[(i - 1) / 2]))
	{
		heap_swap(heap->tree, i, (i - 1) / 2);
		i = (i - 1) / 2;
	}
	heap_check_deadline(heap, i);
}

void	heap_push(t_heap *heap, t_coder *coder)
{
	int	i;

	i = heap->size;
	heap->tree[i] = coder;
	heap->size += 1;
	while (i > 0)
	{
		if (heap_compare(heap->tree[i], heap->tree[(i - 1) / 2]))
		{
			heap_swap(heap->tree, i, (i - 1) / 2);
			i = (i - 1) / 2;
		}
		else
			break ;
	}
}

static int	heap_compare(t_coder *curr, t_coder *coder)
{
	return (get_burnout(curr) < get_burnout(coder));
}

static void	heap_swap(t_coder **tree, int i, int j)
{
	t_coder	*tmp;

	tmp = tree[i];
	tree[i] = tree[j];
	tree[j] = tmp;
}

static void	heap_check_deadline(t_heap *heap, int i)
{
	int	lft;
	int	rgt;
	int	small;

	while ((i * 2) + 1 < heap->size)
	{
		lft = (i * 2) + 1;
		rgt = (i * 2) + 2;
		small = lft;
		if (rgt < heap->size && heap_compare(heap->tree[rgt], heap->tree[lft]))
			small = rgt;
		if (heap_compare(heap->tree[small], heap->tree[i]))
		{
			heap_swap(heap->tree, i, small);
			i = small;
		}
		else
			break ;
	}
}
