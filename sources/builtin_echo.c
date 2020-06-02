#include "minishell.h"

/*
** this file is a subset of the the builtin_root.c file.
** this file will take care of the builtin echo
*/

/*
** note:	this implementation of the echo builtin only takes one option:
**			the -n.
** note:	we assume the argv[0] is the name of the utility.
** note:	the exit_status is always set to zero.
**
** RETURN:	1 on success always.
*/

int	echo_builtin(char **argv, t_control *control)
{
	int i;
	int n_option;

	i = 0;
	n_option = 0;
	if (argv[0])
		i++;
	while (argv[i])
	{
		if (i == 1 && !ft_strncmp(argv[i], "-n", ft_strlen(argv[i])))
		{
			i++;
			n_option = 1;
			continue;
		}
		ft_putstr_fd(argv[i], 1);
		i++;
		if (argv[i])
			ft_putchar_fd(' ', 1);
	}
	if (!n_option)
		ft_putendl_fd("", 1);
	control->exit_status = 0;
	return (1);
}
