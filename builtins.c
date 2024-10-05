
void    execute_builtin(t_command *cmd, char **env, int index)
{
    void (*built_in[6])(t_command*, char**) = {
        cd, my_env, export, unset, echo, pwd
    }
    built_in[index](cmd, env);
}

int    is_builtin(t_command *cmd)
{
	char *built_in[6] = {
        "cd", "env", "export", "unset", "echo", "pwd"
    };
	int i = 6;
	while (--i >= 0)
	{
		if (ft_strncmp(cmd->args[0], built_in[i],
        	ft_strlen(built_in[i]) + 1) == 0)
		{
			return i;
		}
	}
	return i;
}