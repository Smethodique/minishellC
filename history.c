#include "minishell.h"

t_history_manager *create_history_manager()
{
    t_history_manager *manager = malloc(sizeof(t_history_manager));
    if (!manager)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    manager->head = NULL;
    manager->tail = NULL;
    manager->count = 0;
    return manager;
}

void add_history_entry(t_history_manager *manager, const char *line)
{
    t_history *new_entry = malloc(sizeof(t_history));
    if (!new_entry)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    new_entry->line = ft_strdup(line);
    if (!new_entry->line)
    {
        perror("strdup");
        exit(EXIT_FAILURE);
    }
    new_entry->next = NULL;
    new_entry->index = manager->count + 1;
    if (!manager->head)
    {
        manager->head = new_entry;
        manager->tail = new_entry;
    }
    else
    {
        manager->tail->next = new_entry;
        manager->tail = new_entry;
    }

    manager->count++;
}

t_history **get_history(t_history_manager *manager)
{
    t_history **list = malloc((manager->count + 1) * sizeof(t_history *));
    if (!list)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    t_history *current = manager->head;
    int i = 0;
    while (current)
    {
        list[i] = current;
        current = current->next;
        i++;
    }
    list[manager->count] = NULL;

    return list;
}



void print_history(t_history_manager *manager)
{
    t_history **list = get_history(manager);
     t_history **temp = list; 
   while (*list)
    {
        printf("%d: %s\n", (*list)->index, (*list)->line);
        list++;
    }
    free(temp);
}

