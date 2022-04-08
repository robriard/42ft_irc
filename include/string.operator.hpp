/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.operator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: unknow <unknow@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 10:34:23 by unknow            #+#    #+#             */
/*   Updated: 2022/04/05 12:26:39 by riblanc          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_OPERATOR_HPP
# define STRING_OPERATOR_HPP

# include "type_traits.hpp"
# include <iostream>
# include <sstream>

using namespace std;
# define sstream stringstream

template <typename T>
string    itoa(typename ft::enable_if<ft::is_integral<T>::value, T>::type nb) {
    sstream out;
    out << nb;
    return (out.str());
}

template <typename T>
string    ftoa(typename ft::enable_if<ft::is_decimal<T>::value, T>::type nb) {
    sstream out;
    out << nb;
    return (out.str());
}


/* ********** BOOLEAN ********** */
string	operator+(string lhs, bool rhs) {return (rhs ? (lhs+"true") : (lhs+"false"));}
string	operator+(bool lhs, string rhs) {return (lhs ? ("true"+rhs) : ("false"+rhs));}

/* ********** SHORT INT ********** */
string	operator+(string lhs, short int rhs) {return lhs+itoa<short int>(rhs);}
string	operator+(short int lhs, string rhs) {return itoa<short int>(lhs)+rhs;}
string	operator+(string lhs, unsigned short int rhs) {return lhs+itoa<unsigned short int>(rhs);}
string	operator+(unsigned short int lhs, string rhs) {return itoa<unsigned short int>(lhs)+rhs;}

/* ********** INT ********** */
string	operator+(string lhs, int rhs) {return lhs+itoa<int>(rhs);}
string	operator+(int lhs, string rhs) {return itoa<int>(lhs)+rhs;}
string	operator+(string lhs, unsigned int rhs) {return lhs+itoa<unsigned int>(rhs);}
string	operator+(unsigned int lhs, string rhs) {return itoa<unsigned int>(lhs)+rhs;}

/* ********** LONG INT ********** */
string	operator+(string lhs, long int rhs) {return lhs+itoa<long int>(rhs);}
string	operator+(long int lhs, string rhs) {return itoa<long int>(lhs)+rhs;}
string	operator+(string lhs, unsigned long int rhs) {return lhs+itoa<unsigned long int>(rhs);}
string	operator+(unsigned long int lhs, string rhs) {return itoa<unsigned long int>(lhs)+rhs;}

/* ********** DOUBLE ********** */
string	operator+(string lhs, double rhs) {return lhs+ftoa<double>(rhs);}
string	operator+(double lhs, string rhs) {return ftoa<double>(lhs)+rhs;}

#endif
