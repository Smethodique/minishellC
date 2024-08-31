#include "minishell.h"



void add_token(t_token **head, t_token *new_token) {
    if (*head == NULL) {
        *head = new_token;
    } else {
        t_token *current = *head;
        while (current->next) {
            current = current->next;
        }
        current->next = new_token;
    }
}
t_token *new_token(int type, const char *value) {
    t_token *token = malloc(sizeof(t_token));
    token->type = type;
    token->value = ft_strdup(value);
    token->next = NULL;
    return token;
}

t_token *tokenize_input(const char *input) {
    t_token *tokens = NULL;
    int i = 0;
    int len = strlen(input);
    int start = 0;
    int expect_command = 1;
    int expect_heredoc_delim = 0;
    int expect_filename = 0;   
    char current_char;
    char next_char;
    char quote_char = '\0';

    while (i < len) {
        current_char = input[i];
        next_char = (i + 1 < len) ? input[i + 1] : '\0';

        // Handle heredoc
        if (current_char == '<' && next_char == '<' && quote_char == '\0') {
            add_token(&tokens, new_token(HEREDOC, "<<"));
            i += 2;
            expect_heredoc_delim = 1;
            continue;
        }

        // Handle heredoc delimiter
   if (expect_heredoc_delim) {
    // Skip leading whitespace
    while (i < len && isspace(input[i])) {
        i++;
    }
    
    start = i;
    // Capture the delimiter until a whitespace or end of input is encountered
    while (i < len && !isspace(input[i])) {
        i++;
    }
    
    char *delimiter = ft_substr(input, start, i - start);
    add_token(&tokens, new_token(DELIMITER, delimiter));
    free(delimiter);
    expect_heredoc_delim = 0;
    continue;
}

        // Handle quotes
        if ((current_char == '\'' || current_char == '"') && quote_char == '\0') {
            quote_char = current_char;
            start = i;
            i++;
            while (i < len && input[i] != quote_char) {
                if (input[i] == '\\' && quote_char == '"' && i + 1 < len) {
                    i++; // Skip escaped character in double quotes
                }
                i++;
            }
            if (i < len) {
                i++; // Include closing quote
                char *quoted_value = ft_substr(input, start, i - start);
                int token_type = expect_command ? COMMANDE : ARG;
                add_token(&tokens, new_token(token_type, quoted_value));
                free(quoted_value);
                expect_command = 0;
            } else {
                ft_putstr_fd("Error: unclosed quote\n", 2);
                // Free tokens and return NULL (error handling code here)
                return NULL;
            }
            quote_char = '\0';
            continue;
        }

        // Skip whitespace outside quotes
        if (isspace(current_char) && quote_char == '\0') {
            i++;
            continue;
        }

        // Handle pipes
        if (current_char == '|' && quote_char == '\0') {
            add_token(&tokens, new_token(PIPE, "|"));
            i++;
            expect_command = 1;
            continue;
        }

        // Handle other redirections
        if ((current_char == '<' || current_char == '>') && quote_char == '\0') {
            if (current_char == '<') {
                 expect_filename = 1;
                add_token(&tokens, new_token(INPUT, "<"));
            } else if (current_char == '>' && next_char == '>') {
                expect_filename = 1;
                add_token(&tokens, new_token(APPEND, ">>"));
                i++;
            } else {
                expect_filename = 1;
                add_token(&tokens, new_token(OUTPUT, ">"));
            }
            i++;
            continue;
        }
         if(expect_filename)
         {
            start = i;
            while (i < len && !isspace(input[i]) && input[i] != '|' && input[i] != '<' && input[i] != '>' && input[i] != '\'' && input[i] != '"') {
                i++;
            }
            char *filename = ft_substr(input, start, i - start);
            add_token(&tokens, new_token(FILENAME, filename));
            free(filename);
            expect_filename = 0;
            continue;
        }
      //handle env var

      unsigned int i = 0;
while (i < len && input[i]) {
    if (input[i] == '$') {
        size_t var_start = i + 1; // Skip the '$' character

        // Capture the environment variable name
        while (i < len && (isalnum((unsigned char)input[i]) || input[i] == '_')) {
            i++;
        }

        if (var_start == i) {
            // Handle the case where there is nothing after the '$'
            add_token(&tokens, new_token(ENV_VAR, strdup("$")));
            continue;
        }

        char *env_var_name = ft_substr(input, var_start, i - var_start);

        // Expand the environment variable
        char *env_value = getenv(env_var_name);

        if (env_value) {
            add_token(&tokens, new_token(ENV_VAR, env_value));
        } else {
            // If the environment variable is not found, add an empty string
            add_token(&tokens, new_token(ENV_VAR, ""));
        }

        free(env_var_name);
    } else {
        i++;
    }
}
            
        // Handle command or argument
        start = i;
        while (i < len && !isspace(input[i]) && input[i] != '|' && input[i] != '<' && input[i] != '>' && input[i] != '\'' && input[i] != '"') {
            i++;
        }
        char *word = ft_substr(input, start, i - start);
        if (expect_command) {
            add_token(&tokens, new_token(COMMANDE, word));
            expect_command = 0;
        } else if (word[0] == '-') {
            add_token(&tokens, new_token(OPTION, word));
        } else {
            add_token(&tokens, new_token(ARG, word));
        }
        free(word);
    }

    return tokens;
}
void print_tokens(t_token *tokens) {
    while (tokens) {
        printf("Type: %d, Value: %s\n", tokens->type, tokens->value);
        tokens = tokens->next;
    }
}

t_command *new_command() {
    t_command *cmd = malloc(sizeof(t_command));
    cmd->name = NULL;
    cmd->args = malloc(sizeof(char*) * 64);  // Start with space for 64 arguments
    cmd->arg_count = 0;
    cmd->pipe_next = 0;
    cmd->redirections = NULL;
    cmd->next = NULL;
    return cmd;
}

void add_argument(t_command *cmd, char *arg) {
    if (cmd->arg_count == 0) {
        cmd->name = ft_strdup(arg);
    }
    cmd->args[cmd->arg_count++] = ft_strdup(arg);
    cmd->args[cmd->arg_count] = NULL;  // Ensure null-termination
}

void add_redirection(t_command *cmd, int type, char *filename) {
    t_redirection *redir = malloc(sizeof(t_redirection));
    redir->type = type;
    redir->filename = ft_strdup(filename);
    redir->next = cmd->redirections;
    cmd->redirections = redir;
}

void add_command(t_command **list, t_command *cmd) {
    if (*list == NULL) {
        *list = cmd;
    } else {
        t_command *current = *list;
        while (current->next) {
            current = current->next;
        }
        current->next = cmd;
    }
}

int validate_syntax(t_token *tokens) {
    int command_count = 0;
    t_token *current = tokens;

    // Ensure the first token is not a pipe
    if (current && current->type == PIPE) {
        fprintf(stderr, "Error: Invalid syntax near '|'\n");
        return 0;
    }

    while (current) {
        if (current->type == COMMANDE) {
            command_count++;
        } else if (current->type == PIPE) {
            // Ensure a command exists before the pipe
            if (command_count == 0) {
                fprintf(stderr, "Error: Invalid syntax near '|'\n");
                return 0;
            }
            // Reset command count after a pipe and ensure the next token is a command
            command_count = 0;
        }
        current = current->next;
        //ensure that the last token is not a pipe  
        if (current == NULL && command_count == 0) {
            fprintf(stderr, "Error: Invalid syntax near end of input\n");
            return 0;
        }
    }

  

    return 1;
}


t_command *parse_tokens(t_token *tokens) {
    t_command *command_list = NULL;
    t_command *current_command = NULL;
    int expect_heredoc_delim = 0;

    if (!validate_syntax(tokens)) {
        return NULL;
    }

    while (tokens) {
        switch (tokens->type) {
            case COMMANDE:
                if (!current_command) {
                    current_command = new_command();
                    add_command(&command_list, current_command);
                }
                add_argument(current_command, tokens->value);
                break;

            case ARG:
            case OPTION:
                if (!current_command) {
                    current_command = new_command();
                    add_command(&command_list, current_command);
                }
                add_argument(current_command, tokens->value);
                break;

         case ENV_VAR:
            if (!current_command) {
                current_command = new_command();
                add_command(&command_list, current_command);
            }
            char *env_value = getenv(tokens->value + 1);
            if (env_value) {
                add_argument(current_command, env_value);
            } else {
                // In bash, non-existent env vars are treated as empty strings
                add_argument(current_command, "");
            }
            break;

            case INPUT:
            case OUTPUT:
            case APPEND:
                if (!current_command) {
                    current_command = new_command();
                    add_command(&command_list, current_command);
                }
                if (!tokens->next || (tokens->next->type != FILENAME && tokens->next->type != ARG)) {
                    fprintf(stderr, "Error: Missing target after redirection\n");
                    return NULL;
                }
                add_redirection(current_command, tokens->type, tokens->next->value);
                tokens = tokens->next; // Skip the filename token
                break;

            case HEREDOC:
                if (!current_command) {
                    current_command = new_command();
                    add_command(&command_list, current_command);
                }
                expect_heredoc_delim = 1;
                break;

            case DELIMITER:
                if (!expect_heredoc_delim) {
                    fprintf(stderr, "Error: Unexpected delimiter '%s'\n", tokens->value);
                    return NULL;
                }
                add_redirection(current_command, HEREDOC, tokens->value);
                expect_heredoc_delim = 0;
                break;

            case PIPE:
                if (!current_command) {
                    fprintf(stderr, "Error: Pipe without a preceding command\n");
                    return NULL;
                }
                current_command->pipe_next = 1;
                current_command = NULL;
                break;

            default:
                fprintf(stderr, "Error: Unexpected token type\n");
                return NULL;
        }

        tokens = tokens->next;
    }

    if (expect_heredoc_delim) {
        fprintf(stderr, "Error: Missing heredoc delimiter\n");
        return NULL;
    }

    return command_list;
}
void free_command(t_command *cmd) {
    if (cmd->name) free(cmd->name);
    for (int i = 0; i < cmd->arg_count; i++) {
        free(cmd->args[i]);
    }
    free(cmd->args);
    
    t_redirection *redir = cmd->redirections;
    while (redir) {
        t_redirection *next = redir->next;
        free(redir->filename);
        free(redir);
        redir = next;
    }
    
    free(cmd);
}

void free_command_list(t_command *list) {
    while (list) {
        t_command *next = list->next;
        free_command(list);
        list = next;
    }
}

void print_command_list(t_command *list) {
    while (list) {
        if (list->name) {
            printf("Command: %s\n", list->name);
        } else {
            printf("Command: (unnamed)\n");
        }
        
        printf("Arguments: ");
        for (int i = 0; i < list->arg_count; i++) {
            printf("%s ", list->args[i]);
        }
        printf("\n");

        t_redirection *redir = list->redirections;
        while (redir) {
            printf("Redirection: type %d, file %s\n", redir->type, redir->filename);
            redir = redir->next;
        }

        if (list->pipe_next) {
            printf("Pipes to next command\n");
        }

        printf("\n");
        list = list->next;
    }
}