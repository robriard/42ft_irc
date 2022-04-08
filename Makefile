# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: riblanc <riblanc@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/04/03 12:05:55 by riblanc           #+#    #+#              #
#    Updated: 2022/04/05 12:10:33 by riblanc          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

#--- DEFINES
#   Will define a compilation rule for each element in the defines variable,
#   compiling a binary and defining a preprocessing macro with the element name.
#   i.e:
#   If DEFINES = hello world,
#   when 'make hello', it will compile a binary named hello with -D _HELLO
#   and when 'make world', it will compile a binary named world with -D _WORLD.
#
DEFINES = debug


#--- COMPILE_ALL
#   When setting COMPILE_ALL to 'true', 'make' will compile $(NAME) and each
#   elements defined in $(DEFINES). Otherwise, 'make' will only compile $(NAME).
#
#
COMPILE_ALL = false


CC       = clang++
SRC_LIST = ft_irc.cpp
CFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic

-include CompileUtils/Generic.mk
