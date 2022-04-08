/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_traits.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: unknow <unknow@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/10 11:13:34 by unknow            #+#    #+#             */
/*   Updated: 2022/04/05 12:26:48 by riblanc          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPE_TRAITS_HPP
# define TYPE_TRAITS_HPP

namespace ft {

/* ************************************************************************** */
/*            ENABLE_IF                                                       */
/* ************************************************************************** */
	template<bool Cond, class T = void>
	struct enable_if {};
	template<class T>
	struct enable_if<true, T> {typedef T type;};

/* ************************************************************************** */
/*            IS_INTEGRAL                                                     */
/* ************************************************************************** */
	template <class T, T v>
	struct integral_constant {
		static const T value = v;
		typedef T value_type;
		typedef integral_constant<T, v> type;
	};

	typedef integral_constant<bool, true>	integral_true_type;
	typedef integral_constant<bool, false>	integral_false_type;

	template <typename> struct is_integral_type :						integral_false_type {};
	template <> struct is_integral_type<short int> :					integral_true_type {};
	template <> struct is_integral_type<int> :							integral_true_type {};
	template <> struct is_integral_type<long int> :						integral_true_type {};
	template <> struct is_integral_type<unsigned short int> :			integral_true_type {};
	template <> struct is_integral_type<unsigned int> :					integral_true_type {};
	template <> struct is_integral_type<unsigned long int> :			integral_true_type {};
	template <> struct is_integral_type<const short int> :				integral_true_type {};
	template <> struct is_integral_type<const int> :					integral_true_type {};
	template <> struct is_integral_type<const unsigned short int> :		integral_true_type {};
	template <> struct is_integral_type<const unsigned int> :			integral_true_type {};
	template <> struct is_integral_type<const unsigned long int> :		integral_true_type {};

	template <class T> struct is_integral : is_integral_type<T> {};

/* ************************************************************************** */
/*            IS_DECIMAL                                                      */
/* ************************************************************************** */
	template <class T, T v>
	struct decimal_constant {
		static const T value = v;
		typedef T value_type;
		typedef decimal_constant<T, v> type;
	};

	typedef decimal_constant<bool, true>	decimal_true_type;
	typedef decimal_constant<bool, false>	decimal_false_type;

	template <typename> struct is_decimal_type :						decimal_false_type {};
	template <> struct is_decimal_type<double> :						decimal_true_type {};
	template <> struct is_decimal_type<float> :							decimal_true_type {};
	template <> struct is_decimal_type<const double> :					decimal_true_type {};
	template <> struct is_decimal_type<const float> :					decimal_true_type {};

	template <class T> struct is_decimal : is_decimal_type<T> {};

/* ************************************************************************** */
/*            IS_BOOLEAN                                                      */
/* ************************************************************************** */
	template <class T, T v>
	struct boolean_constant {
		static const T value = v;
		typedef T value_type;
		typedef boolean_constant<T, v> type;
	};

	typedef boolean_constant<bool, true>	boolean_true_type;
	typedef boolean_constant<bool, false>	boolean_false_type;

	template <typename> struct is_boolean_type :						boolean_false_type {};
	template <> struct is_boolean_type<bool> :							boolean_true_type {};
	template <> struct is_boolean_type<const bool> :					boolean_true_type {};

	template <class T> struct is_boolean : is_boolean_type<T> {};

}

#endif
