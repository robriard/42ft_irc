/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgaubert <sgaubert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/14 13:50:21 by sgaubert          #+#    #+#             */
/*   Updated: 2022/04/05 12:22:35 by riblanc          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_HPP
#define ERROR_HPP

enum Error {

	ERR_NOSUCHNICK = 401,
		// "<pseudonyme> :No such nick/channel"
	// Utilisé pour indiquer que le pseudonyme passé en paramètre à la commande n'est pas actuellement utilisé.

	ERR_NOSUCHSERVER = 402,
		// "<nom de serveur> :No such server"
	// Utilisé pour indiquer que le nom du serveur donné n'existe pas actuellement.

	ERR_NOSUCHCHANNEL = 403,
		// "<nom de canal> :No such channel"
	// Utilisé pour indiquer que le nom de canal donné est invalide.

	ERR_CANNOTSENDTOCHAN = 404,
		// "<nom de canal> :Cannot send to channel"
	// Envoyé à un utilisateur qui (a) soit n'est pas dans un canal en mode +n ou (b) n'est pas opérateur (ou mode +v) sur un canal en mode +m ; et essaie d'envoyer un PRIVMSG à ce canal.

	ERR_TOOMANYCHANNELS = 405,
		// "<nom de canal> :You have joined too many channels"
	// Envoyé à un utilisateur quand il a atteint le nombre maximal de canaux qu'il est autorisé à accéder simultanément, s'il essaie d'en rejoindre un autre.

	ERR_WASNOSUCHNICK = 406,
		// "<nom de canal> :There was no such nickname"
	// Renvoyé par WHOWAS pour indiquer qu'il n'y a pas d'information dans l'historique concernant ce pseudonyme.

	ERR_TOOMANYTARGETS = 407,
		// "<destination> :Duplicate recipients. No message delivered"
	// Renvoyé à un client qui essaie d'envoyer un PRIVMSG/NOTICE utilisant le format de destination utilisateur@hôte pour lequel utilisateur@hôte a plusieurs occurrences.

	ERR_NOORIGIN = 409,
		// ":No origin specified"
	// Message PING ou PONG sans le paramètre origine qui est obligatoire puisque ces commandes doivent marcher sans préfixe.

	ERR_NORECIPIENT = 411,
		// ":No recipient given (<commande>)"
	// Pas de destinataire.

	ERR_NOTEXTTOSEND = 412,
		// ":No text to send"
	// Pas de texte à envoyer.

	ERR_NOTOPLEVEL = 413,
		// "<masque> :No toplevel domain specified"
	// Domaine principal non spécifié.

	ERR_WILDTOPLEVEL = 414,
		// "<masque> :Wildcard in toplevel domain"
	// Joker dans le domaine principal

	// Les erreurs 412-414 sont renvoyées par PRIVMSG pour indiquer que le message n'a pas été délivERR_NOTOPLEVELERR_WILDTOPLEVEL = sont les erreurs renvoyées lors d'une utilisation invalide de "PRIVMSG $<serveur>" ou de "PRIVMSG #<hôte>".,

	ERR_UNKNOWNCOMMAND = 42,
		// "<commande> :Unknown command"
	// Renvoyé à un client enregistré pour indiquer que la commande envoyée est inconnue du serveur.

	ERR_NOMOTD = 422,
		// ":MOTD File is missing"
	// Le fichier MOTD du serveur n'a pas pu être ouvert.

	ERR_NOADMININFO = 423,
		// "<serveur> :No administrative info available"
	// Renvoyé par un serveur en réponse à un message ADMIN quand il y a une erreur lors de la recherche des informations appropriées.

	ERR_FILEERROR = 424,
		// ":File error doing <opération> on <fichier>
	// Message d'erreur générique utilisé pour rapporter un échec d'opération de fichier durant le traitement d'un message.

	ERR_NONICKNAMEGIVEN = 431,
		// ":No nickname given"
	// Renvoyé quand un paramètre pseudonyme attendu pour une commande n'est pas fourni.

	ERR_ERRONEUSNICKNAME = 432,
		// "<pseudo> :Erroneus nickname"
	// Renvoyé après la réception d'un message NICK qui contient des caractères qui ne font pas partie du jeu autorisé. Voir les sections 1 et 2.2 pour les détails des pseudonymes valides.

	ERR_NICKNAMEINUSE = 433,
		// "<nick> :Nickname is already in use"
	// Renvoyé quand le traitement d'un message NICK résulte en une tentative de changer de pseudonyme en un déjà existant.

	ERR_NICKCOLLISION = 436,
		// "<nick> :Nickname collision KILL"
	// Renvoyé par un serveur à un client lorsqu'il détecte une collision de pseudonymes (enregistrement d'un pseudonyme qui existe déjà sur un autre serveur).

	ERR_USERNOTINCHANNEL = 441,
		// "<pseudo> <canal> :They aren't on that channel"
	// Renvoyé par un serveur pour indiquer que l'utilisateur donné n'est pas dans le canal spécifié.

	ERR_NOTONCHANNEL = 442,
		// "<canal> :You're not on that channel"
	// Renvoyé par le serveur quand un client essaie une commande affectant un canal dont il ne fait pas partie.

	ERR_USERONCHANNEL = 443,
		// "<utilisateur> <channel> :is already on channel"
	// Renvoyé quand un client essaie d'inviter un utilisateur sur un canal où il est déjà.

	ERR_NOLOGIN = 444,
		// "<utilisateur> :User not logged in"
	// Renvoyé par un SUMMON si la commande n'a pas pu être accomplie, car l'utilisateur n'est pas connecté.

	ERR_SUMMONDISABLED = 445,
		// ":SUMMON has been disabled"
	// Renvoyé en réponse à une commande SUMMON si le SUMMON est désactivé. Tout serveur qui ne gère pas les SUMMON doit retourner cette valeur.

	ERR_USERSDISABLED = 446,
		// ":USERS has been disabled"
	// Retourné en réponse à une commande USERS si la commande est désactivée. Tout serveur qui ne gère pas les USERS doit retourner cette valeur.

	ERR_NOTREGISTERED = 451,
		// ":You have not registered"
	// Retourné par le serveur pour indiquer à un client qu'il doit être enregistré avant que ses commandes soient traitées.

	ERR_NEEDMOREPARAMS = 461,
		// "<commande> :Not enough parameters"
	// Renvoyé par un serveur par de nombreuses commandes, afin d'indiquer que le client n'a pas fourni assez de paramètres.

	ERR_ALREADYREGISTRED = 462,
		// ":You may not reregister"
	// Retourné par le serveur à tout lien qui tente de changer les détails enregistrés (tels que mot de passe et détails utilisateur du second message USER)

	ERR_NOPERMFORHOST = 463,
		// ":Your host isn't among the privileged"
	// Renvoyé à un client qui essaie de s'enregistrer sur un serveur qui n'accepte pas les connexions depuis cet hôte.

	ERR_PASSWDMISMATCH = 464,
		// ":Password incorrect"
	// Retourné pour indiquer l'échec d'une tentative d'enregistrement d'une connexion dû à un mot de passe incorrect ou manquant.

	ERR_YOUREBANNEDCREEP = 645,
		// ":You are banned from this server"
	// Retourné après une tentative de connexion et d'enregistrement sur un serveur configuré explicitement pour vous refuser les connexions.

	ERR_KEYSET = 467,
		// "<canal> :Channel key already set"
	// Clé de canal déjà définie.

	ERR_CHANNELISFULL = 471,
		// "<canal> :Cannot join channel (+l)"
	// Impossible de joindre le canal (+l)

	ERR_UNKNOWNMODE = 472,
		// "<caractère> :is unknown mode char to me"
	// Mode inconnu.

	ERR_INVITEONLYCHAN = 473,
		// "<canal> :Cannot join channel (+i)"
	// Impossible de joindre le canal (+i).

	ERR_BANNEDFROMCHAN = 474,
		// "<canal> :Cannot join channel (+b)"
	// Impossible de joindre le canal (+b).

	ERR_BADCHANNELKEY = 475,
		// "<canal> :Cannot join channel (+k)"
	// Impossible de joindre le canal (+k).

	ERR_NOPRIVILEGES = 481,
		// ":Permission Denied- You're not an IRC operator"
	// Toute commande qui requiert le privilège d'opérateur pour opérer doit retourner cette erreur pour indiquer son échec.

	ERR_CHANOPRIVSNEEDED = 482,
		// "<canal> :You're not channel operator"
	// Toute commande qui requiert les privilèges 'chanop' (tels les messages MODE) doit retourner ce message à un client qui l'utilise sans être chanop sur le canal spécifié.

	ERR_CANTKILLSERVER = 483,
		// ":You cant kill a server!"
	// Toute tentative d'utiliser la commande KILL sur un serveur doit être refusée et cette erreur renvoyée directement au client.

	ERR_NOOPERHOST = 491,
		// ":No O-lines for your host"
	// Si un client envoie un message OPER et que le serveur n'a pas été configuré pour autoriser les connexions d'opérateurs de cet hôte, cette erreur doit être retournée.

	ERR_UMODEUNKNOWNFLAG = 501,
		// ":Unknown MODE flag"
	// Renvoyé par un serveur pour indiquer que le message MODE a été envoyé avec un pseudonyme et que le mode spécifié n'a pas été identifié.

	ERR_USERSDONTMATCH = 502
		// ":Cant change mode for other users"
	// Erreur envoyée à tout utilisateur qui essaie de voir ou de modifier le mode utilisateur d'un autre client. 
};

#endif
