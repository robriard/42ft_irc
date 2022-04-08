
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.class.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: unknow <unknow@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/28 13:06:59 by unknow            #+#    #+#             */
/*   Updated: 2022/04/06 15:39:09 by riblanc          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_CLASS_HPP
# define USER_CLASS_HPP

# include "Console.class.hpp"
# include <poll.h>
# include <netdb.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
# include <vector>
# include <string.h>

using namespace std;
# define BUFFER_SIZE 2048

/*	******************************
 *		GRADES
 *	****************************** */
enum userStatus {
	DELETE,
	PASSWORD,
	REGISTER,
	ONLINE
};

enum userGrade {
	Member,
	Moderator,
	Operator
};

class User {
	private:
		const int		_fd;
		string			_userName;
		string			_nickName;
		string			_fullName;
		string			_hostName;
		string			_hostAddr;
		userStatus		_status;
		userGrade		_grade;
		vector<string>	_waitToSend;
		string			_incompletePacket;
		bool 			_away;
		time_t			_lastPing;

	public:
		User(int fd, struct sockaddr_in addr, string username = "default_userName", string nickname = "default_nickName", string fullname = "default_fullName") :
		_fd(fd), _userName(username), _nickName(nickname), _fullName(fullname), _hostAddr(inet_ntoa(addr.sin_addr)), _status(PASSWORD), _grade(Member), _away(false), _lastPing(time(0)) {		
			fcntl(_fd, F_SETFL, O_NONBLOCK);
			char hostname[NI_MAXHOST];
			if (getnameinfo((struct sockaddr *)&addr, sizeof(addr), hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV) != 0) {
				console.error("getnameinfo() failed");
				return;
			} else
				_hostName = hostname;
			return;
		};
		~User(void) {close(_fd);};

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

		vector<string>				parsePacket(string packet) {
			return (tokenize(packet, "\r\n"));
		};

		pair<int, vector<string> >	receivePacket(void) {
			pair<int, vector<string> > ret;
			char	buffer[BUFFER_SIZE];
			string	packet;

			fill(buffer, buffer + BUFFER_SIZE, 0);
			if ((ret.first = recv(_fd, buffer, sizeof(buffer), 0)) <= 0) {
				ret.second = vector<string>();
#ifdef _DEBUG
				if (ret.first == 0)
					console.log(getUsername()+" close her connection");
#endif
				return ret;
			}
			packet = _incompletePacket + buffer;
			_incompletePacket.clear();
			_incompletePacket.insert(0, packet, (packet.rfind("\n") + 1), (packet.size() - packet.rfind("\n") + 1));
			packet.resize(packet.rfind("\n") + 1);
			ret.second = parsePacket(packet);

#ifdef _DEBUG
			console.log(to_str+ret.first+" bytes received from "+getNickname());
			for (size_t i=0; i<ret.second.size(); i++)
				console.debug("Packet received from "+_nickName+" (fd "+_fd+") = [ "+ret.second[i]+" ]", ORANGE | BOLD);
			if (_incompletePacket.size())
				console.debug("Packet not ended from "+_nickName+" (fd "+_fd+") = [ "+_incompletePacket+" ]", ORANGE | BOLD);
#endif
			return ret;
		};

		int							sendPacket(string packet) {
			int ret = send(_fd, packet.c_str(), packet.size(), 0);
#ifdef _DEBUG
			console.debug(to_str+"Packet send to "+_nickName+" (fd "+_fd+") = [ "+packet.substr(0, packet.size() - 2)+" ] "+ret, CYAN | BOLD);
#endif
			return ret < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
		};

		int							getFd(void) const {return _fd;};
		string						getUsername(void) {return _userName;};
		string						getNickname(void) {return _nickName;};
		string						getFullname(void) {return _fullName;};
		userStatus					getStatus(void) const {return _status;};
		userGrade					getGrade(void) const {return _grade;};
		string						getHostname(void) const {return _hostName;};
		string						getId(void) const {return to_str+_nickName+"!"+_userName+"@"+_hostName;};
		bool 						getAway(void) const {return _away;};
		time_t						getLastPing(void) const {return _lastPing;};

		void						setLastPing(time_t t) {_lastPing = t;};
		void						setUsername(string const & userName) {_userName = userName;};
		void						setNickname(string const & nickName) {_nickName = nickName;};
		void						setFullname(string const & fullName) {_fullName = fullName;};
		void						setStatus(userStatus status) {_status = status;};
		void						setGrade(userGrade newGrade) {_grade = newGrade;};
		void						setAway(bool b) {_away = b;};
};

#endif
