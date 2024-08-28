#include "minishell.h"

typedef enum {
   COMMANDE,
   ARG,
    REDIRECTION,
    PIPE,
    APPEND,
    INPUT,
    OUTPUT,
    FILENAME,
    DELIMITER,
    OPTION,
    ENV_VAR,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    SEMICOLON,
    AND,
    OR,
    PARENTHESIS_OPEN,
    PARENTHESIS_CLOSE

} t_token_type;

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
    int expect_filename = 0;
    char current_char;
    char next_char;

    while (i < len) {
        current_char = input[i];
        next_char = (i + 1 < len) ? input[i + 1] : '\0';

        // Skip whitespace
        if (isspace(current_char)) {
            i++;
            continue;
        }

        // Handle pipes and OR
        if (current_char == '|') {
            if (next_char == '|') {
                add_token(&tokens, new_token(OR, "||"));
                i += 2;
            } else {
                add_token(&tokens, new_token(PIPE, "|"));
                i++;
            }
            expect_command = 1;
            continue;
        }

        // Handle AND
        if (current_char == '&' && next_char == '&') {
            add_token(&tokens, new_token(AND, "&&"));
            i += 2;
            expect_command = 1;
            continue;
        }

        // Handle semicolon
        if (current_char == ';') {
            add_token(&tokens, new_token(SEMICOLON, ";"));
            i++;
            expect_command = 1;
            continue;
        }

        // Handle parentheses
        if (current_char == '(') {
            add_token(&tokens, new_token(PARENTHESIS_OPEN, "("));
            i++;
            expect_command = 1;
            continue;
        }
        if (current_char == ')') {
            add_token(&tokens, new_token(PARENTHESIS_CLOSE, ")"));
            i++;
            continue;
        }

        // Handle redirections
        if (current_char == '<' || current_char == '>') {
            if (next_char == current_char) {
                char redir[3] = {current_char, current_char, '\0'};
                add_token(&tokens, new_token(current_char == '<' ? INPUT : APPEND, redir));
                i += 2;
            } else {
                char redir[2] = {current_char, '\0'};
                add_token(&tokens, new_token(current_char == '<' ? INPUT : OUTPUT, redir));
                i++;
            }
            expect_filename = 1;
            continue;
        }

        // Handle quotes for strings
             if (current_char == '\'' || current_char == '"') {
    char quote_char = current_char;
    start = i;
    i++; // Move past the opening quote
    
    while (i < len) {
        if (input[i] == quote_char) {
            if (i + 1 < len && input[i + 1] == quote_char) {
                // Empty quotes or consecutive quotes
                i += 2;
            } else {
                // End of quoted string
                break;
            }
        } else {
            i++;
        }
    }

    if (i < len) {
        // Include the quotes in the token value
        char *quoted_value = ft_substr(input, start, i - start + 1);
        int token_type = (expect_command) ? COMMANDE : ARG;
        add_token(&tokens, new_token(token_type, quoted_value));
        free(quoted_value);
        i++; // Move past the closing quote
        expect_command = 0;
    } else {
        // Unclosed quote, handle error
        fprintf(stderr, "Error: Unclosed quote\n");
        return tokens;
    }
    continue;
}
        // Handle environment variables
        if (current_char == '$') {
            start = i;
            i++;
            while (i < len && (isalnum(input[i]) || input[i] == '_')) {
                i++;
            }
            add_token(&tokens, new_token(ENV_VAR, ft_substr(input, start, i - start)));
            continue;
        }

        // Handle command or argument
        start = i;
        while (i < len && !isspace(input[i]) && input[i] != '|' && input[i] != '<' && input[i] != '>' &&
               input[i] != '&' && input[i] != ';' && input[i] != '(' && input[i] != ')' && input[i] != '$') {
            i++;
        }
        char *word = ft_substr(input, start, i - start);
        if (expect_command) {
            add_token(&tokens, new_token(COMMANDE, word));
            expect_command = 0;
        } else if (expect_filename) {
            add_token(&tokens, new_token(FILENAME, word));
            expect_filename = 0;
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
