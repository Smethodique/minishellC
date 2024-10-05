#include "../minishell.h"

static int	*exit_code(void)
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

void	env(t_command *cmd, char **env)
{
	void env(t_command *cmd, char **env)
	{
    int i;

    if (cmd->args && double_pointer_len(cmd->args) != 1) 
	{
        ft_putstr_fd("minishell: env: too many arguments\n", 2);
        ft_setter(1);
        return;
    }

	ft_setter(0);
	while (env[i])
	{
		if (ft_strchr(env[i], '='))
			printf("%s\n", env[i]);
		i++;
	}
}