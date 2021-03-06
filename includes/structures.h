/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structures.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mli <mli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/21 09:56:29 by mli               #+#    #+#             */
/*   Updated: 2020/09/29 09:51:53 by mli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTURES_H
# define STRUCTURES_H

/*
** structure for the position of the cursror
*/
typedef struct	s_int_pair
{
	int				x;
	int				y;
}				t_int_pair;

/*
** structure for the clipboard (copy, cut, paste)
** highlight: activated with ctrl_k for visual select mode.
** note: the index start and end are inclusive.
*/
typedef struct	s_clipboard
{
	int				highlight;
	int				start;
	int				end;
	int				swaped_end;
	char			*paste_me;
}				t_clipboard;

/*
** structure that contains dataregardig the terminal, helpful for the termcaps.
** line: the current line we will update after prompt.
*/
typedef struct	s_term
{
	char			*line;
	int				line_len;
	int				inline_position;
	t_int_pair		cursor;
	t_int_pair		cursor_saved;
	t_int_pair		cursor_start;
	t_int_pair		size_window;
	int				prompt_ps1;
	char			*ps1;
	char			*ps2;
	int				prompt_len;
	char			**array_esc_seq;
	t_dlist			*current_history_link;
	t_clipboard		clipboard;
}				t_term;

/*
** structure that handles the history of commands. modified with up and down
** arrows.
** head: the first link contains the current history string. We append to it
** control->term->line again and again if we have to prompt PS2.
*/
typedef struct	s_history
{
	int				size;
	t_dlist			*head;
	int				max_size;
}				t_history;

typedef struct	s_lexer_end
{
	unsigned int other:1;       // Pour '|', '||', '&&'    => PS2 simple
	unsigned int quote:1;       // Pour '\'' , '\"'        => PS2
	unsigned int backslash:1;   // Pour '\\'               => PS2 + Del '\'
	unsigned int brace:1;       // Pour '(''               => PS2
	unsigned int add_semi:1;    // Pour '(''               => PS2 + Append ';'
	unsigned int unexpected;    // Pour ')', '>>', '>' ... => Génère une erreur
}				t_lexer_end;

/*
** see lexing.h for the defined values of the id.
**
** unquote_protected marks a token that should not be subject to quote removal
** at the time of word expansion before executing the command.
** Within that token, the start and end of the protection are marked by the
** unquote_start  and unquote_end
*/
typedef struct	s_token
{
	char			*str;
	int				id;
	int				is_filename;
	unsigned int	open_quote:1;
	unsigned int	esc_next:1;
	int				unquote_protected;
	int				protect_s;
	int				protect_e;
}				t_token;

/*
** structure for controling the whole minishell.
*/
typedef struct	s_control
{
	unsigned int	quit:1;
	unsigned int	ctrl_c:1;
	int				exit_status;
	pid_t			parent_pid;
	t_history		*history;
	t_term			*term;
	t_list			*env;
	t_lexer_end		lexer_end;
	int				truefd[3];
	struct termios	termios_default;
	char			*cwd;
}				t_control;

/*
** This structure contains everything required by a simple command to be run.
** redirections will need to be operated in the order of the linked list.
** the indirections and redirections bothe contains a linked list of t_arrow*.
**	argv_list will hold temporary the argv words until expansion is done.
*/
typedef struct	s_simple_cmd
{
	t_list	*arrows;
	char	**argv;
}				t_simple_cmd;

/*
** this structure holds filename, and type which can be either GREAT, DGREAT,
** LESS, DLESS.
*/
typedef struct	s_arrow
{
	char	*src;
	char	*dest;
	int		id;
}				t_arrow;

/*
** this structure is used in the linked list containing the environement
**	variables.
*/
typedef struct	s_env
{
	char	*label;
	char	*value;
	int		tmpdeclare;
}				t_env;

/*
** structure assising in the process of parameter expansion and field splitting
** start: the position in the string where the $ is at.
** end:	the index of the last char belonging to the variable.
** quoted: will help us know if the field_splitting will be applied or not.
** ifs: internal field separator. stored here for when it comes to the field
**		splittting part.
*/
typedef struct	s_expansion
{
	int			start;
	int			end;
	char		quoted;
	int			is_filename;
	char		*ifs;
}				t_expansion;

/*
** note:	this structure will be used to break the path in path_parts while
**			in the process of pathname expansion (executing simple command)
**
** path:	the string we obtain after spliting the path with the '/' chars.
** star_index: a linked list of the index of the star operators that are valid
** quoted:	helps us knowing if the begining of the string is in a quoted
**			section	and the kind of quotes.
*/

typedef struct	s_path_part
{
	char		*path_part;
	t_list		*star_index;
	char		quoted;
}				t_path_part;

/*
**	note:	this structure will be used while trying to match a token
**			containing a valid kleen star operator, with possible path names.
**			path_parts: contains a linked list of t_path_part structures.
**			first_match: will let us if we have at least one match already,
**			is_filename: with first_match will help abort in the case of an
**			ambiguous redirect.
*/

typedef	struct	s_path_exp
{
	t_list		*path_parts;
	int			is_filename;
	int			first_match;
	t_list		*matched_paths;
}				t_path_exp;

/*
** note:	this structure will only help us with the norm and clarity.
**			it is used while unquoting strings. If the string is not protected
**			the  values are set to -1.
**			both fiels contain the inclusive start and end index of the
**			protected section of the string.
*/

typedef struct	s_no_unquote
{
	int	start;
	int	end;
}				t_no_unquote;

/*
** note:	this structure is used in the pathname expansion section.
**			it only contains the two trings to be compared.
*/

typedef struct	s_strings
{
	char	*str;
	char	*ref;
}				t_strings;

/*
** note:	A global variable to know which function have to be executed
**			for a precise token id. (+) avoid une foret de if
*/

typedef struct	s_exe
{
	int		id;
	int		(*fct)(t_btree *, t_control *);
}				t_exe;

typedef struct	s_pipe
{
	int	pid;
	int	fildes[2];
	int	status;
}				t_pipe;

#endif
