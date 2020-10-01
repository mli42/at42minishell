/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mli <mli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/21 10:08:14 by mli               #+#    #+#             */
/*   Updated: 2020/10/01 11:36:33 by mli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** this file will be in charge of anything in relation to environment variables
*/

/*
** note:	this function will represent the env buit-in facility
**			it just displays the env list, and set the exit status to 0
**
** RETURN:	always 1. and therefore the exit_status is always 0
*/

int		env_builtin(t_list *env, t_control *control)
{
	while (env)
	{
		ft_putstr_fd(((t_env *)env->content)->label, 1);
		ft_putstr_fd("=", 1);
		ft_putendl_fd(((t_env *)env->content)->value, 1);
		env = env->next;
	}
	control->exit_status = 0;
	return (1);
}

/*
** note:	this function will build the env linked list. using as input the
**			exter char **environ
**
** RETURN:	t_list *
**			NULL malloc failed
*/

t_list	*env_build_linked_list(char **env)
{
	t_list	*lst_head;
	t_list	*lst_link;
	t_env	*env_struct;
	int		i;

	i = 0;
	lst_head = NULL;
	while (env[i])
	{
		if (!(env_struct = env_build(env[i])))
		{
			ft_lstclear(&lst_head, env_del_struct);
			return (NULL);
		}
		if (!(lst_link = ft_lstnew(env_struct)))
		{
			env_del_struct(env_struct);
			ft_lstclear(&lst_head, env_del_struct);
			return (NULL);
		}
		ft_lstadd_back(&lst_head, lst_link);
		i++;
	}
	return (lst_head);
}

/*
**	note:	this function will be called when its necessary to free the list of
**			t_env structs.
*/

void	env_del_struct(void *env)
{
	free(((t_env *)env)->label);
	free(((t_env *)env)->value);
	free((t_env *)env);
}

/*
** note:	This function will get the string either imported from the env
**			variable list that we are converting at the start. Or from the
**			export utility. in the second case, the validity of the string has
**			already been checked (equal sign present, str not starting by a '='
**			or a '\')
**			It split str at the first '=' sign found into two elements of a
**			struct: label and value.
**
** RETURN:	t_env *
**			NULL if some malloc failed.
*/

t_env	*env_build(char *str)
{
	int		i;
	t_env	*new;

	if ((i = ft_strichr(str, '=')) == -1)
		return (NULL);
	if (!(new = env_init_struct()))
		return (NULL);
	if (!(new->label = ft_substr(str, 0, i++)) ||
		!(new->value = ft_substr(str + i, 0, ft_strlen(str + i))))
	{
		env_del_struct(new);
		return (NULL);
	}
	return (new);
}

/*
** note:	this function will init the env struc and set its subfields to NULL
**
** RETURN:	null if failure in malloc
**			t_env *
*/

t_env	*env_init_struct(void)
{
	t_env *new;

	if (!(new = (t_env *)ft_memalloc(sizeof(t_env))))
		return (NULL);
	return (new);
}
