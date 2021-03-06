/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_dlstadd_front.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mli <mli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/14 22:37:18 by mli               #+#    #+#             */
/*   Updated: 2020/08/21 09:38:31 by mli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** input:	- arg1: head of list (ex: &head)
**			- arg2: new link to be added
**
** note2:	if *alst is NULL, it means the list was empty. new list is created
*/

void	ft_dlstadd_front(t_dlist **alst, t_dlist *new)
{
	if (!alst || !new)
		return ;
	if (*alst)
	{
		(*alst)->previous = new;
		new->next = *alst;
	}
	*alst = new;
}
