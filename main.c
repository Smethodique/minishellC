#include "minishell.h"



void init_shell()
{
   while(1)
    {
         char *line = readline("minishell$ ");
         if (line == NULL)
         {
              break;
         }
         if (ft_strlen(line) > 0)
         {
              print_tokens(tokenize_input(line));
              add_history(line);
         }
         free(line);
    }
}

int main(int ac , char **av, char **env)
{
   (void)ac;
    (void)av;
    (void)env;
     all_signals();
    init_shell();
}