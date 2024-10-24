/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stakhtou <stakhtou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 17:05:45 by stakhtou          #+#    #+#             */
/*   Updated: 2024/10/21 14:06:18 by stakhtou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	*exit_code(void)
{
	static int	c;

	return (&c);
}

int	ft_getter(void)
{
	int	*i;

	i = exit_code();
	return (*i);
}

void	ft_setter(int value)
{
	int	*i;

	i = exit_code();
	*i = value;
}

int	double_pointer_len(char **str)
{
	int	i;

	i = 0;
	if (!str)
		return (0);
	while (str[i])
		i++;
	return (i);
}

void	increment_shlvl(char **env)
{
	int		i;
	int		shlvl;

	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], "SHLVL=", 6) == 0)
		{
			shlvl = atoi(env[i] + 6);
			shlvl++;
			while (env[i])
			{
				if (strncmp(env[i], "SHLVL=", 6) == 0)
				{
					env[i] = ft_strjoin("SHLVL=", ft_itoa(shlvl));
					break ;
				}
			}
			return ;
		}
		i++;
	}
}
