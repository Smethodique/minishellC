
#ifndef MINISHELL_H
#define MINISHELL_H


typedef struct s_token {
    int type;
    char *value;
    struct s_token *next;
} t_token;




void init_shell();
void sigint_handler(int sig);
void sigquit_handler(int sig);
void all_signals();
t_token *new_token(int type, const char *value);
void add_token(t_token **head, t_token *new_token);
t_token *tokenize_input(const char *input);
void print_tokens(t_token *tokens);


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