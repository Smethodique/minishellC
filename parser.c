#include "minishell.h"

t_token *new_token(int type, const char *value) {
    t_token *token = malloc(sizeof(t_token));
    if (!token) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    token->type = type;
    token->value = strdup(value);
    token->next = NULL;
    return token;
}

void add_token(t_token **head, t_token *new_token) {
    if (!*head) {
        *head = new_token;
    } else {
        t_token *temp = *head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = new_token;
    }
}

t_token *tokenize_input(const char *input) {
    t_token *tokens = NULL;
    int i = 0;
    int len = strlen(input);

    while (i < len) {
        if (isspace(input[i])) {
            i++;
            continue;
        }

        if (input[i] == '|') {
            add_token(&tokens, new_token(1, "|"));
            i++;
        } else if (input[i] == '>') {
            if (input[i+1] == '>') {
                add_token(&tokens, new_token(4, ">>"));
                i += 2;
            } else {
                add_token(&tokens, new_token(3, ">"));
                i++;
            }
        } else if (input[i] == '<') {
            if (input[i+1] == '<') {
                add_token(&tokens, new_token(5, "<<"));
                i += 2;
            } else {
                add_token(&tokens, new_token(2, "<"));
                i++;
            }
        } else {
            int start = i;
            while (i < len && !isspace(input[i]) && input[i] != '|' && input[i] != '>' && input[i] != '<') {
                i++;
            }
            char *word = strndup(&input[start], i - start);
            add_token(&tokens, new_token(0, word));
            free(word);
        }
    }
    return tokens;
}
