/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgaubert <sgaubert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/15 11:53:54 by sgaubert          #+#    #+#             */
/*   Updated: 2022/04/05 12:22:47 by riblanc          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

enum Response {

	RPL_NONE = 301,

	// Numéro de réponse bidon. Inutilisé.

	RPL_USERHOST = 302,
	// 	":[<réponse>{<espace><réponse>}]"

	// Format de réponse utilisé par USERHOST pour lister les réponses à la liste des requêtes. La chaîne de réponse est composée ainsi :
	// <réponse> ::= <pseudo>['*'] '=' <'+'|'-'><hôte>
	// Le '*' indique si le client est enregistré comme opérateur. Les caractères '-' ou '+' indiquent respectivement si le client a défini un message AWAY ou non.

	RPL_ISON = 303,
	// 	":[<pseudo> {<espace><pseudo>}]"

	// Format de réponse utilisé par ISON pour lister les réponses à la liste de requête.

	RPL_AWAY = 301,
	// 	"<pseudo> :<message d'absence>"
	RPL_UNAWAY = 305,
	// 	":You are no longer marked as being away"
	RPL_NOWAWAY = 306,
	// 	":You have been marked as being away"

	// Ces trois réponses sont utilisées avec la commande AWAY (si elle est autoriséRPL_AWAY est envoyé à tout client qui envoie un PRIVMSG à un client abseRPL_AWAY n'est envoyé que par le serveur sur lequel le client est connecté. Les réponRPL_UNAWAYRPL_NOWAWAY sont envoyées quand un client retire et définit un message A =WAY.,

	RPL_WHOISUSER = 311,
	// 	"<pseudo> <utilisateur> <hôte> * :<vrai nom>"
	RPL_WHOISSERVER = 312,
	// 	"<pseudo> <serveur> :<info serveur>"
	RPL_WHOISOPERATOR = 313,
	// 	"<pseudo> :is an IRC operator"
	RPL_WHOISIDLE = 317,
	// 	"<pseudo> <integer> :seconds idle"
	RPL_ENDOFWHOIS = 318,
	// 	"<pseudo> :End of /WHOIS list"
	RPL_WHOISCHANNELS = 319,
	// 	"<pseudo> :{[@|+]<canal><espace>}"

	// Les réponses 311 à 313 et 317 à 319 sont toutes générées en réponse à un message WHOIS. S'il y a assez de paramètres, le serveur répondant doit soit formuler une réponse parmi les numéros ci-dessus (si le pseudo recherché a été trouvé) ou renvoyer un message d'erreur. Le '*' dRPL_WHOISUSER est là en tant que littéral et non en tant que joker. Pour chaque jeu de réponses, sRPL_WHOISCHANNELS peut apparaître plusieurs fois (pour les longues listes de noms de canaux). Les caractères '@' et '+' à côté du nom de canal indiquent si un client est opérateur de canal, ou si on l'a autorisé à parler dans un canal modéré. La répoRPL_ENDOFWHOIS est utilisée pour marquer la fin de la réponse WH =OIS.,

	RPL_WHOWASUSER = 314,
	// 	"<pseudo> <utilisateur> <hôte> * :<vrai nom>"
	RPL_ENDOFWHOWAS = 369,
	// 	"<pseudo> :End of WHOWAS"

	// Lorsqu'il répond à un message WHOWAS, un serveur doit utiliRPL_WHOWASUSRPL_WHOISSERVER ou ERR_WASNOSUCHNICK pour chacun des pseudonymes de la liste fournie. A la fin de toutes les réponses, il doit y avoirRPL_ENDOFWHOWAS (même s'il n'y a eu qu'une réponse, et que c'était une erre =ur).,

	RPL_LISTSTART = 312,
	// 	"Channel :Users Name"
	RPL_LIST = 322,
	// 	"<canal> <# visible> :<sujet>"
	RPL_LISTEND = 323,
	// 	":End of /LIST"

	// Les réponRPL_LISTSTARPL_LIRPL_LISTEND marquent le début, les réponses proprement dites, et la fin du traitement d'une commande LIST. S'il n'y a aucun canal disponible, seules les réponses de début et de fin sont envoy =ées.,

	RPL_CHANNELMODEIS = 324,
	// 	"<canal> <mode> <paramètres de mode>"
	RPL_NOTOPIC = 331,
	// 	"<canal> :No topic is set"
	RPL_TOPIC = 332,
	// 	"<canal> :<sujet>"

	// Lors de l'envoi d'un message TOPIC pour déterminer le sujet d'un canal, une de ces deux réponses est envoyée. Si le sujet est défiRPL_TOPIC est renvoyée, sinon c'RPL_NOTO =PIC.,

	RPL_INVITING = 341,
	// 	"<canal> <pseudo>"

	// Renvoyé par un serveur pour indiquer que le message INVITE a été enregistré, et est en cours de transmission au client final.

	RPL_SUMMONING = 342,
	// 	"<utilisateur> :Summoning user to IRC"

	// Renvoyé par un serveur en réponse à un message SUMMON pour indiquer qu'il appelle cet utilisateur.

	RPL_VERSION = 351,
	// 	"<version>.<debuglevel> <serveur> :<commentaires>"

	// Réponse du serveur indiquant les détails de sa version. <version> est la version actuelle du programme utilisé (comprenant le numéro de mise à jour) et <debuglevel> est utilisé pour indiquer si le serveur fonctionne en mode débugage.
	// Le champ <commentaire> peut contenir n'importe quel commentaire au sujet de la version, ou des détails supplémentaires sur la version.

	RPL_WHOREPLY = 352,
	// 	"<canal> <utilisateur> <hôte> <serveur> <pseudo> <H|G>[*][@|+] :<compteur de distance> <vrai nom>"
	RPL_ENDOFWHO = 315,
	// 	"<nom> :End of /WHO list"

	// La paRPL_WHOREPLYRPL_ENDOFWHO est utilisée en réponse à un message WHO.RPL_WHOREPLY n'est envoyé que s'il y a une correspondance à la requête WHO. S'il y a une liste de paramètres fournie avec le message WHO,RPL_ENDOFWHO doit être envoyé après le traitement de chaque élément de la liste, <nom> étant l'élém =ent.,

	RPL_NAMREPLY = 353,
	// 	"<canal> :[[@|+]<pseudo> [[@|+]<pseudo> [...]]]"
	RPL_ENDOFNAMES = 366,
	// 	"<canal> :End of /NAMES list"

	// En réponse à un message NAMES, une paire consistantRPL_NAMREPLYRPL_ENDOFNAMES est renvoyée par le serveur au client. S'il n'y a pas de canal résultant de la requête, sRPL_ENDOFNAMES est retourné. L'exception à cela est lorsqu'un message NAMES est envoyé sans paramètre et que tous les canaux et contenus visibles sont renvoyés en une suite de messRPL_NAMEREPLY avecRPL_ENDOFNAMES indiquant la  =fin.,

	RPL_LINKS = 364,
	// 	"<masque> <serveur> :<compteur de distance> <info serveur>"
	RPL_ENDOFLINKS = 365,
	// 	"<mask> :End of /LINKS list"

	// En réponse à un message LINKS, un serveur doit répondre en utilisant le nomRPL_LINKS, et indiquer la fin de la liste avec une répoRPL_ENDOFLI =NKS.,

	RPL_BANLIST = 367,
	// 	"<canal> <identification de bannissement>"
	RPL_ENDOFBANLIST = 368,
	// 	"<canal> :End of channel ban list"

	// Quand il liste les bannissements actifs pour un canal donné, un serveur doit renvoyer la liste en utilisant les messaRPL_BANLISTRPL_ENDOFBANLIST.RPL_BANLIST différent doit être utilisé pour chaque identification de bannissement. Après avoir listé les identifications de bannissement (s'il y en a),RPL_ENDOFBANLIST doit être renv =oyé.,

	RPL_INFO = 371,
	// 	":<chaîne>"
	RPL_ENDOFINFO = 374,
	// 	":End of /INFO list"

	// Un serveur répondant à un message INFO doit envoyer toute sa série d'info en une suite de réponRPL_INFO, avecRPL_ENDOFINFO pour indiquer la fin des répon =ses.,

	RPL_MOTDSTART = 375,
	// 	":- <serveur> Message of the day - "
	RPL_MOTD = 372,
	// 	":- <texte>"
	RPL_ENDOFMOTD = 376,
	// 	":End of /MOTD command"

	// Lorsqu'il répond à un message MOTD et que le fichier MOTD est trouvé, le fichier est affiché ligne par ligne, chaque ligne ne devant pas dépasser 80 caractères, en utilisant des réponses au forRPL_MOTD. Celles-ci doivent être encadrées parRPL_MOTDSTART (avant RPL_MOTDs) etRPL_ENDOFMOTD (apr =ès).,

	RPL_YOUREOPER = 381,
	// 	":You are now an IRC operator"

	// RPL_YOUREOPER est renvoyé à un client qui vient d'émettre un message OPER et a obtenu le statut d'opérat =eur.,

	RPL_REHASHING = 382,
	// 	"<fichier de configuration> :Rehashing"

	// Si l'option REHASH est activée et qu'un opérateur envoie un message REHASH,RPL_REHASHING est renvoyé à l'opérat =eur.,

	RPL_TIME = 391,
	// 	"<serveur> :<chaîne indiquant l'heure locale du serveur>"

	// Lorsqu'il répond à un message TIME, un serveur doit répondre en utilisant le forRPL_TIME ci-dessus. La chaîne montrant l'heure ne doit contenir que le jour et l'heure corrects. Il n'y a pas d'obligation supplémenta =ire.,

	RPL_USERSSTART = 392,
	// 	":UserID Terminal Hôte"
	RPL_USERS = 393,
	// 	":%-8s %-9s %-8s"
	RPL_ENDOFUSERS = 394,
	// 	":End of users"
	RPL_NOUSERS = 395,
	// 	":Nobody logged in"

	// Si le message USERS est géré par un serveur, les réponRPL_USERSTARPL_USERPL_ENDOFUSERSRPL_NOUSERS sont utiliséRPL_USERSSTART doit être envoyé en premier, suivi par soit une séquenceRPL_USERS soit un uniRPL_NOUSER. Enfin, vientRPL_ENDOFUS =ERS.,

	RPL_TRACELINK = 200,
	// 	"Link <version & niveau de débugage> <destination> <serveur suivant>"
	RPL_TRACECONNECTING = 201,
	// 	"Try. <classe> <serveur>"
	RPL_TRACEHANDSHAKE = 202,
	// 	"H.S. <classe> <serveur>"
	RPL_TRACEUNKNOWN = 203,
	// 	"???? <classe> [<adresse IP du client au format utilisant des points>]"
	RPL_TRACEOPERATOR = 204,
	// 	"Oper <classe> <pseudo>"
	RPL_TRACEUSER = 205,
	// 	"User <classe> <pseudo>"
	RPL_TRACESERVER = 206,
	// 	"Serv <classe> <int>S <int>C <serveur> <pseudo!utilisateur|*!*>@<hôte|serveur>"
	RPL_TRACENEWTYPE = 208,
	// 	"<nouveau type> 0 <nom du client>"
	RPL_TRACELOG = 261,
	// 	"File <fichier log> <niveau de débugage>"

	// RPL_TRACE* sont tous renvoyés par le serveur en réponse à un message TRACE. Le nombre de messages retournés dépend de la nature du message TRACE, et s'il a été envoyé par un opérateur ou non. Il n'y a pas d'ordre définissant lequel doit être le premier. Les réponRPL_TRACEUNKNORPL_TRACECONNECTINGRPL_TRACEHANDSHAKE sont toutes utilisées pour des connexions qui n'ont pas été complètement établies, et sont soit inconnues, soit toujours en cours de connexion, soit dans la phase terminale de la 'poignée de main du serveuRPL_TRACELINK est envoyé par tout serveur qui traite un message TRACE et doit le transmettre à un autre serveur. La listeRPL_TRACELINK envoyée en réponse à une commande TRACE traversant le réseau IRC devrait refléter la connectivité actuelle des serveurs le long du chemRPL_TRACENEWTYPE est utilisé pour toute connexion qui n'entre pas dans les autres catégories, mais qui est néanmoins affic =hée.,

	RPL_STATSLINKINFO = 211,
	// 	"<nom du lien> <sendq> <messages envoyés> <octets envoyés> <message reçus> <octets reçus> <temps de connexion>"
	RPL_STATSCOMMANDS = 212,
	// 	"<commande> <compteur>"
	RPL_STATSCLINE = 213,
	// 	"C <hôte> * <nom> <port> <classe>"
	RPL_STATSNLINE = 214,
	// 	"N <hôte> * <nom> <port> <classe>"
	RPL_STATSILINE = 215,
	// 	"I <hôte> * <hôte> <port> <classe>"
	RPL_STATSKLINE = 216,
	// 	"K <hôte> * <nom d'utilisateur> <port> <classe>"
	RPL_STATSYLINE = 218,
	// 	"Y <classe> <fréquence des PINGS> <fréquence de connexion> <sendq max>"
	RPL_ENDOFSTATS = 219,
	// 	"<lettre de stats> :End of /STATS report"
	RPL_STATSLLINE = 241,
	// 	"L <masque d'hôte> * <nom de serveur> <profondeur maxi>"
	RPL_STATSUPTIME = 242,
	// 	":Server Up %d days %d:%02d:%02d"
	RPL_STATSOLINE = 243,
	// 	"O <masque d'hôte> * <nom>"
	RPL_STATSHLINE = 244,
	// 	"H <masque d'hôte> * <nom de serveur>"

	RPL_UMODEIS = 221,
	// 	"<chaîne de mode utilisateur>"

	// Pour répondre à une requête au sujet du mode du clieRPL_UMODEIS est renv =oyé.,

	RPL_LUSERCLIENT = 251,
	// 	":There are <entier> users and <entier> invisible on <entier> servers"
	RPL_LUSEROP = 252,
	// 	"<entier> :operator(s) online"
	RPL_LUSERUNKNOWN = 253,
	// 	"<entier> :unknown connection(s)"
	RPL_LUSERCHANNELS = 254,
	// 	"<entier> :channels formed"
	RPL_LUSERME = 255,
	// 	":I have <entier> clients and <integer> servers"

	// Lors du traitement d'un message LUSERS, le serveur envoie un lot de réponses paRPL_LUSERCLIERPL_LUSERRPL_USERUNKNORPL_LUSERCHANNELSRPL_LUSERME. Lorsqu'il répond, un serveur doit envoRPL_LUSERCLIENTRPL_LUSERME. Les autres réponses ne sont renvoyées que si le nombre trouvé n'est pas  =nul.,

	RPL_ADMINME = 256,
	// 	"<serveur> :Administrative info"
	RPL_ADMINLOC1 = 257,
	// 	":<info admin>"
	RPL_ADMINLOC2 = 258,
	// 	":<info admin>"
	RPL_ADMINEMAIL = 259
	// 	":<info admin>"

	// Lorsqu'il répond à un message ADMIN, un serveur doit renvoyer les réponses RLP_ADMINMRPL_ADMINEMAIL et fournir un texte de message avec chacune. PRPL_ADMINLOC1, on attend une description de la ville et de l'état où se trouve le serveur, suivie des détails de l'université et du départemeRPL_ADMINLOC2), et finalement le contact administratif pour ce serveur (avec obligatoirement une adresse email) dRPL_ADMINEMA =IL. ,
};

#endif
