/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: riblanc <riblanc@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/04 21:01:37 by riblanc           #+#    #+#             */
/*   Updated: 2022/04/05 03:03:49 by riblanc          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP
# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <utility>
# include "Lexer.hpp"

using namespace std;

class Parser
{
	private:
		Lexer &_lexer;
		vector<string> _tokens;
		std::map<string, string> *kvp;

	public:
		Parser(Lexer &lexer) : _lexer(lexer), kvp(new map<string, string>)
		{
#ifdef _DEBUG
			cout << "####### Parser #######" << endl;
			for (Lexer::iterator it = _lexer.begin(); it != _lexer.end(); ++it)
			{
				for (vector<Token>::iterator it2 = it->begin();
						it2 != it->end(); ++it2)
				{
					cout << it2->pos[0] << ":" << it2->pos[1] << " " << it2->type
					<< " " << it2->value << endl;
				}
				cout << endl;
			}
			cout << "#######################" << endl;
#endif
		}

		~Parser()
		{
			delete kvp;
		}

		map<string, string> &parse(void)
		{
			for (Lexer::iterator it = _lexer.begin(); it != _lexer.end(); ++it)
			{
				checkInvalidToken(it);
				vector< pair<string, string> > kvps = parseLine(it);
				for (vector< pair<string, string> >::iterator it2 = kvps.begin();
						it2 != kvps.end(); ++it2)
					kvp->insert(*it2);
			}
			return *kvp;
		}

		vector< pair<string, string> > parseLine(Lexer::iterator it)
		{
			int nbWords = 0;
			bool assign = false;
			vector< pair<string, string> > kvps;
			pair<string, string> kv;

			vector<Token>::iterator it2 = it->begin();
			for (; it2 != it->end(); ++it2)
			{
				if (it2->type == TOKEN_TYPE_KEYWORD)
				{
					if (nbWords == 0)
					{
						if (kvp->find(it2->value) == kvp->end())
							kv.first = it2->value;
						else
							goto already_defined;
					}
					else if (nbWords == 1)
					{
						if (assign == 1)
						{
							kv.second = it2->value;
							kvps.push_back(kv);
						}
						else
							goto invalid_key;
					}
					else if (nbWords > 1)
						goto invalid_key;
					nbWords++;
				}
				else if (it2->type == TOKEN_TYPE_ASSIGNMENT)
				{
					if (assign || nbWords == 0)
						goto invalid_assign;
					assign = true;
				}
				else if (it2->type == TOKEN_TYPE_SEPARATOR)
				{
					if (nbWords != 2 || !assign)
					{
						if (nbWords == 0 && !assign)
							continue ;
						else if (nbWords == 1 && assign)
						{
							kv.second = "";
							kvps.push_back(kv);
							continue ;
						}
						--it2;
						if (it2->type == TOKEN_TYPE_ASSIGNMENT)
							goto invalid_assign;
						else if (it2->type != TOKEN_TYPE_SEPARATOR)
							goto invalid_key;
					}
					nbWords = 0, assign = false;
				}
				continue ;
invalid_key:
				throw Token::InvalidTokenException
				(
					_lexer.getFilename(),
					it->getLine(),
					it2->pos,
					it2->value,
					TOKEN_TYPE_KEYWORD
				);
invalid_assign:
				throw Token::InvalidTokenException
				(
					_lexer.getFilename(),
					it->getLine(),
					it2->pos,
					it2->value,
					TOKEN_TYPE_ASSIGNMENT
				);
already_defined:
				throw Token::AlreadyDefinedException
				(
					_lexer.getFilename(),
					it->getLine(),
					it2->pos,
					it2->value
				);
			}
			if (nbWords != 2 || !assign)
			{
				if (nbWords == 1 && assign)
				{
					kv.second = "";
					kvps.push_back(kv);
					return kvps;
				}
				--it2;
				if (it2->type == TOKEN_TYPE_ASSIGNMENT)
					goto invalid_assign;
				else if (it2->type != TOKEN_TYPE_SEPARATOR)
					goto invalid_key;
			}
			return kvps;
		}

		void checkInvalidToken(Lexer::iterator &it)
		{
			for (vector<Token>::iterator it2 = it->begin(); it2 != it->end(); ++it2)
			{
				if (it2->type == TOKEN_TYPE_NONE)
					throw Token::InvalidTokenException
					(
						_lexer.getFilename(),
						it->getLine(),
						it2->pos,
						it2->value,
						it2->type
					);
			}
		}
};

#endif
