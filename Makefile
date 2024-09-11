# Name of the final executable
NAME = philo

# Source files
SOURCES =	src/philo.c \
			src/forks.c \
			src/utils.c \
			src/supervisor.c \
			src/parse_init_cleanup.c \

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Compiler and flags
CC = cc
CFLAGS = -Wall -Werror -Wextra

# Target for all
all: $(NAME)

# Target for the final executable
$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS)

# Rule to build object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean object files
clean:
	@rm -f $(OBJECTS)
	
# Full clean, including the executable and libft
fclean: clean
	@rm -f $(NAME)

# Rebuild everything
re: fclean all

# Phony targets
.PHONY: all clean fclean re
