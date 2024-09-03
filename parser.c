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
int get_status()
{
    int status;
    waitpid(-1, &status, WNOHANG);
    return status;
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
    i++;
    start = i;

    // Loop to find the closing quote
    while (i < len) {
        if (input[i] == '\\' && quote_char == '"') {
            // In double quotes, backslash is special only for ", \, and $
            if (i + 1 < len && (input[i+1] == '"' || input[i+1] == '\\' || input[i+1] == '$')) {
                i++; // Skip the backslash
            }
        } else if (input[i] == quote_char) {
            break;
        }
        i++;
    }


    if (i < len) {
        char *quoted = ft_substr(input, start, i - start);

        // Check if there is an environment variable OR  exit_status in the quoted string
        if (quote_char == '"') {  // Only replace in double quotes
            char *result = ft_strdup("");
            char *temp = quoted;
            char *env_pos = strchr(temp, '$');
            // hand


            while (env_pos) {
                // Get the part before the environment variable
                char *before_env = ft_substr(temp, 0, env_pos - temp);
                char *env_name;
                char *env_value;
                size_t env_name_len = 0;

                // Determine the end of the environment variable name
                if (env_pos[1] == '{') {
                    env_name_len++;
                    while (env_pos[1 + env_name_len] != '}' && (isalnum(env_pos[1 + env_name_len]) || env_pos[1 + env_name_len] == '_')) {
                        env_name_len++;
                    }
                    env_name_len++; // Include closing brace
                } else {
                    while (isalnum(env_pos[1 + env_name_len]) || env_pos[1 + env_name_len] == '_') {
                        env_name_len++;
                    }
                }

                // Extract and fetch the environment variable value
                if (env_name_len > 0) {
                    env_name = ft_substr(env_pos + 1, 0, env_name_len);
                    env_value = getenv(env_name);
                    free(env_name);
                } else {
                    env_value = NULL;
                }

                // Replace the environment variable with its value if it exists
                if (env_value) {
                    char *temp_result = ft_strjoin(result, before_env);
                    result = ft_strjoin(temp_result, env_value);
                    temp = env_pos + 1 + env_name_len;
                } else {
                    // If no valid environment variable, replace with the it with empty string
                    //if we have $? we add the exit status

                    char *temp_result = ft_strjoin(result, before_env);
                    result = ft_strjoin(temp_result, "");
                    temp = env_pos + 1 + env_name_len;
                    //if we have just dollar sign we add it to the result skip the space

                    if (env_pos[1] == '\0' || isspace(env_pos[1]))
                    {
                        char *temp_result = ft_strjoin(result, "$");
                        result = ft_strjoin(temp_result, "");
                    }
                    //if we have $? we add the exit status token
                    int status = get_status();
                    if (env_pos[1] == '?') {
                        // Replace the $? with the exit status

                        char exit_status_str[12];
                        snprintf(exit_status_str, sizeof(exit_status_str), "%d", WEXITSTATUS(status));
                        char *temp_result = ft_strjoin(result, before_env);
                        result = ft_strjoin(temp_result, exit_status_str);
                        temp = env_pos + 2;
                        env_pos = strchr(temp, '$');
                        continue;
                    }





                }

                env_pos = strchr(temp, '$');
            }

            // Add the remaining part of the string after the last environment variable
            char *final_result = ft_strjoin(result, temp);
            // Add the final result as a token
            //if the final result is $? dont add it as a ARG

            add_token(&tokens, new_token(ARG, final_result));
        } else {
            // No environment variable found, just add the quoted string as a token
            add_token(&tokens, new_token(ARG, quoted));
        }

        i++;  // Skip closing quote
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
        if (current_char == '$' && next_char == '?' && quote_char == '\0') {
            add_token(&tokens, new_token(EXIT_STATUS, "$?"));
            i += 2;
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
        // remove quotes
        if (current_char == '$' && quote_char == '\0') {
            start = i;
            i++;
            if (i < len && input[i] == '{') {
                i++;
                if (i < len) i++; // Skip closing brace
            } else if (i < len && (isalnum(input[i]) || input[i] == '_')) {
                while (i < len && (isalnum(input[i]) || input[i] == '_')) i++;
            } else {
                add_token(&tokens, new_token(ARG, "$"));
                continue;
            }
            char *env_var = ft_substr(input, start, i - start);
            add_token(&tokens, new_token(ENV_VAR, env_var));
            continue;
        }




        // Handle command or argument
        start = i;
        while (i < len && !isspace(input[i]) && input[i] != '|' && input[i] != '<' && input[i] != '>' && input[i] != '\'' && input[i] != '"' && input[i] != '$') {
            i++;
        }
        char *word = ft_substr(input, start, i - start);
        if (expect_command) {
            add_token(&tokens, new_token(COMMANDE, word));
            expect_command = 0;
        } else if (word[0] == '-') {
            add_token(&tokens, new_token(OPTION, word));
        } else {
            //dont add arg if it start with $ or if it is empty
            printf("hiiii\n");
            if (word[0] != '"' && ft_strlen(word) > 0)
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
    int status = get_status();

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
            printf("tokens->value + 1: %s\n", tokens->value + 1);
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

            case EXIT_STATUS:
                if (!current_command) {
                    current_command = new_command();
                    add_command(&command_list, current_command);
                }
            char exit_status_str[12];
            snprintf(exit_status_str, sizeof(exit_status_str), "%d", WEXITSTATUS(status));
            add_argument(current_command, exit_status_str);
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