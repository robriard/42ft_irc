/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.class.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: unknow <unknow@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/13 14:36:22 by sgaubert          #+#    #+#             */
/*   Updated: 2022/04/06 14:37:06 by unknow           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "User.class.hpp"
# include "Console.class.hpp"
# include "Error.hpp"
# include "Response.hpp"

# include <string>
# include <map>
# include <vector>
# include <list>
# include <algorithm>

using namespace std;

class Channel {

	private:

		typedef map<User*, char>	Users;

		string 			_name;
		string			_topic;
		string			_mode;
		Users			_users;
		vector<User *>	_invited;

		User *findByName(string const &name) {
			for (Users::iterator it = _users.begin(); it != _users.end(); it++)
				if (it->first->getNickname() == name)
					return it->first;
			return NULL;
		}

		void checkChannop() {
			bool noChannop = true;
			for (Users::iterator it = _users.begin(); it != _users.end(); it++)
				if (it->second != '\0')
					noChannop = false;
			if (noChannop && !_users.empty())
				_users.begin()->second = 'o';
		}

	public:

		Channel(string name, User *creator) : _name(name), _topic(""), _mode(""), _users() {
			if (_name.size() > 200 || _name.find_first_of("&#") != 0 || _name.find_first_of(" \a,") != string::npos)
				throw ChannelException("Channel() Bad channel name");
			_users[creator] = 'O';
		}

		~Channel() {}

		string const &name() { return _name; };
		size_t userNb() { return _users.size(); };

		bool isOnChannel(User *user) const { return _users.find(user) != _users.end(); }
		bool isCreator(User *user) const { return isOnChannel(user) and _users.find(user)->second == 'O'; }
		bool isChannop(User *user) const { return isOnChannel(user) and _users.find(user)->second != '\0'; }

		void join(User *user) {
			if (_mode.find('i') != string::npos && find(_invited.begin(), _invited.end(), user) == _invited.end()) {
				user->sendPacket(to_str+ERR_INVITEONLYCHAN+" "+_name+" :Cannot join channel (+i)\r\n");
				user->sendPacket(to_str+":"+user->getId()+" PART "+_name+"\r\n");
			}
			else {
				_users.insert(make_pair(user, '\0'));
				sendPacket(NULL, to_str+":"+user->getId()+" JOIN "+_name+"\r\n");
				if (_topic.empty())
					user->sendPacket(to_str+RPL_NOTOPIC+" "+user->getNickname()+" "+_name+" :No topic is set\r\n");
				else
					user->sendPacket(to_str+RPL_TOPIC+" "+user->getNickname()+" "+_name+" :"+_topic+"\r\n");
				user->sendPacket(to_str+RPL_NAMREPLY+" "+user->getNickname()+" = "+_name+" :"+names()+"\r\n");
				user->sendPacket(to_str+RPL_ENDOFNAMES+" "+user->getNickname()+" "+_name+" :End of NAMES list\r\n");
			}
		}

		void part(User *user, string const &msg) {
			if (!isOnChannel(user))
				user->sendPacket(to_str+ERR_NOTONCHANNEL+" "+_name+" :You're not on that channel\r\n");
			else {
				sendPacket(NULL, ":"+user->getId()+" PART "+_name+" :"+msg+"\r\n");
				_users.erase(user);
				checkChannop();
			}
		}

		void quit(User *user, string const &msg) {
			if (_users.erase(user) != 0) {
				sendPacket(NULL, ":"+user->getId()+" PART "+_name+" :"+msg+"\r\n");
				checkChannop();
			}
		}

		void invite(User *user, User *target, bool chanAlreadyCreated) {
			if (chanAlreadyCreated && !isOnChannel(user))
				user->sendPacket(to_str+ERR_NOTONCHANNEL+" "+_name+" :You're not on that channel\r\n");
			else if (_mode.find('i') != string::npos && !isChannop(user))
				user->sendPacket(to_str+ERR_CHANOPRIVSNEEDED+" "+_name+" :You're not channel operator\r\n");
			else if (isOnChannel(target))
				user->sendPacket(to_str+ERR_USERONCHANNEL+" "+target->getNickname()+" "+_name+" :is already on channel\r\n");
			else {
				target->sendPacket(to_str+": "+user->getId()+" INVITE "+target->getNickname()+" "+_name+"\r\n");
				user->sendPacket(to_str+RPL_INVITING+" "+_name+" "+target->getNickname()+"\r\n");
				_invited.push_back(target);
			}
		}

		void kick(User *user, string const &userName, string const &msg) {
			User *target = findByName(userName);
			if (!isOnChannel(user))
				user->sendPacket(to_str+ERR_NOTONCHANNEL+" "+_name+" :You're not on that channel\r\n");
			else if (!isChannop(user))
				user->sendPacket(to_str+ERR_CHANOPRIVSNEEDED+" "+_name+" :You're not channel operator\r\n");
			else if (!isOnChannel(target))
				user->sendPacket(to_str+ERR_USERNOTINCHANNEL+" "+userName+" "+_name+" :They aren't on that channel\r\n");
			else {
				sendPacket(NULL, to_str+":"+user->getId()+" KICK "+_name+" "+userName+" :"+msg+"\r\n");
				_users.erase(target);
			}
		}

		void setMode(User *user, vector<string> & args) {
			if (!isChannop(user)) {
				user->sendPacket(to_str+ERR_CHANOPRIVSNEEDED+" "+_name+" :You're not channel operator\r\n");
				return ;
			}
			char flag = '+';
			list<string> todo;
			list<string> modeArgs;
			for (size_t i=0; i<args.size(); i++) {
				if (args[i][0] == '+' || args[i][0] == '-') {
					for (size_t j=0; j<args[i].size(); j++) {
						char c = args[i][j];
						if (c == '+' || c == '-')
							flag = c;
						else if (flag == '+') {
							if (c == 'i' && _mode.find(c) == string::npos)
								_mode += c;
							else
								todo.push_back(to_str+flag+c);
						}
						else if (flag == '-') {
							if (c == 'i' && _mode.find(c) != string::npos)
								_mode.erase(_mode.find(c), 1);
							else
								todo.push_back(to_str+flag+c);
						}
					}
				}
				else if (args[i] != "MODE" && args[i] != _name)
					modeArgs.push_back(args[i]);
			}
			list<string>::iterator mode_it = todo.begin();
			list<string>::iterator arg_it = modeArgs.begin();
			for (; mode_it!=todo.end() && arg_it!=modeArgs.end(); mode_it++, arg_it++) {
				if (mode_it->at(1) == 'o') {
#ifdef _DEBUG
					console.debug(to_str+(*arg_it));
#endif
					User *target = findByName(*arg_it);
					if (target == NULL || _users[target] == 'O')
						continue;
					if (mode_it->at(0) == '+')
						_users[target] = 'o';
					else if (mode_it->at(0) == '-')
						_users[target] = '\0';
				}
			}
		}

		void setTopic(User *user, string const & newTopic) {
			if (isChannop(user))
				_topic = newTopic;
		}

		map<User*, char>	getUsers(void) const {return _users;};

		string getTopic() { return _topic; }
		string getMode() { return _mode; }

		string names() {
			string names;
			for (Users::iterator it = _users.begin(); it!=_users.end(); it++)
				names += (it->second == 'O' ? "@" : "") + it->first->getNickname() + " ";
			return names.substr(0, names.size() - 1);
		}

		void sendPacket(User *user, string packet) {
			if (user != NULL && _users.count(user) == 0)
				return;
			for (Users::iterator it = _users.begin(); it != _users.end(); it++)
				if (it->first != user)
					it->first->sendPacket(packet);
		}

		/*
		 *	Execptions
		 */
		class ChannelException : public std::exception {
				string _msg;
			public:
				ChannelException(string const &msg) : _msg("Channel::" + msg) {}
				virtual const char *what() const throw() { return _msg.c_str(); }
				virtual ~ChannelException() _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {return;};
		};

};

#endif
