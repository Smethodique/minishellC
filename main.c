#include "minishell.h"
void make_history(t_history_manager *manager)
{
    char *line;
    while (1)
    {
        line = readline("minishell$ ");
        if (!line)
            continue;
        if (line[0] == '\0')
        {
            free(line);
            continue;
        }
            if (line[0] != '\0' && line[0] != ' ')
        {
            add_history_entry(manager, line);
            if (strcmp(line, "history") == 0)
            {
                print_history(manager);
            }
        }
        free(line);
    }
}
void init_shell(t_history_manager *manager)
{
    make_history(manager);
}

void free_history_manager(t_history_manager *manager)
{
    t_history *current = manager->head;
    t_history *next;

    while (current)
    {
        next = current->next;
        free(current->line);
        free(current);
        current = next;
    }

    free(manager);
}

int main()
{
    t_history_manager *manager = create_history_manager();
    init_shell(manager ); 
    free_history_manager(manager);
    return 0;
}
