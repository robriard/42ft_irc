/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: unknow <unknow@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/04 19:48:39 by riblanc           #+#    #+#             */
/*   Updated: 2022/04/05 16:55:40 by unknow           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
# define LEXER_HPP
# include <iostream>
# include <string>
# include <fstream>
# include <vector>
# include <sstream>
# include <algorithm>

using namespace std;

enum e_token_type
{
	TOKEN_TYPE_NONE,
	TOKEN_TYPE_KEYWORD,
	TOKEN_TYPE_SEPARATOR,
	TOKEN_TYPE_ASSIGNMENT
};

class Token
{
	public:
		e_token_type	type;
		string			value;
		int				pos[2];

		Token(e_token_type type, string value, int line, int column)
			: type(type), value(value)
		{
			pos[0] = line;
			pos[1] = column;
		}

		~Token() {}

		class InvalidTokenException : public exception
		{
			public:
				string filename;
				string buffLine;
				int line;
				int column;
				string token;
				e_token_type type;

				InvalidTokenException(string fname, string buff, int _pos[2], string val, e_token_type _type) :
					filename(fname),
					buffLine(buff),
					line(_pos[0]),
					column(_pos[1]),
					token(val),
					type(_type)
				{}

				virtual ~InvalidTokenException() throw() {}

				virtual string getError() const throw()
				{
					stringstream ss;
					ss << "Invalid ";
					if (type == TOKEN_TYPE_NONE)
						ss << "token '" << token << "'";
					else if (type == TOKEN_TYPE_KEYWORD)
						ss << "keyword '" << token << "'";
					else if (type == TOKEN_TYPE_SEPARATOR)
						ss << "separator '" << token << "'";
					else if (type == TOKEN_TYPE_ASSIGNMENT)
						ss << "assignment";
					return ss.str();
				}
		};

		class AlreadyDefinedException : public exception
		{
			public:
				string filename;
				string buffLine;
				int line;
				int column;
				string token;

				AlreadyDefinedException(string fname, string buff, int _pos[2], string val) :
					filename(fname),
					buffLine(buff),
					line(_pos[0]),
					column(_pos[1]),
					token(val)
				{}

				virtual ~AlreadyDefinedException() throw() {}

				virtual string getError() const throw()
				{
					stringstream ss;
					ss << "Redefinition of variable '" << token << "'";
					return ss.str();
				}
		};
};

class Line
{
	public:
		typedef vector<Token>::iterator iterator;

	private:
		vector<Token>	_tokens;
		string			_line;

	public:
		Line(string &line) : _tokens(), _line(line) {}

		iterator begin()
		{
			return _tokens.begin();
		}

		iterator end()
		{
			return _tokens.end();
		}

		void push(Token token)
		{
			_tokens.push_back(token);
		}

		size_t size()
		{
			return _tokens.size();
		}

		string getLine()
		{
			return _line;
		}
};

class Lexer
{
	public:
		typedef vector<Line>::iterator iterator;

	private:
		vector<Line>	_lines;
		fstream			_file;
		string			_charset;
		string			_filename;

	public:
		Lexer(string filename) :
			_lines(),
			_charset("./_-"),
			_filename(filename)
		{
			_file.open(filename.c_str(), ios::in);
			if (!_file.is_open())
				throw "Error: _file not found";
		}

		vector<Line> lex()
		{
			int line = 1;

			for (; _file.eof() != true; line++)
			{
				string line_content;
				getline(_file, line_content);
				Line l(line_content);

				for (size_t col = 0; col < line_content.length(); col++)
				{
					if (line_content[col] == '#')
						break ;
					else if (line_content[col] == ';')
						l.push(Token(TOKEN_TYPE_SEPARATOR, ";", line, col + 1));
					else if (line_content[col] == '=')
						l.push(Token(TOKEN_TYPE_ASSIGNMENT, "=", line, col + 1));
					else if (isKeyWordChar(line_content[col]))
					{
						string tmp = getKeyWord(line_content, col);

						l.push
						(
							Token(TOKEN_TYPE_KEYWORD,
							tmp,
							line,
							col - tmp.length() + 2)
						);
					}
					else if (!isspace(line_content[col]))
					{
						string value = "";
						l.push
						(
							Token(TOKEN_TYPE_NONE,
							(value += line_content[col]),
							line,
							col + 1)
						);
					}
				}
				if (l.size() > 0)
					_lines.push_back(l);
			}
			return _lines;
		}

		bool isKeyWordChar(char c)
		{
			return (isalnum(c) || _charset.find(c) != string::npos);
		}

		string getKeyWord(string line, size_t &col)
		{
			string tmp;

			int i = 0;
			for (; col < line.length(); i++, col++)
			{
				if (isalnum(line[col]) || _charset.find(line[col]) != string::npos)
					tmp += line[col];
				else
				{
					--col;
					break;
				}
			}
			if (col == line.length())
				--col;
			return tmp;
		}

		~Lexer()
		{
			_file.close();
		}

		iterator begin()
		{
			return _lines.begin();
		}

		iterator end()
		{
			return _lines.end();
		}

		string &getFilename()
		{
			return this->_filename;
		}
};

#endif
