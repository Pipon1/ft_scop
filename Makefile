SHELL := /bin/bash

NAME := ft_scop
SRC_DIR := src/
OBJ_DIR := obj/
INC_DIR := include/
INCLUDE_DIR := /usr/include
LIB_DIR := /usr/lib
SRC_FILES := main glad image_loader
SRC := $(addprefix $(SRC_DIR), $(addsuffix .cpp, $(SRC_FILES)))
OBJ := $(addprefix $(OBJ_DIR), $(addsuffix .o, $(SRC_FILES)))
CC := g++ -g -fsanitize=address

# Define colors
GREEN := \033[0;32m
YELLOW := \033[1;33m
NC := \033[0m # No Color

# Compiler Flags
CXXFLAGS := -std=c++17 \
            -I$(INCLUDE_DIR) \
            -I$(INC_DIR)

# Linker Flags
LDFLAGS := -L$(LIB_DIR) \
			-lglfw \
			-lGL \
        	-lX11 \
			-lpthread \
			-lXrandr \
			-ldl

define LOADING_BAR
	@echo -n "Compiling $<: "
	@for i in {1..5}; do \
		echo -ne "$(YELLOW)#"; \
		sleep 0.1; \
	done; \
	echo -e "$(GREEN) Done!$(NC)"
endef

all: $(NAME)

# Compilation Rule
$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(OBJ_DIR)
	$(LOADING_BAR)
	@$(CC) $(CXXFLAGS) -c $< -o $@

# Linking Rule
$(NAME): $(OBJ)
	@$(CC) $(OBJ) $(LDFLAGS) -o $(NAME)

# Clean Rules
clean: 
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
