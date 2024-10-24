/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_cd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stakhtou <stakhtou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 17:05:45 by stakhtou          #+#    #+#             */
/*   Updated: 2024/10/21 14:31:49 by stakhtou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <stdbool.h>

void	expand_tilde(char **env, char **path)
{
	char	*home;
	int		i;
	char	*expanded_path;

	home = NULL;
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], "HOME=", 5) == 0)
		{
			home = env[i++] + 5;
			break ;
		}
	}
	if (!home)
	{
		g_vars.exit_status = 1;
		return ;
	}
	if ((*path)[1] == '/' || (*path)[1] == '\0')
		expanded_path = ft_strjoin(home, *path + 1);
	else
		expanded_path = ft_strdup(*path);
	free(*path);
	*path = expanded_path;
}

void	handle_tilde(char **env, char **path)
{
	char	*home;
	int		i;

	i = 0;
	home = NULL;
	while (env[i])
	{
		if (ft_strncmp(env[i], "HOME=", 5) == 0)
		{
			home = env[i] + 5;
			break ;
		}
		i++;
	}
	if (!home)
	{
		ft_putstr_fd("minishell: cd: HOME not set\n", 2);
		g_vars.exit_status = 1;
		return ;
	}
	if ((*path)[0] == '~')
	{
		expand_tilde(env, path);
	}
}

void	cd_part1(t_command *cmd, char **env, char **path)
{
	if (!cmd->args || !cmd->args[1] || ft_isspace(cmd->args[1][0])
		|| cmd->args[1][0] == '\0' || ft_strncmp(cmd->args[1], "--", 3) == 0)
	{
		*path = get_env_value("HOME", env);
		if (!*path || **path == '\0' || ft_isspace(**path))
		{
			g_vars.exit_status = 1;
			return ;
		}
	}
	else if (ft_strncmp(cmd->args[1], "-", 2) == 0)
	{
		*path = get_env_value("OLDPWD", env);
		if (!*path)
		{
			g_vars.exit_status = 1;
			return ;
		}
		ft_putstr_fd(*path, 1);
	}
	else
	{
		*path = ft_strdup(cmd->args[1]);
		handle_tilde(env, path);
	}
}

void	cd_part2(t_command *cmd, char **env, char *path)
{
	if (cmd->args[1] && cmd->args[2])
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", 2);
		g_vars.exit_status = 1;
		free(path);
		return ;
	}
	if (!change_dir(env, path))
		g_vars.exit_status = 1;
	else
	{
		g_vars.exit_status = 0;
	}
}

void	cd(t_command *cmd, char **env)
{
	char	*path;

	path = NULL;
	cd_part1(cmd, env, &path);
	if (g_vars.exit_status == 0 && path)
	{
		cd_part2(cmd, env, path);
	}
}
