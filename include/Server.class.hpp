/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.class.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robriard <robriard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/05 18:40:05 by simon             #+#    #+#             */
/*   Updated: 2023/01/03 13:38:21 by robriard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CLASS_HPP
# define SERVER_CLASS_HPP

# include "User.class.hpp"
# include "Console.class.hpp"
# include "Channel.class.hpp"
# include "Utils.hpp"
# include "Lexer.hpp"
# include "Parser.hpp"

# include <sys/types.h>
# include <netinet/in.h>
# include <sys/select.h>
# include <sys/time.h>
# include <sys/types.h>
# include <poll.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

# include <cerrno>
# include <cstring>
# include <string>
# include <fstream>
# include <map>
# include <vector>

using namespace std;

class Server;

typedef int (Server::* t_cmd)(User*, vector<string>&);

class Server {
	private:
		/*
		 *	Attributes
		 */
		map<string, string>				_config;
		map<int, User*>					_users;
		vector<pollfd>					_pfds;
		map<string, Channel*>			_channels;
		map<string, t_cmd>				_commands;
		bool							_running;
		pair<string, vector<string> >	_lastCommand;
		time_t							_lastPing;


		/*
		 *	Member functions
		 */
		User *findByName(string const &name) {
			for (map<int, User*>::iterator it = _users.begin(); it != _users.end(); it++)
				if (it->second->getNickname() == name)
					return it->second;
			return NULL;
		};

		string getPastDoubleDot(vector<string> &args) {
			string msg;
			for (size_t i=0; i<args.size(); i++) {
				if (args[i][0] == ':') {
					for (; i<args.size(); i++)
						msg += args[i] + " ";
				}
			}
			return msg.empty() ? "" : msg.substr(1, msg.size() - 2);
		}

		int checkConfig(void) {
			fstream motd;
			vector<string> host;
			if (_config.size() == 0)
				throw ServerException("Config file is empty");
			if (_config.find("host") == _config.end())
				throw ServerException("No host specified in config file");
			host = tokenize(string(_config["host"]), ".");
			if (host.size() != 4 || isNumber(host[0]) || isNumber(host[1]) || isNumber(host[2]) || isNumber(host[3]))
				throw ServerException("Invalid host specified in config file");
			if (checkIntegral(_config["port"]) < 0)
				throw ServerException("Invalid port: \"" + _config["port"] + "\"");
			if (_config.find("ping") == _config.end())
				throw ServerException("No ping specified in config file");
			if (checkIntegral(_config["ping"]) < 0)
				throw ServerException("Invalid ping: \"" + _config["ping"] + "\"");
			if (_config.find("timeout") == _config.end())
				throw ServerException("No timeout specified in config file");
			if (checkIntegral(_config["timeout"]) < 0)
				throw ServerException("Invalid timeout: \"" + _config["timeout"] + "\"");
			if (checkIntegral(_config["ping"].c_str()) > checkIntegral(_config["timeout"].c_str()) / 2)
				throw ServerException("ping must be less than timeout / 2");
			if (_config.find("oper_user") == _config.end())
				throw ServerException("No oper_user specified in config file");
			if (_config.find("oper_password") == _config.end())
				throw ServerException("No oper_password specified in config file");
			if (_config.find("motd") == _config.end())
				throw ServerException("No motd specified in config file");
			motd.open(_config["motd"].c_str(), ios::in);
			if (!motd.is_open())
				throw ServerException("Can't open motd file");
			motd.close();
			if (_config.find("max") == _config.end())
				throw ServerException("No max specified in config file");
			if (checkIntegral(_config["max"]) <= 0)
				throw ServerException("max must be a positive integer");
			if (_config.find("backlog") == _config.end())
				throw ServerException("No backlog specified in config file");
			if (checkIntegral(_config["backlog"]) <= 0)
				throw ServerException("backlog must be a positive integer");
			return RETURN_SUCCESS;
		}

	public:
		/*
		 *	Constructor
		 */
		Server(void) {
#ifdef _DEBUG
			console.log("IRC server created", BOLD);
#endif
		};

		~Server() {
			map<int, User*>::iterator it1 = _users.begin();
			map<int, User*>::iterator ite1 = _users.end();
			for(; it1 != ite1; ++it1)
				delete (*it1).second;
			map<string, Channel*>::iterator it2 = _channels.begin();
			map<string, Channel*>::iterator ite2 = _channels.end();
			for(; it2 != ite2; ++it2)
				delete (*it2).second;
			_pfds.clear();
			_users.clear();
			_config.clear();
			_channels.clear();
			if (_pfds.size() > 0)
				close(_pfds[0].fd);
			console.log("Server successfully destroyed", BOLD);
		};

		void	init(const string port = "6667", const string password = "", const string& config = "./config/default.conf"){
			ifstream	ifs;
			string		buffer, key, value;
			Lexer lexer(config);
			lexer.lex();

			Parser parser(lexer);
			this->_config = parser.parse();
			if (password.size())
				set("port", port);
			set("password", password);
			checkConfig();
			console.log("start of irc server");
			loadCommands();
			int enable = 1;
			_pfds.push_back(pollfd());
			if ((_pfds[0].fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
				throw ServerException("socket() failed");
			if (setsockopt(_pfds[0].fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(enable)))
				throw ServerException("setsockopt() failed");
			if (fcntl(_pfds[0].fd, F_SETFL, O_NONBLOCK) < 0)
				throw ServerException("fcntl() failed");
			in_addr sin_addr;
			sin_addr.s_addr = inet_addr(_config["host"].c_str());
			struct sockaddr_in		addr;
			addr.sin_addr = sin_addr;
			addr.sin_family = AF_INET;
			addr.sin_port = htons(atoi(_config["port"].c_str()));
			if ((bind(_pfds[0].fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))) < 0)
				throw ServerException("bind() failed");
			if (listen(_pfds[0].fd, atoi(_config["backlog"].c_str())) < 0)
				throw ServerException("listen() failed");
			_pfds[0].events = POLLIN;
#ifdef _DEBUG
			console.log("IRC server successfully initialized", BOLD);
#endif
		};

		void	set(string key, string value) {_config[key] = value;};

		void	loadCommands(void) {
			_commands["PING"] = &Server::PING;
			_commands["PONG"] = &Server::PONG;
			_commands["QUIT"] = &Server::QUIT;
			_commands["JOIN"] = &Server::JOIN;
			_commands["PART"] = &Server::PART;
			_commands["INVITE"] = &Server::INVITE;
			_commands["KICK"] = &Server::KICK;
			_commands["MODE"] = &Server::MODE;
			_commands["TOPIC"] = &Server::TOPIC;
			_commands["NAMES"] = &Server::NAMES;
			_commands["PRIVMSG"] = &Server::PRIVMSG;
			_commands["NICK"] = &Server::NICK;
			_commands["USER"] = &Server::USER;
			_commands["PASS"] = &Server::PASS;
			_commands["MOTD"] = &Server::MOTD;
			_commands["CAP"] = &Server::CAP;
			_commands["AWAY"] = &Server::AWAY;
			_commands["WHOIS"] = &Server::WHOIS;
			_commands["WHO"] = &Server::WHO;
			_commands["WHOWAS"] = &Server::WHOWAS;
			_commands["NOTICE"] = &Server::NOTICE;
			_commands["die"] = &Server::DIE;
			_commands["OPER"] = &Server::OPER;
			_commands["LIST"] = &Server::LIST;
			_commands["kill"] = &Server::KILL;
		};

		/*
		 *	Member functions
		 */
		void	notACommand(int fd, string cmd) {
#ifdef _DEBUG
			console.debug(cmd, BOLD | PURPLE);
#endif
			string packet = "Command not found: "+cmd+"\r\n";
			send(fd, packet.c_str(), packet.size(), 0);
		};

		void	testConnection(void) {
			time_t current = std::time(0);
			int timeout = atoi(_config["timeout"].c_str());

			for (std::map<int, User *>::iterator it = _users.begin(); it != _users.end(); ++it)
				if (current - (*it).second->getLastPing() >= timeout) {
					delUser((*it).first);
					(*it).second->setStatus(DELETE);
				}
				else if ((*it).second->getStatus() == ONLINE)
					(*it).second->sendPacket("PING "+(*it).second->getNickname()+"\r\n");
		};

		void	run(void) {
			_running = true;
			_lastPing = std::time(0);

			while (_running) {
				console.debug("Wait for commands...", ITALIC);
				if (poll(&_pfds[0], _pfds.size(), atoi(_config["ping"].c_str()) * 1000) == -1)
					throw ServerException("poll() failed");
				if (time(0) - _lastPing > atoi(_config["ping"].c_str())) {
					testConnection();
					_lastPing = time(0);
				}
				if (_pfds[0].revents == POLLIN)
					addUser();
				else {
					for (vector<pollfd>::iterator it = _pfds.begin() + 1; it != _pfds.end(); ++it) {
						if (it->revents == POLLIN) {
							pair<int, vector<string> > packet = _users[it->fd]->receivePacket();
							if (packet.first == 0)
								delUser(it->fd);
							else {
								for (size_t i=0; i<packet.second.size(); i++) {
									vector<string> args = tokenize(packet.second[i], " \t");
									if (_users.find(it->fd) != _users.end() && _commands.find(args[0]) != _commands.end())
										(this->*_commands[args[0]])(_users[it->fd], args);
									else if (_users.find(it->fd) != _users.end())
										notACommand(it->fd, args[0]);
									_lastCommand.first = _users[it->fd]->getNickname();
									_lastCommand.second = args;
								}
							}
							if (_users[it->fd]->getStatus() == DELETE)
								delUser(it->fd);
						}
					}
					delUser();
				}
				delEmptyChannels();
#ifdef _DEBUG
				displayUsers();
				displayChannels();
#else
				displayInfo();
#endif
			}
		};

		void	delEmptyChannels(void) {
			for(map<string, Channel*>::iterator it = _channels.begin(); it != _channels.end();) {
				map<string, Channel*>::iterator tmp = it;
				tmp++;
				if (it->second->userNb() == 0) {
					delete (*it).second;
					_channels.erase(it);
				}
				it = tmp;
			}
		};

		/*
		 *	Users manager
		 */
		void	addUser(void) {
			unsigned long max = atoi(_config["max"].c_str());
			if (_users.size() == max) {
				console.error("too many user connected");
				return;
			}
			struct sockaddr_in addr;
			socklen_t csin_len = sizeof(addr);
			int newFd = accept(_pfds[0].fd, (struct sockaddr*)&addr, &csin_len);
			if (newFd == -1)
				throw ServerException("accept() failed");
			_users[newFd] = new User(newFd, addr);
			if (not _config["password"].size())
				_users[newFd]->setStatus(REGISTER);
			_pfds.push_back(pollfd());
			_pfds.back().fd = newFd;
			_pfds.back().events = POLLIN;
		};

		void	delUser(int fd = 0) {
			static vector<int> userToDel;
			User *user;

			if (fd > 0)
				userToDel.push_back(fd);
			else {
				for (size_t i=0; i<userToDel.size(); ++i) {
					user = _users[userToDel[i]];
#ifdef _DEBUG
					console.log("Delete user "+user->getUsername(), BOLD);
#endif
					delete user;
					_users.erase(userToDel[i]);
					for (vector<pollfd>::iterator it = _pfds.begin(); it != _pfds.end(); ++it) {
						if ((*it).fd == userToDel[i]) {
							vector<pollfd>::iterator tmp = it;
							it--;
							_pfds.erase(tmp);
						}
					}
				}
				userToDel.clear();
			}
		};

		/*
		 *	Geters
		 */
		void	displayConfig(void) {
			map<string, string>::iterator it = _config.begin();
			map<string, string>::iterator ite = _config.end();

			console.log("		DISPLAY CONFIG", BOLD | UNDER | ITALIC);
			console.debug("============================", BOLD);
			console.group();
			for(; it != ite; ++it) {
				console.debug("key: ["+it->first+"] <=> value: ["+it->second+"]", ITALIC);
			}
			console.groupEnd();
			console.debug("============================", BOLD);
		};

		void	displayUsers(void) {
			console.log("		DISPLAY USERS", BOLD | UNDER | ITALIC);
			console.debug("============================", BOLD);
			console.group();
			for (map<int, User *>::iterator it = _users.begin(); it != _users.end(); ++it) {
				console.debug("username: "+ it->second->getUsername() + " | nickname: " + it->second->getNickname() + " | fd: " + it->second->getFd() + " | status: " + it->second->getStatus(), ITALIC);
			}
			console.groupEnd();
			console.debug("============================", BOLD);
		};

		void	displayChannels(void) {
			console.log("		DISPLAY CHANNELS", BOLD | UNDER | ITALIC);
			console.debug("============================", BOLD);
			console.group();
			for (map<string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
				console.debug("name: "+ it->first + " | users: " + it->second->names(), ITALIC);
			console.groupEnd();
			console.debug("============================", BOLD);
		};

		void	displayInfo(void) {
			console.clear();
			for (uint i = 0; i < 3; i++)
				console.group();
			console.debug("IRC server running", BOLD | UNDER | ITALIC);
			for (uint i = 0; i < 3; i++)
				console.groupEnd();
			if (_lastCommand.first.size()) {
				console.jumpLine();
				console.log("LAST COMMAND RECEIVE: [ ", NOENDL);
				console.debug(_lastCommand.first, BOLD | PURPLE | NOENDL);
				console.debug(" ] : ", NOENDL);
				for (uint i = 0; i < _lastCommand.second.size(); i++) {
					console.debug(_lastCommand.second[i]+" ", NOENDL | BOLD | ORANGE);
				}
			}
			console.jumpLine(4);
			console.debug("USERS LIST:", BOLD | UNDER | ITALIC);
			console.debug("fd		nickname  		grade		status");
			for (map<int, User *>::iterator it1 = _users.begin(); it1 != _users.end(); ++it1) {
				console.debug(to_str + it1->second->getFd() + "		", CYAN | NOENDL);
				if (it1->second->getNickname().size() <= 10) {
					console.debug(to_str + it1->second->getNickname(), PURPLE | NOENDL);
					for (uint i = 0; i < 10 - it1->second->getNickname().size(); i++)
						console.debug(" ", NOENDL);
				} else
					for (uint i = 0; i < 10; i++)
						console.debug(to_str+it1->second->getNickname()[i], PURPLE | NOENDL);
				console.debug("		", NOENDL);
				if (it1->second->getGrade() == Operator)
					console.debug("Admin", ORANGE | NOENDL);
				else
					console.debug("Member", ORANGE | NOENDL);
				console.debug("		", NOENDL);
				if (it1->second->getStatus() == ONLINE && !it1->second->getAway())
					console.debug("ONLINE", GREEN);
				else if (it1->second->getStatus() == ONLINE && it1->second->getAway())
					console.debug("AWAY", ORANGE);
				else if (it1->second->getStatus() == REGISTER)
					console.debug("REGISTER", ORANGE);
				else if (it1->second->getStatus() == PASSWORD)
					console.debug("PASSWORD", ORANGE);
				else
					console.debug("DELETE", RED);
			}
			console.jumpLine(3);
			console.debug("CHANNELS LIST:", BOLD | UNDER | ITALIC);
			for (map<string, Channel*>::iterator it2 = _channels.begin(); it2 != _channels.end(); ++it2) {
				console.debug(it2->first+" <= ", NOENDL);
				map<User*, char> users = it2->second->getUsers();
				for (map<User*, char>::iterator it3 = users.begin(); it3 != users.end(); ++it3) {
					if (it2->second->isCreator(it3->first))
						console.debug(it3->first->getNickname()+" ", RED | NOENDL);
					else if (it2->second->isChannop(it3->first))
						console.debug(it3->first->getNickname()+" ", GREEN | NOENDL);
					else
						console.debug(it3->first->getNickname()+" ", GRAY | NOENDL);
				}
				console.jumpLine();
			}
			console.jumpLine(3);
		};

		/*
		 *	Commands
		 */
		int		PING(User* user, vector<string>& args __attribute__((unused))) {
			if (user->getStatus() != ONLINE)
				return EXIT_FAILURE;
			return user->sendPacket("PONG\r\n");
		}

		int		PONG(User* user, vector<string>& args __attribute__((unused))) {
			if (user->getStatus() != ONLINE)
				return EXIT_FAILURE;
			user->setLastPing(time(0));
			return EXIT_SUCCESS;
		}

		int		QUIT(User *user, vector<string> &args) {
			for (map<string, Channel*>::iterator chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt)
				chanIt->second->quit(user, getPastDoubleDot(args));
			delUser(user->getFd());
			return EXIT_SUCCESS;
		}

		int		JOIN(User *user, vector<string> &args) {
			if (user->getStatus() != ONLINE)
				return EXIT_FAILURE;
			if (args.size() < 2) {
				user->sendPacket(to_str+ERR_NEEDMOREPARAMS+" JOIN :Not enough parameters\r\n");
				return EXIT_FAILURE;
			}
			map<string, Channel*>::iterator it;
			vector<string> chanList = tokenize(args[1], ",");
			Channel *chan;

			if (chanList.size() == 1 && chanList[0] == "0") {
				for (it = _channels.begin(); it != _channels.end(); ++it)
					if (it->second->isOnChannel(user))
						it->second->part(user, getPastDoubleDot(args));
				return EXIT_SUCCESS;
			}
			
			for (size_t i=0; i<chanList.size(); i++) {
				if (chanList[i][0] != '#') {
					user->sendPacket(to_str+ERR_BADCHANNELKEY+" JOIN :"+chanList[i]+" :Bad channel key\r\n");
					continue;
				}
				it = _channels.find(chanList[i]);

				if (it == _channels.end()) {
					chan = new Channel(chanList[i], user);
					_channels[chanList[i]] = chan;
				}
				else
					chan = it->second;
				chan->join(user);
			}
			return EXIT_SUCCESS;
		}

		int		PART(User *user, vector<string> &args) {
			if (user->getStatus() != ONLINE)
				return EXIT_FAILURE;
			if (args.size() < 2)  {
				user->sendPacket(to_str+ERR_NEEDMOREPARAMS+"PART :Not enough parameters\r\n");
				return EXIT_FAILURE;
			}
			map<string, Channel*>::iterator it;
			vector<string> chanList = tokenize(args[1], ",");
			Channel *chan;

			for (size_t i=0; i<chanList.size(); i++) {
				it = _channels.find(chanList[i]);
				if (it != _channels.end()) {
					chan = it->second;
					chan->part(user, getPastDoubleDot(args));
				}
				else
					user->sendPacket(to_str+ERR_NOSUCHCHANNEL+" "+chanList[i]+" :No such channel\r\n");
			}
			return EXIT_SUCCESS;
		}

		int		INVITE(User *user, vector<string> &args) {
			if (user->getStatus() != ONLINE)
				return EXIT_FAILURE;
			if (args.size() < 3) {
				user->sendPacket(to_str+ERR_NEEDMOREPARAMS+"INVITE :Not enough parameters\r\n");
				return EXIT_FAILURE;
			}
			Channel	*chan = NULL;
			User	*target = findByName(args[1]);
			bool	chanAlreadyCreated = true;
			
			if (!target) {
				user->sendPacket(to_str+ERR_NOSUCHNICK+" "+args[1]+" :No such nick/channel\r\n");
				return ERR_NOSUCHNICK;
			}

			if (_channels.count(args[2]) != 0)
				chan = _channels[args[2]];
			else {
				chan = new Channel(args[2], user);
				_channels[args[2]] = chan;
				chanAlreadyCreated = false;
			}
			
			chan->invite(user, target, chanAlreadyCreated);
			return EXIT_SUCCESS;
		}

		int		KICK(User *user, vector<string> &args) {
			if (user->getStatus() != ONLINE)
				return EXIT_FAILURE;
			if (args.size() < 3) {
				user->sendPacket(to_str+ERR_NEEDMOREPARAMS+" KICK :Not enough parameters\r\n");
				return EXIT_FAILURE;
			}
			vector<string> chanList = tokenize(args[1], ",");
			string reason = getPastDoubleDot(args);
			for (size_t i=0; i < chanList.size(); i++) {
				if (_channels.count(chanList[i]) != 0)
					_channels[chanList[i]]->kick(user, args[2], reason);
				else
					user->sendPacket(to_str+ERR_NOSUCHCHANNEL+" "+chanList[i]+"\r\n");
			}
			return EXIT_SUCCESS;
		}

		int		MODE(User *user, vector<string> &args) {
			if (user->getStatus() != ONLINE)
				return EXIT_FAILURE;
			if (args.size() < 2) {
				user->sendPacket(to_str+ERR_NEEDMOREPARAMS+" MODE :Not enough parameters\r\n");
				return EXIT_FAILURE;
			}

			if (_channels.count(args[1])) {
				if (args.size() >= 3)
					_channels[args[1]]->setMode(user, args);
				user->sendPacket(":"+user->getId()+" "+RPL_CHANNELMODEIS+" "+user->getNickname()+" "+args[1]+" "+_channels[args[1]]->getMode()+"\r\n");
			}
			return EXIT_SUCCESS;
		}

		int		TOPIC(User *user, vector<string> &args) {
			if (user->getStatus() != ONLINE)
				return EXIT_FAILURE;
			Channel *chan = NULL;
			if (_channels.count(args[2]) != 0)
				chan = _channels[args[2]];

			if (chan) {
				if (args.size() >= 3)
					chan->setTopic(user, getPastDoubleDot(args));
				string topic = chan->getTopic();
				if (topic.empty())
					user->sendPacket(to_str+RPL_NOTOPIC+" "+user->getNickname()+" "+chan->name()+" :No topic is set\r\n");
				else
					user->sendPacket(to_str+RPL_TOPIC+" "+user->getNickname()+" "+chan->name()+" :"+topic+"\r\n");
			}
			return EXIT_SUCCESS;
		}

		int		NAMES(User *user, vector<string> &args) {
			if (user->getStatus() != ONLINE)
				return EXIT_FAILURE;
			map<string, Channel*>::iterator it;
			vector<string> chanList;
			
			if (args.size() == 1) {
				for (it = _channels.begin(); it != _channels.end(); it++)
					chanList.push_back(it->first);
			}
			else
				chanList = tokenize(args[1], ",");

			for (size_t i=0; i<chanList.size(); i++) {
				it = _channels.find(chanList[i]);

				if (it != _channels.end()) {
					user->sendPacket(to_str+RPL_NAMREPLY+" = "+it->second->name()+" :"+it->second->names()+"\r\n");
				}
			}
			user->sendPacket(to_str+RPL_ENDOFNAMES+" "+it->second->name()+" :End of NAMES list\r\n");
			return EXIT_SUCCESS;
		}

		int		PRIVMSG(User *user, vector<string> &args) {
			if (user->getStatus() != ONLINE)
				return EXIT_FAILURE;
			if (args.size() < 3) {
				user->sendPacket(to_str+ERR_NEEDMOREPARAMS+" PRIVMSG :Not enough parameters\r\n");
				return EXIT_FAILURE;
			}
			Channel *chanTarget = NULL;
			User *userTarget = NULL;
			string target = args[1];
			string msg = getPastDoubleDot(args);

			if (target.find_first_of("#&") == 0) {
				if (_channels.count(target))
					chanTarget = _channels[target];
			}
			else
				userTarget = findByName(target);

			if (chanTarget)
				chanTarget->sendPacket(user, to_str+":"+user->getId()+" PRIVMSG "+target+" "+msg+"\r\n");
			else if (userTarget)
				userTarget->sendPacket(to_str+":"+user->getId()+" PRIVMSG "+target+" "+msg+"\r\n");
			else
				user->sendPacket(to_str+ERR_NOSUCHNICK+" "+target+"\r\n");

			return EXIT_SUCCESS;
		}

		int		NOTICE(User *user, vector<string> &args) {
			if (user->getStatus() != ONLINE)
				return EXIT_FAILURE;
			if (args.size() < 3) {
				user->sendPacket(to_str+ERR_NEEDMOREPARAMS+" PRIVMSG :Not enough parameters\r\n");
				return EXIT_FAILURE;
			}
			string	message;
			User*	target = NULL;
			for (uint i=2; i < args.size(); i++)
				message += args[i] + " ";
			trim(message);
			if ((target = findByName(args[1])) != NULL)
				target->sendPacket(":"+user->getId()+" "+args[0]+" "+target->getNickname()+" :"+message+"\r\n");
			return EXIT_SUCCESS;
		}

		int		CAP(User *user __attribute__((unused)), vector<string> &args __attribute__((unused))) {
			return EXIT_SUCCESS;
		}

		int		NICK(User *user, vector<string> &args) {
			if (user->getStatus() != ONLINE and user->getStatus() != REGISTER)
				return EXIT_FAILURE;
			if (args.size() < 2) {
				user->sendPacket(to_str+ERR_NONICKNAMEGIVEN+" :No nickname given\r\n");
				return EXIT_FAILURE;
			}
			map<int, User *>::iterator nick = _users.find(user->getFd());
			while (true) {
				for (map<int, User *>::iterator it = _users.begin(); it != _users.end(); ++it) {
					if (args[1] == it->second->getNickname() && it != nick) {
						args[1]+="_";
						continue;
					}
				}
				break;
			}
			user->sendPacket(":"+user->getId()+" NICK :"+args[1]+"\r\n");
			user->setNickname(args[1]);
			return EXIT_SUCCESS;
		}

		int		USER(User *user, vector<string> &args) {
			if (user->getStatus() != REGISTER) {
				user->setStatus(DELETE);
				return EXIT_FAILURE;
			}
			string fullName = getPastDoubleDot(args);
			user->setUsername(args[1]);
			user->setFullname(fullName);
			user->sendPacket(to_str+"001 "+user->getNickname()+" :Welcome to the Internet Relay Network "+user->getId()+"\r\n");
			MOTD(user, args);
			
			if (user->getStatus() == REGISTER)
				user->setStatus(ONLINE);
			else
				user->setStatus(DELETE);
			return EXIT_SUCCESS;
		}

		int		PASS(User *user __attribute__((unused)), vector<string> &args) {
			if (user->getStatus() != PASSWORD)
				return EXIT_FAILURE;
			if (_config["password"].size() and _config["password"].compare(args.back())) {
				user->setStatus(DELETE);
				return RETURN_FAILURE;
			}
			if (not _config["password"].size() and args.back().size()) {
				user->setStatus(DELETE);
				return RETURN_FAILURE;
			}
			user->setStatus(REGISTER);
			return EXIT_SUCCESS;
		}

		int		MOTD(User* user, vector<string>& args __attribute__((unused))) {
			if (user->getStatus() != REGISTER)
				return EXIT_FAILURE;
			ifstream	ifs;
			string buffer, content;

			ifs.open(_config["motd"].c_str());
			while (getline(ifs, buffer)) {
				content.append(buffer);
				buffer.clear();
				content.append("\n");
			}
			ifs.close();
			if (send(user->getFd(), content.c_str(), content.size(), 0) < 0)
				throw ServerException("send() failed");
			content.clear();
			return EXIT_SUCCESS;
		}

		int		AWAY(User *user, vector<string> &args __attribute__((unused))) {
			if (user->getStatus() != ONLINE)
				return EXIT_FAILURE;
			if (args.size() > 1) {
				user->setAway(true);
				user->sendPacket(":"+user->getId()+" "+RPL_NOWAWAY+" "+user->getNickname()+" :You have been marked as being away\r\n");
			} else {
				user->setAway(false);
				user->sendPacket(":"+user->getId()+" "+RPL_UNAWAY+" "+user->getNickname()+" :You are no longer marked as being away\r\n");
			}
			return RETURN_SUCCESS;
		}

		int		WHOIS(User *user, vector<string> &args) {
			if (args.size() < 2)
				return RETURN_FAILURE;
			User*	target;
			string	packet;
			string	channel;

			if (args.size() == 2)
				channel = "*";
			else if (args.size() == 3)
				channel = (not args[1].compare(0, 1, "#")) ? (args[1]) : ("#"+args[1]);
			if ((args.size() != 3 or _channels.find(args[1]) != _channels.end()) and (target = findByName(args.back())))
			{
				packet = ":"+user->getId()+" "+RPL_WHOISUSER+" "+user->getNickname()+" "+target->getNickname()+" "+target->getUsername()+" "+target->getHostname()+" "+channel+" :"+target->getFullname()+"\n:"
					+user->getId()+" "+RPL_ENDOFWHOIS+" "+user->getNickname()+" "+args.back()+" :End of /WHOIS list\r\n";
			} else {
				packet = ":"+user->getId()+" "+ERR_NOSUCHNICK+" "+user->getNickname()+" "+args[1]+" :No such nick/channel\n:"
					+user->getId()+" "+RPL_ENDOFWHOIS+" "+user->getNickname()+" "+args.back()+" :End of /WHOIS list\r\n";
			}
			user->sendPacket(packet);
			return EXIT_SUCCESS;
		}

		int		WHO(User *user, vector<string> &args) {
			if (args.size() < 2)
				return RETURN_FAILURE;
			User*	target;
			string	packet;
			string	channel;

			if (args.size() == 2)
				channel = "*";
			else if (args.size() == 3)
				channel = (not args[1].compare(0, 1, "#")) ? (args[1]) : ("#"+args[1]);
			if ((args.size() != 3 or _channels.find(args[1]) != _channels.end()) and (target = findByName(args.back())))
			{
				packet = ":"+user->getId()+" "+RPL_WHOISUSER+" "+user->getNickname()+" "+target->getNickname()+" "+target->getUsername()+" "+target->getHostname()+" "+channel+" :"+target->getFullname()+"\n:"
					+user->getId()+" "+RPL_ENDOFWHOIS+" "+user->getNickname()+" "+args.back()+" :End of /WHO list\r\n";
			} else {
				packet = ":"+user->getId()+" "+ERR_NOSUCHNICK+" "+user->getNickname()+" "+args[1]+" :No such nick/channel\n:"
					+user->getId()+" "+RPL_ENDOFWHOIS+" "+user->getNickname()+" "+args.back()+" :End of /WHO list\r\n";
			}
			user->sendPacket(packet);
			return EXIT_SUCCESS;
		}

		int		WHOWAS(User *user, vector<string> &args) {
			if (args.size() < 2)
				return RETURN_FAILURE;
			User*	target;
			string	packet;
			string	channel;

			if (args.size() == 2)
				channel = "*";
			else if (args.size() == 3)
				channel = (not args[1].compare(0, 1, "#")) ? (args[1]) : ("#"+args[1]);
			if ((args.size() != 3 or _channels.find(args[1]) != _channels.end()) and (target = findByName(args.back())))
			{
				packet = ":"+user->getId()+" "+RPL_WHOWASUSER+" "+user->getNickname()+" "+target->getNickname()+" "+target->getUsername()+" "+target->getHostname()+" "+channel+" :"+target->getFullname()+"\n:"
					+user->getId()+" "+RPL_ENDOFWHOWAS+" "+user->getNickname()+" "+args.back()+" :End of /WHOWAS list\r\n";
			} else {
				packet = ":"+user->getId()+" "+ERR_NOSUCHNICK+" "+user->getNickname()+" "+args[1]+" :No such nick/channel\n:"
					+user->getId()+" "+RPL_ENDOFWHOWAS+" "+user->getNickname()+" "+args.back()+" :End of /WHOWAS list\r\n";
			}
			user->sendPacket(packet);
			return EXIT_SUCCESS;
		}

		int		OPER(User *user, vector<string> &args) {
			if (args.size() < 3 or user->getStatus() != ONLINE)
				return RETURN_FAILURE;
			if (args[1].compare(_config.at("oper_user")) || args[2].compare(_config.at("oper_password"))) {
				user->sendPacket(":"+user->getId()+" "+ERR_PASSWDMISMATCH+" "+user->getNickname()+" :Password incorrect\r\n");
				return RETURN_FAILURE;
			}
			user->setGrade(Operator);
			user->sendPacket(":"+user->getId()+" "+RPL_YOUREOPER+" "+user->getNickname()+" :You are now an IRC operator\r\n");
			return RETURN_SUCCESS;
		}

		int		DIE(User *user, vector<string> &args __attribute__((unused))) {
			if (user->getGrade() != Operator)
				return RETURN_FAILURE;
			_running = false;
			return RETURN_SUCCESS;
		}

		int 	LIST(User *user, vector<string> &args) {
			if (args.size() >= 2)
				return RETURN_FAILURE;
			string	packet;
			for (map<string, Channel*> ::iterator it = _channels.begin(); it != _channels.end(); ++it)
				packet += ":"+user->getId()+" "+RPL_LIST+" "+user->getNickname()+" "+it->first+" "+it->second->getUsers().size()+" :\n";
			packet += ":"+user->getId()+" "+RPL_LISTEND+" "+user->getNickname()+" :End of /LIST\r\n";
			user->sendPacket(packet);
			return RETURN_SUCCESS;
		}

		int		KILL(User *user, vector<string> &args) {
			if (args.size() < 3)
				return RETURN_FAILURE;
			if (user->getGrade() != Operator) {
				user->sendPacket(":"+user->getId()+" "+ERR_NOPRIVILEGES+" "+user->getNickname()+" :You're not an IRC operator\r\n");
				return RETURN_FAILURE;
			}
			User* 	target = findByName(args[1]);
			if (target == NULL) {
				user->sendPacket(":"+user->getId()+" "+ERR_NOSUCHNICK+" "+user->getNickname()+" :No such nick/channel\r\n");
				return RETURN_FAILURE;
			}
			string reason = getPastDoubleDot(args);
			for (map<string, Channel*>::iterator chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt)
				chanIt->second->quit(target, reason);
			target->sendPacket(":"+user->getId()+" KILL "+args[1]+" :"+reason+"\r\n");
			delUser(target->getFd());
			return RETURN_SUCCESS;
		}

		/*
		 *	Execptions
		 */
		class 	ServerException : public std::exception {
				string _msg;
			public:
				ServerException(string const &msg) : _msg("Server::" + msg + ((errno != 0) ? to_str+" "+strerror(errno) : "")) {}
				virtual const char *what() const throw() { return _msg.c_str(); }
				virtual ~ServerException() _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {return;};
		};
};

#endif
