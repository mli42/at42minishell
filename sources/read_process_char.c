/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_process_char.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mli <mli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/21 11:19:16 by mli               #+#    #+#             */
/*   Updated: 2020/08/21 11:19:18 by mli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** this file is assisting the file read_root
*/

/*
** note:	this function is in charge of adding a character that if printable
*/

void	read_process_add_char(t_control *control, char c)
{
	if (!(ft_strcadd(&(control->term->line), c, \
					control->term->inline_position + 1)))
	{
		control->quit = 1;
		return ;
	}
	control->term->inline_position++;
	control->term->line_len++;
	if (!terminfo_insert_char(control, c))
	{
		control->quit = 1;
		return ;
	}
}

/*
** note:	this function will delete effectively a character from the curent
**			string, if the cursor is not against the prompt.
*/

void	read_process_del_char(t_control *control)
{
	if (control->term->inline_position == -1)
		return ;
	ft_strcdel(control->term->line, control->term->inline_position);
	if (!terminfo_del_char(control))
		return ;
	control->term->inline_position--;
	control->term->line_len--;
}

/*
** note:	this function will be called when a special key is hit on the
**			keyboard: either one of the arrows, 'delete=>', 'home', 'end',
**			'page_down', 'page_up', or 'esc'.
** input:	-	control struct
**			-	c: the '\033' sequence.
*/

void	read_process_special_key(t_control *control, int i)
{
	if (i == KEY_RIGHT_ID)
		terminfo_cursor_move_right(control);
	else if (i == KEY_LEFT_ID)
		terminfo_cursor_move_left(control);
	else if (i == KEY_HOME_ID)
		read_process_control_combo(control, CTRL_A_COMBO);
	else if (i == KEY_END_ID)
		read_process_control_combo(control, CTRL_E_COMBO);
	else if (i == KEY_UP_CTRL_ID)
		terminfo_cursor_move_up(control, &control->term->cursor);
	else if (i == KEY_DOWN_CTRL_ID)
		terminfo_cursor_move_down(control, &control->term->cursor);
	else if (i == KEY_RIGHT_CTRL_ID)
		terminfo_cursor_move_next_word(control);
	else if (i == KEY_LEFT_CTRL_ID)
		terminfo_cursor_move_previous_word(control);
	else if (i == KEY_UP_ID)
		history_search(control, 1);
	else if (i == KEY_DOWN_ID)
		history_search(control, 0);
}

/*
** note:	this is the root function for adding special features when the user
**			uses a ctrl + letter combinaison.
** note:	the flags like control->quit wil have to be raised in case of error
*/

void	read_process_control_combo(t_control *control, char c)
{
	if (c == CTRL_A_COMBO)
	{
		if (!terminfo_cursor_move_endl(control, 1))
			return ;
		control->term->inline_position = -1;
		control->term->cursor = control->term->cursor_start;
	}
	else if (c == CTRL_E_COMBO)
	{
		if (!terminfo_cursor_move_endl(control, 0))
			return ;
		control->term->inline_position = control->term->line_len - 1;
	}
	else if (c == CTRL_P_COMBO)
	{
		terminfo_clipboard_paste(control, &control->term->clipboard);
	}
}
