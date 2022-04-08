# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Generic.mk                                         :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: unknow <unknow@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/12/05 07:17:02 by riblanc           #+#    #+#              #
#    Updated: 2022/04/04 14:38:42 by riblanc          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

-include CompileUtils/Common.mk

ifndef DEFINES_TMP
DEFINES_TMP := $(NAME) $(DEFINES)
endif

ifndef NAME
    NAME = a.out 
    NAME_ATTR = DEFAULT
else
    NAME_ATTR = USER
endif

# Check pkg-config #############################################################

ifneq ($(shell pkg-config --help 1>/dev/null 2>/dev/null; echo $$?), 0)
    $(error pkg-config must be installed.)
endif

# Sources ######################################################################

ifndef SRC_DIR
    SRC_DIR  = ./src
    SRC_DIR_ATTR = DEFAULT
else
    SRC_DIR_ATTR = USER
endif

ifndef SRC_LIST
    SRC_LIST = main.cpp
    SRCS_ATTR = DEFAULT
else
    SRCS_ATTR = USER
endif

SRCS     = $(addprefix $(SRC_DIR)/,$(SRC_LIST))

# Objs *########################################################################

ifndef OBJ_DIR
    OBJ_DIR  = ./obj
    OBJ_DIR_ATTR = DEFAULT
else
    OBJ_DIR_ATTR = USER
endif

OBJ_LIST = $(SRC_LIST:.cpp=.o)
OBJS     = $(addprefix $(OBJ_DIR)/$(NAME)/, $(OBJ_LIST))
DEPS     = $(OBJS:%.o=%.d)

OBJS_ATTR = DEFAULT
DEPS_ATTR = DEFAULT

# Includes #####################################################################

ifndef INC_DIR
    INC_DIR  = ./include
endif

INCLUDES = -I$(INC_DIR)

INCLUDES_ATTR = DEFAULT

# Check if dbg has changed #####################################################

DBG_F    = $(if $(findstring dbg,$(MAKECMDGOALS)),1,0)
ASAN_F   = $(if $(findstring asan,$(MAKECMDGOALS)),1,0)
DBG      = 0

ifeq ($(DBG_F),1)
    ifeq ($(ASAN_F),1)
        $(error Cannot use both asan and dbg)
    endif
endif

ifneq ($(findstring re,$(MAKECMDGOALS)),re)
    ifeq ($(shell test -f $(OBJ_DIR)/$(NAME)/.DBG.1; echo $$?),0)
        ifneq ($(ASAN_F),1)
            DBG = 1
        endif
    else ifeq ($(shell test -f $(OBJ_DIR)/$(NAME)/.DBG.2; echo $$?),0)
        ifneq ($(DBG_F),1)
            DBG = 2
        endif
    endif
endif

# Compilation ##################################################################

ifndef CC
    CC = clang++
    CC_ATTR = DEFAULT
else
    CC_ATTR = USER
endif

ifndef CFLAGS_TMP
    ifndef CFLAGS
        CFLAGS_ATTR = DEFAULT
    else
        CFLAGS_ATTR = USER
    endif
    
    CFLAGS += -MMD $(INCLUDES)
    
    ifeq ($(MAKELEVEL), 0)
        ifneq ($(filter 1,$(DBG_F) $(DBG)),)
            CFLAGS += -g3
        else ifneq ($(filter 1,$(ASAN_F) $(if $(filter 2,$(DBG)),1)),)
            CFLAGS += -g3 -fsanitize=address
        endif
    endif
    
    CFLAGS_TMP := $(CFLAGS)
    export CFLAGS_TMP
else
	CFLAGS := $(CFLAGS_TMP)
endif

ifndef GFLAGS
    GFLAGS = 
    GFLAGS_ATTR = DEFAULT
else
    GFLAGS_ATTR = USER
endif

# Definition of some constants #################################################

RUN_ARGS ::= $(wordlist 1,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))   # Get args
ND       ::= --no-print-directory
DBG_DEP    = $(OBJ_DIR)/$(NAME)/.DBG.$(DBG)

RUN_ARGS_ATTR = AUTO

# Display configuration ########################################################

ifeq ($(MAKELEVEL),0)
    $(call print_config)
else ifeq ($(FORCE_HEADER),1)
    $(call print_config)
endif

# Define rules #################################################################

all: | $(DIR_NAME)
ifeq ($(COMPILE_ALL),true)
	@$(foreach n,$(DEFINES_TMP),$(call submake, NAME=$n $n DEFINES=))
else
	@$(call submake, $(NAME) DEFINES=)
endif

ifeq ($(filter 1,$(DBG_F) $(ASAN_F)),1)
$(DEFINES) $(NAME): FORCE
	@:
else ifeq ($(filter re,$(MAKECMDGOALS)),re)
$(DEFINES) $(NAME): FORCE
	@:
else ifeq ($(filter clean,$(MAKECMDGOALS)),clean)
$(DEFINES) $(NAME): FORCE
	@:
else ifeq ($(filter fclean,$(MAKECMDGOALS)),fclean)
$(DEFINES) $(NAME): FORCE
	@:
else ifeq ($(SUBNAME),1)
$(DEFINES) $(NAME): $(OBJS)
	$(CC) $^ $(CFLAGS) -o $@ $(GFLAGS)
else
$(DEFINES) $(NAME): FORCE
	@$(call submake,-j4 NAME=$@ $@ DEFINES= SUBNAME=1)
endif

-include $(DEPS)
$(OBJ_DIR)/$(NAME)/%.o: $(SRC_DIR)/%.cpp $(DBG_DEP) | $(OBJ_DIR)/$(NAME)
	$(CC) $(CFLAGS) -D _$(call UC,$(NAME)) -o $@ -c $<

$(OBJ_DIR)/$(NAME):
	mkdir -p $(OBJ_DIR)/$(NAME)

FORCE:

ifeq ($(findstring re,$(MAKECMDGOALS)),re)   # Here, we check if 're' is called.
dbg: | fclean                                # If it's the case, so we'll call
	@$(call subfilter,dbg re, DBG=1)         # the fclean rule for both dbg and
asan: | fclean                               # asan to be sure to fclean before
	@$(call subfilter,asan re, DBG=2)        # make again no matter if re is
else                                         # 'called' before or after the
dbg:                                         # debugging rules.
	@$(call subfilter,dbg, DBG=1)            # Else, if 're' isn't called,
asan:                                        # we simply call make again with
	@$(call subfilter,asan, DBG=2 )          # DBG variable set to the according
endif                                        # value.

$(OBJ_DIR)/$(NAME)/.DBG.$(DBG): | $(OBJ_DIR)/$(NAME)
	@rm -f $(OBJ_DIR)/$(NAME)/.DBG.*
	@touch $@

clean:
	@$(eval GOAL=$(addprefix $(OBJ_DIR)/,$(filter $(DEFINES_TMP),$(MAKECMDGOALS))))
	rm -rf $(if $(GOAL),$(GOAL),$(OBJ_DIR))

fclean: clean
	@$(eval GOAL=$(filter $(DEFINES_TMP),$(MAKECMDGOALS)))
	rm -rf $(if $(GOAL),$(GOAL),$(DEFINES_TMP))

ifneq ($(filter 1,$(DBG_F) $(ASAN_F)),1)    # If none of the debugging rules are
re: fclean                                  # called, so we simply fclean and
	@$(call subfilter, DBG=0)               # make again.
else                                        # Else,
re:                                         # we simply ignore the rule, because
	@:                                      # we already fclean when we are
endif                                       # calling the debugging rules.

.PHONY: all asan dbg clean fclean re FORCE
