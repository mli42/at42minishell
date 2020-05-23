#include "minishell.h"

void		parser_disp(t_token *node)
{
	int i;
	int fd = 1;
	char *str = (node ? node->str : "null");
//	int len = ft_strlen(str);

	i = -1;
	ft_putstr_fd("[", fd);
	while (++i < SIZE_LEAF - 2 && str[i])
		ft_putchar_fd(str[i], fd);
	while (i++ < SIZE_LEAF - 2)
		ft_putchar_fd(' ', fd);

	ft_putstr_fd("]", fd);
}

void		del_ast(t_btree **node)
{
	if (!*node)
		return ;
	del_ast(&(*node)->left);
	del_ast(&(*node)->right);
	if (!(*node)->left && !(*node)->right)
	{
//		printf("DEL: [%s]\n", ((t_token *)(*node)->item)->str);
		ft_free((void **)node);
	}
}

int			token_id(t_token *token)
{
	if (!token)
		return (-1);
	return (token->id);
}

void		ast_add(t_btree **ast, t_btree *add)
{
		if (*ast == NULL)
			*ast = add;
		else if ((*ast)->left == NULL)
			(*ast)->left = add;
		else if ((*ast)->item == NULL)
		{
			(*ast)->item = add->item;
			ft_free((void **)&add);
		}
		else if ((*ast)->right == NULL)
			(*ast)->right = add;
		else
		{
			add->left = *ast;
			*ast = add;
		}
}

// For specifying children + special state for cmds (in, maybe, a struct)
// Ex : job argvs &
int			parser_cmd(t_list **tklst, t_btree *new)
{
	if (token_id((*tklst)->content) == TOKEN && (*tklst)->next &&
			token_id((*tklst)->next->content) == TOKEN)
	{
		(*tklst) = (*tklst)->next;
		// node->left =  linked list containing parameters
		while ((*tklst) && token_id((*tklst)->content) == TOKEN)
			(*tklst) = (*tklst)->next;
		if ((*tklst) && token_id((*tklst)->content) == AND)
		{
			if (!(new->right = btree_new((*tklst)->content)))
				return (0);
			(*tklst) = (*tklst)->next;
		}
		return (1);
	}
	return (0);
}

int			did_move(int tkid)
{
	if (tkid == LBRACE)
		return (1);

//	if (tkid == RBRACE ||
//		tkid == SEMI ||
//		tkid == LBRACE)
//		return (1);
	return (0);
}

t_btree		*parser_assist(t_dlist *dlst, t_list **tklst);

// Filling New with the new child
// Return -1 if allocation pb,
// 0 if tklst has no more tokens
// 1 on success
int		parser_next_child(t_dlist **dlst, t_list **tklst, t_btree **new)
{
//	printf("ENTER WITH : [%d]\t'%s'\n",
//		token_id((*tklst)->content), ((t_token *)(*tklst)->content)->str);
	if (!did_move(token_id((*tklst)->content)))
		*new = btree_new((*tklst)->content);
	else
	{
		*tklst = (*tklst)->next;
		if (!*tklst)
			return (0);
		if (!((*dlst)->next = ft_dlstnew(NULL)))
			return (-1);
		if (!(*new = parser_assist((*dlst)->next, tklst)))
			return (-1);
		ft_free((void **)&((*dlst)->next));

//	printf("\nNEW CONTAINS\n");
//		btree_debug(*new, parser_disp);

//		debug_tokens_list(*tklst);
	}

//	if ((*new)->item)
//		printf("New is : [%d]\t'%s'\n", token_id((*new)->item), ((t_token *)(*new)->item)->str);
	(void)dlst;
	return (1);
}

t_btree		*parser_assist(t_dlist *dlst, t_list **tklst)
{
	t_btree		*new;
	int			state;

	if (!(dlst->content = btree_new(NULL)))
		return (NULL);
	while (*tklst && token_id((*tklst)->content) != RBRACE)
	{
//		printf("%d\n", token_id(tklst->content));
		if ((state = parser_next_child(&dlst, tklst, &new)) == 0)
			break ;
		else if (state == -1)
			return (0);
		ast_add((t_btree **)&dlst->content, new);
//		if (dlst->next)
//		{
//			new->right = dlst->next->content;
//			ft_free((void **)&dlst->next);
//		}
		if (!parser_cmd(tklst, new))
			*tklst = (*tklst)->next;
	}
//	btree_debug(dlst->content, parser_disp);
	return (dlst->content);
}

t_btree		*parser_root(t_list *tklst, t_control *control)
{
	t_dlist		*dlst;
	t_btree		*ast;

	ast = NULL;
	if ((dlst = ft_memalloc(sizeof(*dlst))) && parser_assist(dlst, &tklst))
	{
		printf("Nb of Btrees : %d\n", ft_lstsize((t_list *)dlst));
		ast = dlst->content;
		/** {
			t_dlist		*lst = dlst;
			while (lst)
			{
				btree_debug(lst->content, parser_disp);
				lst = lst->next;
			}

		} */
		btree_debug(ast, parser_disp);
	}
	else
	{
//		control->quit = 1;
	}

	ft_dlstclear(&dlst, NULL);
	del_ast(&ast);
	return (ast);
	(void)control;
}
