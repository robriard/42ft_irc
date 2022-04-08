/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: unknow <unknow@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/05 18:40:12 by simon             #+#    #+#             */
/*   Updated: 2022/04/05 02:50:19 by riblanc          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.class.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

int main(int argc, char const *argv[])
{
	if (argc != 3)
		console.breakError("Bad number of argument: " + string(*argv) + " <port> <password>");
	Server server;
	try {
		server.init(argv[1], argv[2]);
		server.displayConfig();
		server.run();
	} catch(Server::ServerException except) {
		console.breakError(except.what());
	}catch (Token::InvalidTokenException &e) {
		console.printConfigError(e.buffLine, e.line, e.column, e.getError(), e.filename);
		exit(EXIT_FAILURE);
	} catch (Token::AlreadyDefinedException &e) {
		console.printConfigError(e.buffLine, e.line, e.column, e.getError(), e.filename);
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}
