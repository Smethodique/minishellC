
#ifndef MINISHELL_H
#define MINISHELL_H

typedef struct s_history
{
    char *line;
    struct s_history *next;
    int index;
} t_history;

typedef struct s_history_manager
{
    t_history *head;
    t_history *tail;
    int count;
} t_history_manager;

typedef struct s_token {
    int type;
    char *value;
    struct s_token *next;
} t_token;





t_history_manager *create_history_manager(void);
void add_history_entry(t_history_manager *manager, const char *line);
t_history **get_history(t_history_manager *manager);
void print_history(t_history_manager *manager);
void free_history_manager(t_history_manager *manager);
void make_history(t_history_manager *manager);
void free_history(t_history_manager *manager);


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <errno.h>
#include "1337Libft/libft.h"

#endif