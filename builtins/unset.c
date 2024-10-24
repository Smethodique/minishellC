/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stakhtou <stakhtou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 17:05:45 by stakhtou          #+#    #+#             */
/*   Updated: 2024/10/21 16:20:20 by stakhtou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

size_t	length(char *s)
{
	size_t	i;

	i = 0;
	while (s[i] && s[i] != '=')
	{
		i++;
	}
	return (i);
}

int	check_dup_env(char *cmd)
{
	int	i;

	i = 0;
	while (g_vars.env[i])
	{
		if (!ft_strncmp(g_vars.env[i], cmd, length(g_vars.env[i])))
			return (i);
		else
			i++;
	}
	return (0);
}

void	unset_helper(char *cmd, int len)
{
	int	checker;
	int	i;

	checker = check_dup_env(cmd);
	if (checker && (length(g_vars.env[checker]) == length(cmd)))
	{
		i = checker;
		while (i < len - 1)
		{
			g_vars.env[i] = g_vars.env[i + 1];
			i++;
		}
		g_vars.env[len - 1] = NULL;
	}
}

void	unset(t_command *cmd)
{
	int	i;
	int	len;

	i = 1;
	if (cmd->args[1])
	{
		while (cmd->args[i])
		{
			len = 0;
			while (g_vars.env[len])
				len++;
			unset_helper(cmd->args[i], len);
			i++;
		}
	}
}
