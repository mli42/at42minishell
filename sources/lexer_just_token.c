/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_just_token.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mli <mli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/21 11:37:18 by mli               #+#    #+#             */
/*   Updated: 2020/08/21 11:37:19 by mli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** this file is a subset of the lexer_root.c and lexer_find_token.c
** it is taking care of the case when the token starts by anything expect for
** an operator or a quotation mark (or it can be an escaped one).
*/

/*
** note:	this function is going to be called when the token is neither going
**			to be an operator or a quote.
** note:	when we meet an escape sequence, we still add it to the token, but
**			we know we will see the next character as a simple character.
**
** RETURN:	1 ok
**			0 if malloc failed.
*/

int	lexer_just_token(const char *input, int *j, t_token *token)
{
	while (input[*j] && (token->esc_next || input[*j] != ' '))
	{
		if (!token->esc_next && ft_strchr("\"\'", input[*j]))
			return (lexer_quoted(input, j, token));
		else if (!token->esc_next && ft_strchr("&<>()|;", input[*j]))
			return (1);
		else if (!token->esc_next && input[*j] == '\\')
		{
			token->esc_next = 1;
			if (!ft_strappend(&(token->str), input[*j]))
				return (0);
			(*j)++;
			continue;
		}
		else if (!ft_strappend(&(token->str), input[*j]))
			return (0);
		token->esc_next = 0;
		(*j)++;
	}
	return (1);
}

/*
** note:	this function is called when the first character of the token is a
**			number. it could possibly be part of an indirection as in the
**			formulas: [n]<word OR [n]>word OR [n]>>word ...
**
** RETURN:	1 ok
**			0 if failure in malloc
*/

int	lexer_token_or_indirection(const char *input, int *j, t_token *token)
{
	while (ft_isdigit(input[*j]))
	{
		if (!ft_strappend(&(token->str), input[*j]))
			return (0);
		(*j)++;
	}
	if (input[*j] && input[*j] != ' ')
	{
		if (ft_strchr("&()|;", input[*j]))
			return (1);
		else if (ft_strchr("<>", input[*j]))
			return (lexer_indirection(input, j, token));
		else
			return (lexer_just_token(input, j, token));
	}
	return (1);
}

/*
** note:	this function is going to be called when we the beginning of the
**			token with only numbers (not even an escape sequence!) and the
**			current character is either '<' or '>' so we are in the case of an
**			indirection token.
** RETURN:	1 ok
**			0 if failure
*/

int	lexer_indirection(const char *input, int *j, t_token *token)
{
	if (input[*j] == '<')
		token->id = LESS;
	else
		token->id = GREAT;
	return (lexer_operator2(input, j, token));
}
