NAME = minishell
SRCS =  main.c  signals.c parser.c
OBJS = $(SRCS:.c=.o)
LIBFT = 1337Libft/libft.a
CC = cc
CFLAGS = -Wall -Wextra -Werror 
LDFLAGS = -L 1337Libft -lft -lreadline

all: $(NAME)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBFT):
	make -C 1337Libft

clean:
	rm -f $(OBJS)
	make -C 1337Libft clean

fclean: clean
	rm -f $(NAME)
	make -C 1337Libft fclean

re: fclean all

.PHONY: all clean fclean re