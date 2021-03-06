/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_root.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mli <mli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/21 11:45:40 by mli               #+#    #+#             */
/*   Updated: 2020/09/26 14:22:23 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** note:	this function is the root function for geting the user input before
**			passing it to the parser.
**
** note:	returning NULL doesnt mean we have to quit the program, it could be
**			because we simply pressed ctrl_C, sowe will rather use a control
**			structure with flags for that.
**
** RETURN:	a linked list of the tokens
**			NULL
*/

t_list	*input_root(t_control *control)
{
	t_list *tokens_lst;

	control->ctrl_c = 0;
	control->term->prompt_ps1 = 1;
	if (!reset_prompts(control->env, control->term))
	{
		control->quit = 1;
		return (NULL);
	}
	if (!history_add_new_link(control))
	{
		control->quit = 1;
		return (NULL);
	}
	tokens_lst = input_root_assist_and_prompt(control);
	return (tokens_lst);
}

/*
** note:	this function should do the majority of the job getting the user
**			input
** note:	as above, returning NULL, doesnt mean we have to quit, we rather
**			use flags for this purpose in a control struct
**
** note:	we recheck the values of the window size between each loop.
**
** RETURN:	t_tokens_lst*
**			NULL
*/

t_list	*input_root_assist_and_prompt(t_control *control)
{
	t_list *tokens_lst;

	tokens_lst = NULL;
	while (!tokens_lst && !control->quit && !control->ctrl_c \
			&& !control->lexer_end.unexpected)
	{
		if (!terminfo_load_win_size(control->term) && (control->quit = 1))
			break ;
		control->term->clipboard.highlight = 0;
		if (control->term->prompt_ps1)
			ft_putstr_fd(control->term->ps1, 2);
		else
			ft_putstr_fd(control->term->ps2, 2);
		if (!input_reset_term_struct(control))
			return (NULL);
		tokens_lst = input_reading_and_lexing(control);
		control->term->prompt_ps1 = 0;
		ft_free((void **)&(control->term->line));
	}
	if (control->lexer_end.unexpected)
		input_synthax_error(control, control->lexer_end.unexpected);
//	debug_tokens_list(tokens_lst);
	//ft_lstclear(&tokens_lst, del_token);
	return (tokens_lst);
}

/*
** note:	this function is in charge of reseting the control->term fields
**			each time we restart afresh with a new prompt.
**
** RETURN:	1 ok
**			0 failure
*/

int		input_reset_term_struct(t_control *control)
{
	if (!terminfo_cursor_get_pos(control, &(control->term->cursor_start)))
		return (0);
	control->term->cursor = control->term->cursor_start;
	control->term->prompt_len = control->term->cursor.x;
	control->term->inline_position = -1;
	control->term->line_len = 0;
	control->term->current_history_link = control->history->head;
	return (1);
}

/*
** note:	this function will take care of the following main tasks:
**			-	read and append characters.
**			-	update control structure
**			-	check if a line ends with a '\'
**			-	update the curent history link
**			-	send the full content of the history link to the lexer.
**
** RETURN:	t_token_list *
**			NULL
*/

t_list	*input_reading_and_lexing(t_control *control)
{
	t_list *token_lst;

	token_lst = NULL;
	read_root(control, 0, 0);
	if (!terminfo_clipboard_disable_highlight(control))
		return (NULL);
	if (!terminfo_cursor_move_endl(control, 0))
		return (NULL);
	ft_putchar_fd('\n', 1);
	if (control->quit)
		return (NULL);
	if (!history_root(control, control->history))
		return (NULL);
	//printf("\n\033[38;5;27mENTERING LEXER WITH: [\033[0m%s\033[38;5;27m]\033[0m\n\n", control->history->head->content);
	token_lst = lexer_root((char *)(control->history->head->content), control);
	return (token_lst);
}

/*
** displays a synthax error right after the lexer stage (before the parser).
*/

void	input_synthax_error(t_control *control, unsigned int unexpected)
{
	ft_putstr_fd("minishell: synthax error near unexpected token `", 2);
	if (unexpected == SEMI)
		ft_putchar_fd(';', 2);
	else if (unexpected == PIPE)
		ft_putchar_fd('|', 2);
	else if (unexpected == OR_IF)
		ft_putstr_fd("||", 2);
	else if (unexpected == AND)
		ft_putchar_fd('&', 2);
	else if (unexpected == AND_IF)
		ft_putstr_fd("&&", 2);
	else if (unexpected == LBRACE)
		ft_putchar_fd('(', 2);
	else if (unexpected == RBRACE)
		ft_putchar_fd(')', 2);
	else if (unexpected == LESS || unexpected == DLESS || unexpected == GREAT \
			|| unexpected == DGREAT)
		ft_putstr_fd("newline", 2);
	ft_putstr_fd("'\n", 2);
	control->lexer_end.unexpected = 0;
}
