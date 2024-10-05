#include "../minishell.h"

int is_n_option(char *arg)
{
    int i = 0;
    if (arg[i] == '-')
        i++;
    else
        return 0;
    while (arg[i]) {
        if (arg[i] != 'n')
            return 0;
        i++;
    }
    return 1;
}

int first_non_option(char **env)
{
    int i = 1; 
    while (env[i] && is_n_option(env[i]))
        i++;
    return i;
}

int echo(t_command *cmd, char **env)
{
	(void)cmd;
    int i = first_non_option(env);

    while (env && env[i])
    {
        printf("%s", env[i]);
        if (env[++i])
            printf(" ");
    }
    if (!is_n_option(env[1]) || !env[1])
        printf("\n");

    return 0;
}