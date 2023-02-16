BUILD_DIR = build
SRC_DIRS = src
# INCLUDES = include


SRCS = $(shell find $(SRC_DIRS) -name '*.c') server_socket.c 
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)

#CC specifies wich compiler we're using
CC = gcc
# -w suppresses al warnings
COMPILER_FLAGS = -Wall

#LINKER_FLAGS specifies the librairies we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm

#OBJ_NAME specifies the name of our executable
OBJ_NAME = server_socket


#this is the target that compiles our executable
#all : $(OBJS)
# $(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(BUILD_DIR)/$(OBJ_NAME)

#The final build step.
$(BUILD_DIR)/$(OBJ_NAME): $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(INCLUDES) -o $@ $(LINKER_FLAGS)
	
#Build step for C source
$(BUILD_DIR)/%.c.o: %.c	
	mkdir -p $(dir $@)
	$(CC) $(COMPILER_FLAGS) $(LINKER_FLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

.PHONY: run
run:
	make && ./build/$(OBJ_NAME)		

