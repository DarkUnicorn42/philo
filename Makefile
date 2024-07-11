# Name of the final executable
NAME = philo

# Source files
SOURCES =	src/pipex.c \
		src/utils.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Compiler and flags
CC = cc
CFLAGS = -Wall -Werror -Wextra -Iincludes

# Paths
LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

# Target for all
all: $(LIBFT) $(NAME)

# Target for the final executable
$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS) $(LIBFT)

# Rule to build object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build the libft library
$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

# Clean object files
clean:
	@rm -f $(OBJECTS)
	@$(MAKE) -C $(LIBFT_DIR) clean
	
# Full clean, including the executable and libft
fclean: clean
	@rm -f $(NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean

# Rebuild everything
re: fclean all

# Phony targets
.PHONY: all clean fclean re
