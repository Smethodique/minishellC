/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_pipe_helper.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stakhtou <stakhtou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 17:05:45 by stakhtou          #+#    #+#             */
/*   Updated: 2024/10/24 03:04:59 by stakhtou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	close_unused_fds(int red_in, int red_out, int in_fd, int out_fd)
{
	int	fd;

	fd = 3;
	while (fd < 256)
	{
		if (fd != red_in && fd != red_out && fd != in_fd && fd != out_fd)
		{
			close(fd);
		}
		fd++;
	}
}

void	execute_command(t_command *cmd, char **env)
{
	reset_signals();
	if (is_builtin(cmd) != NOT_BUILT_IN)
	{
		execute_builtin(cmd, env, is_builtin(cmd));
		exit(g_vars.exit_status);
	}
	else
	{
		cmd->args[0] = get_path(cmd->args);
		if (!cmd->args[0])
		{
			ft_putstr_fd("minishell: command not found: ", 2);
			exit(127);
		}
		if (execve(cmd->args[0], cmd->args, env) == -1)
		{
			ft_putstr_fd("minishell: error executing command: ", 2);
			exit(127);
		}
	}
}

pid_t	execute_piped_command(t_command *cmd, int in_fd, int out_fd, char **env)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		ft_printf("minishell: fork failed\n");
		exit(1);
	}
	if (pid == 0)
	{
		setup_child_signals();
		setup_redirections(cmd, in_fd, out_fd);
		close_unused_fds(get_in(cmd, in_fd), get_out(cmd, out_fd), in_fd,
			out_fd);
		execute_command(cmd, env);
	}
	return (pid);
}

int	count_pipes(t_command *commands)
{
	int			pipe_count;
	t_command	*current;

	pipe_count = 0;
	current = commands;
	while (current)
	{
		pipe_count++;
		current = current->next;
	}
	return (pipe_count);
}

void	setup_pipe(int pipes[2][2], int i)
{
	if (pipe(pipes[i % 2]) == -1)
	{
		perror("pipe failed");
		exit(1);
	}
}
