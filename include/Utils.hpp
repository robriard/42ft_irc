/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: unknow <unknow@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/28 23:16:58 by riblanc           #+#    #+#             */
/*   Updated: 2022/04/05 16:04:43 by riblanc          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP
# include <iostream>
# include <string>
# include <sstream>
# include <cctype>
# include <cstdlib>

using namespace std;

string& rightTrim(string& str)
{
	if (str.size() == 0)
		return str;
	size_t n = str.size() - 1;
	while (n != 0 && (!isprint(str[n]) || str[n] == 0x20))
		--n;
    str.erase(n + 1);
    return str;
}

string& leftTrim(string& str)
{
	size_t n = 0;
	while (n < str.size() && (!isprint(str[n]) || str[n] == 0x20))
		++n;
    str.erase(0, n);
    return str;
}

string &trim(string &str)
{
	leftTrim(str);
	rightTrim(str);
	return str;
}

vector<string> tokenize(string const &str, const char* delim) {
	vector<string> tmp;
    char *token = strtok(const_cast<char*>(str.c_str()), delim);
    while (token != NULL)
    {
        tmp.push_back(std::string(token));
        token = strtok(NULL, delim);
    }
	return tmp;
}

int isNumber(string str) {
	for (size_t i = 0; i < str.size(); i++) {
		if (!isdigit(str[i]))
			return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int checkIntegral(string str)
{
	char *c;
	double ret;

	ret = static_cast<int>(strtod(str.c_str(), &c));
	if (*c != '\0' || isNumber(str) == EXIT_FAILURE)
		return (-1);
	return ret;
}

#endif
