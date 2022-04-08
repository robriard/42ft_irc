/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Console.class.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: unknow <unknow@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/04 10:54:32 by unknow            #+#    #+#             */
/*   Updated: 2022/04/05 12:22:24 by riblanc          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSOLE_CLASS_HPP
# define CONSOLE_CLASS_HPP

# include "string.operator.hpp"

# include <cstdlib>
# include <ctime>
# include <iomanip>
# include <map>
# include <algorithm>

# define RETURN_SUCCESS		0
# define RETURN_FAILURE		1

# define to_str	string("")
# define arraySize(x, type) sizeof(x) / sizeof(type)
# define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

# define std_in		0
# define std_out	1
# define std_err	2

# define RESET		0
# define BOLD		(1 << 1)
# define DIM		(1 << 2)
# define ITALIC		(1 << 3)
# define UNDER		(1 << 4)
# define HIDDEN		(1 << 5)
# define BLACK		(1 << 6)
# define RED		(1 << 7)
# define GREEN		(1 << 8)
# define ORANGE		(1 << 9)
# define BLUE		(1 << 10)
# define PURPLE     (1 << 11)
# define CYAN		(1 << 12)
# define GRAY		(1 << 13)
# define NOENDL		(1 << 30)

static const char *g_format[14] = {
			"\x1b[0m",	//	RESET
			"\x1b[1m",	//	BOLD
			"\x1b[2m",	//	DIM
			"\x1b[3m",	//	ITALIC
			"\x1b[4m",	//	UNDER
			"\x1b[8m",	//	HIDDEN
			"\x1b[30m",	//	BLACK
			"\x1b[31m",	//	RED
			"\x1b[32m",	//	GREEN
			"\x1b[33m",	//	ORANGE
			"\x1b[34m",	//	BLUE
			"\x1b[35m",	//	PURPLE
			"\x1b[36m",	//	CYAN
			"\x1b[37m"	//	GRAY
};

template <class T = char *>
class Console {
	private:
		bool canLog;

    public:
		Console(void) : canLog(true), _GRPLVL(0) {return;};
		~Console(void) {this->_timers.erase(this->_timers.begin(), this->_timers.end());};

        void		log(T msg , int flags = 0) {
			if (!this->canLog)
				return ;
			if (msg.size()) this->_addGroups();
			this->_printTimestamp();
			this->_addFlags(flags);
			cout << msg;
			cerr << g_format[0];
			if (~flags & NOENDL)
				cout << endl;
		};

		void		error(T msg, int flags = 0) {
			if (!this->canLog)
				return ;
			if (msg.size()) this->_addGroups(std_err);
			this->_printTimestamp(std_err);
			this->_addFlags(flags, std_err);
			cerr << msg << g_format[0];;
			if (~flags & NOENDL)
				cerr << endl;
		};


		int			printError(string err, string prefix = string(""))
		{
			if (prefix.length() > 0)
				cerr << prefix << ": ";
			cerr << "\x1b[31;1merror:\x1b[0;1m " << err << "\x1b[0m" << endl;
			return (1);
		}

		int			printConfigError(string buffLine, int line, int col,
						string err, string filename = string(""))
		{
			stringstream prefix;

			prefix	<< "\x1b[1m"
					<< filename << (filename.size() > 0 ? ":" : "")
					<< line << ":"
					<< col;
			printError(err, prefix.str());
			replace(buffLine.begin(), buffLine.end(), '\t', ' ');
			cerr << string(8, ' ') << buffLine << endl;
			cerr << string(col + 7, ' ');
			cerr << "\x1b[32;1m^\x1b[0m   " << endl;
			this->canLog = false;
			return (1);
		}

		void		breakError(T msg, int flags = 0) {
			if (msg.size()) this->_addGroups(std_err);
			this->_printTimestamp(std_err);
			this->_addFlags(flags, std_err);
			cerr << msg << g_format[0];
			if (~flags & NOENDL)
				cerr << endl;
			exit (EXIT_FAILURE);
		}

		void		debug(T msg, int flags = 0) const {
			if (!this->canLog)
				return ;
			if (msg.size()) this->_addGroups();
			this->_addFlags(flags);
			cout << msg;
			cerr << g_format[0];
			if (~flags & NOENDL)
				cout << endl;
		};

		void	jumpLine(int nb = 1) {
			if (!this->canLog)
				return ;
			for (int i = 0; i < nb; i++)
				cout << endl;
		};

		void		assert(bool cond, T msg, int flags = 0) {if (not cond) this->log(msg, flags);};
		string		input(int flags = 0) {
			if (!this->canLog)
				return string();
			this->_addGroups();
			this->_addFlags(flags);
			string	input;
			getline(cin, input);
			cerr << g_format[0];
			return (input);
		}

		time_t		timestamp(void) {return (std::time(NULL));};
		void		time(T label) {
			if (this->_timers.find(label) != this->_timers.end())
				this->_timers.erase(label);
			this->_timers.insert(make_pair(label, this->timestamp()));
		};
		void		timeEnd(T label) {this->_timers.erase(label);};
		double		timeShow(T label) {
			for (typename map<T, time_t>::iterator it = this->_timers.begin(); it != this->_timers.end(); ++it) {
				if (it->first == label)
					return std::difftime(std::time(NULL), it->second);
			}
			return (-1);
		};

		void		clear(void) {cout << "\033[2J\033[1;1H";};

		void		group(void) {this->_GRPLVL++;};
		bool		groupEnd(void) {if (this->_GRPLVL == 0) {return false;} this->_GRPLVL--; return true;};

	private:
		size_t			_GRPLVL;
		map<T, time_t>	_timers;

		void		_addGroups(int fd = std_out)const {
			for (size_t i = 0;i < this->_GRPLVL; i++)
				if (fd == std_out)					cout << '\t';
				else
					cerr << '\t';
		};
		void		_addFlags(int flags, int fd __attribute__((unused))= std_out)const {
			for (size_t i = 0; i < arraySize(g_format, char*); i++)
					if (flags & (1 << i)) {
						cerr << g_format[i];
					}
		};
		void		_printTimestamp(int fd = std_out){
			time_t tmp = this->timestamp();
			tm* now = std::localtime(&tmp);
			if (fd == std_out)				cerr << setfill('0') << "[" << setw(2) << now->tm_mday << '-'
					<< setw(2) << now->tm_mon + 1 << "-"
					<< setw(2) << now->tm_year + 1900 << ' '
					<< setw(2) << now->tm_hour << ":"
					<< setw(2) << now->tm_min << ":"
					<< setw(2) << now->tm_sec << "] ";
			else
				cout << setfill('0') << "[" << setw(2) << now->tm_mday << '-'
					<< setw(2) << now->tm_mon + 1 << "-"
					<< setw(2) << now->tm_year + 1900 << ' '
					<< setw(2) << now->tm_hour << ":"
					<< setw(2) << now->tm_min << ":"
					<< setw(2) << now->tm_sec << "] ";
		}
};

Console<string>		console;
#endif

/*
 *				CONSOLE
 *	#include "console.class.hpp"
 *
 *
 *
 *				DESCRIPTION
 *	console() est une classe global de gestion du terminal, il n'y a besoin de l'instancier,
 *	le parametre flags peut prendre l'un des modes suivants:
 *
 *		BOLD
 *			imprime le texte en surbrillance
 *		DIM
 *			imprime le texte par clignotemant
 *		ITALIC
 *			imprime le texte en italique
 *		UNDER
 *			imprime le texte souligne
 *		HIDDEN
 *			masque le texte
 *		BLACK
 *			imprime le texte en noir
 *		RED
 *			imprime le texte en rouge
 *		GREEN
 *			imprime le texte en vert
 *		ORANGE
 *			imprime le texte en orange
 *		BLUE
 *			imprime le texte en bleu
 *		PURPLE
 *			imprime le texte en violet
 *		CYAN
 *			imprime le texte en cyan
 *		WHITE
 *			imprime le texte en blanc
 *		NOENDL
 *			ne pas imprimer de retour a la ligne
 *
 *	la macro to_str permet de convertir un type numerique, boolean ou char en string
 *
 *
 *
 *				FONCTION MEMBRES
 *		log -							void		log(T msg , int flags = 0);
 *	imprime le message passe sur la sortie standard dans msg precede par un timestamp;
 *
 *		error -							void	error(T msg, int flags = 0) const;
 *	imprime le message passe sur la sortie error dans msg precede par un timestamp;
 *
 *		breakError -					void		breakError(T msg, int flags = 0) const;
 *	imprime le message passe sur la sortie error dans msg precede par un timestamp et arrete le programme;
 *
 *		debug -							void		debug(T msg, int flags = 0) const;
 *	imprime le message passe sur la sortie standard dans msg non precede par un timestamp;
 *
 *		jumpLine -						void		jumpLine(int nb = 1);
 *	imprime nb lignes vides sur la sortie standard;
 *
 *		assert -						void		assert(bool cond, T msg, int flags = 0);
 *	imprime le message passe sur la sortie standard dans msg precede par un timestamp seulement si la condition cond n'est pas verifie;
 *
 *		input -							string		input(int flags = 0);
 *	demande une chaine de caractere a l'utilisateur et la retourne;
 *
 *		timestamp -						time_t		timestamp(void);
 *	retourne un time_t	correspondant au nombre de seconde ecoule depuis le 1er janvier 1970;
 *
 *		time -							void		time(T label);
 *	lance un chronometre nomme par la string label;
 *
 *		timeEnd -						void		timeEnd(T label);
 *	arrete le chronometre label
 *
 *		timeShow -						double		timeShow(T label);
 *	retourne la difference de seconde entre le lancement et l'appel d la fonction;
 *
 *		clear -							void		clear(void);
 *	efface l'integralite de la console;
 *
 *		group -							void		group(void);
 *	augmente de 1 le groupe d'indentation
 *
 *		groupEnd -						void		groupEnd(void);
 *	reduit de 1 le groupe d'indentation
 */
