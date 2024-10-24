/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stakhtou <stakhtou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 17:05:45 by stakhtou          #+#    #+#             */
/*   Updated: 2024/10/24 03:50:57 by stakhtou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wordexp.h>

char	*expand_path(const char *path)
{
	wordexp_t	p;
	char		**w;
	char		*expanded_path;

	expanded_path = NULL;
	if (wordexp(path, &p, 0) == 0)
	{
		w = p.we_wordv;
		if (p.we_wordc > 0)
		{
			expanded_path = strdup(w[0]);
		}
		wordfree(&p);
	}
	return (expanded_path);
}

int	handle_redirection(t_redirection *redir, int fd_in)
{
	int		new_fd;
	char	*expanded;

	new_fd = fd_in;
	if (redir->type == INPUT || redir->type == HEREDOC)
	{
		if (new_fd != fd_in)
			close(new_fd);
		expanded = expand_path(redir->filename);
		if (redir->type == INPUT)
		{
			if (check_file(expanded, O_RDONLY) == -1
				|| check_directory(get_directory_path(expanded), 0) == -1)
			{
				free(expanded);
				return (-1);
			}
		}
		new_fd = open(expanded, O_RDONLY);
		free(expanded);
		if (new_fd == -1)
			return (-1);
	}
	return (new_fd);
}

int	get_in(t_command *cmd, int fd_in)
{
	t_redirection	*redir;
	int				new_fd;

	redir = cmd->redirections;
	new_fd = fd_in;
	while (redir)
	{
		new_fd = handle_redirection(redir, new_fd);
		if (new_fd == -1)
		{
			return (-1);
		}
		redir = redir->next;
	}
	return (new_fd);
}

int	get_out(t_command *cmd, int fd_out)
{
	t_redirection	*red;
	int				new_fd;
	char			*expanded;

	red = cmd->redirections;
	new_fd = fd_out;
	while (red)
	{
		if (red->type == OUTPUT || red->type == APPEND)
		{
			expanded = expand_path(red->filename);
			if (ft_strlen(expanded) == 0 || get_in(cmd, STDIN_FILENO) == -1
				|| check_directory(get_directory_path(expanded), 1) == -1)
				return (ft_putstr_fd("minishell: ", 2), -1);
			if (new_fd != fd_out)
				close(new_fd);
			if (red->type == OUTPUT)
				new_fd = open(expanded, O_WRONLY | O_CREAT | O_TRUNC, P);
			else
				new_fd = open(expanded, O_WRONLY | O_CREAT | O_APPEND, P);
			dup2(new_fd, fd_out);
		}
		red = red->next;
	}
	return (new_fd);
}
