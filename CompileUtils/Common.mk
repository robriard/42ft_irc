# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Common.mk                                          :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: riblanc <riblanc@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/24 16:50:26 by riblanc           #+#    #+#              #
#    Updated: 2022/04/05 12:23:26 by riblanc          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Define colors ################################################################

BOLD   = \e[01m
ITALIC = \e[03m
UNDER  = \e[04m
RED    = \e[31m
GREEN  = \e[32m
LGRAY  = \e[37m
GRAY   = \e[90m
RESET  = \e[0m
UP     = \e[A

KCLR   ::= $(ITALIC)$(LGRAY)
VCLR   ::= $(RESET)$(GRAY)

# Define functions #############################################################

show_upline       = $(shell printf "$(UP)")
show_color_filter = $(shell printf "$(1)$(UP)")
show_color        = $(info $(call show_color_filter,$(filter $($(1)),          \
$(BOLD)                                                                        \
$(ITALIC)                                                                      \
$(UNDER)                                                                       \
$(RED)                                                                         \
$(GREEN)                                                                       \
$(LGRAY)                                                                       \
$(GRAY)                                                                        \
$(RESET)                                                                       \
)))

spt            = $(shell printf "%$(2).$(2)s"  $(1))
spf            = $(shell printf "%-$(2).$(2)s" $(1))
show_info      = $(info $(1))
show_variable  = $(info - $(shell echo "$(KCLR)$(call spt,$(1),8):"            \
				 "$(call spf,[$(call get_attr,$(1))],10)$(VCLR) $($1)"         \
	             "$(RESET)"))
get_attr       = $($(addprefix $(1),_ATTR))
show_separator = $(call show_color,RESET)                                      \
                 $(call show_color,BOLD)                                       \
                 $(call show_info,-------------------------)                   \
                 $(call show_color,RESET)
show_title     = $(call show_separator)                                        \
                 $(call show_color,BOLD)$(call show_color,ITALIC)              \
                 $(call show_info,$(1))	                                       \
                 $(call show_color,RESET)

# Display configuration ########################################################

print_config   = $(call show_title,Makefile Configuration:)                    \
                 $(call show_variable,NAME)                                    \
                 $(call show_variable,SRC_DIR)                                 \
                 $(call show_variable,SRCS)                                    \
                 $(call show_variable,OBJ_DIR)                                 \
                 $(call show_variable,DEPS)                                    \
                 $(call show_variable,CC)                                      \
                 $(call show_variable,INCLUDES)                                \
                 $(call show_variable,CFLAGS)                                  \
                 $(call show_variable,GFLAGS)                                  \
                 $(call show_variable,RUN_ARGS)                                \
                 $(call show_title,$(NAME) compilation:)

# Submakes #####################################################################
submake=$(MAKE) $1 $(ND);

ifeq ($(COMPILE_ALL),true)
subfilter= $(eval tmp=$(filter $(filter-out $1,$(MAKECMDGOALS)), \
			$(DEFINES_TMP)))\
			$(if $(tmp),$(foreach rule,$(tmp),\
			$(call submake,NAME=$(rule) DEFINES_TMP=$(rule) $2 DEFINES=)), \
			$(foreach rule,$(DEFINES_TMP), \
			$(call submake,NAME=$(rule) $2 DEFINES=)))
else
subfilter= $(eval tmp=$(filter $(filter-out $1,$(MAKECMDGOALS)), \
			$(DEFINES_TMP)))\
			$(if $(tmp),$(foreach rule,$(tmp),\
			$(call submake,NAME=$(rule) DEFINES_TMP=$(rule) $2 DEFINES=)), \
			$(call submake,$2 DEFINES=))
endif

UC = $(shell echo '$1' | tr '[:lower:]' '[:upper:]')
