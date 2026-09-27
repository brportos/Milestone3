NAME = codexion

CC = cc -g
CFLAGS = -Wall -Wextra -Werror -pthread


SRC = coders/coders.c \
	coders/main.c \
	coders/dongle.c \
	coders/arg.c \
	coders/frees.c \
	coders/init.c \
	coders/simul.c \
	coders/heap.c \
	coders/scheduler.c \
	coders/utils.c \
	coders/burnout.c \
	coders/display.c \
	coders/time.c \
	coders/routine.c 

OBJ = $(SRC:%.c=%.o)

all: $(NAME)

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $(OBJ)
	@echo "codexion is created"

clean:
	@rm -rf $(OBJ)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
