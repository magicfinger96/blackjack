# Blackjack compte-rendu

## Introduction

Le but était de faire un blackjack en réseau local.
Le groupe est composé de :
* Andréa Franco
* Nicolas Serf

## Fonctionnement général

Nous avons décidé de réaliser notre blackjack en réseau en utilisant UDP et TCP. La partie UDP est utilisée pour réaliser toutes la gestion de demander de serveur, c'est à dire lorsque le client envoie un broadcast à tous les serveurs disponibles sur le réseau local. La partie TCP quant à elle est utilisée pour toutes la partie gestion du jeu.

Ce choix a été décidé par simplicité de la gestion du protocole TCP qui assure la bonne réception des messages, et aussi dans une optique de projet plus 'professionnel', nous aurions probablement réalisé le même choix car nous réalisons un jeu en tour par tour, nous ne pouvons pas nous permettre de perdre une information et la réalisation en UDP, de maniére propre, aurait demandé de réaliser une surcouche important pour gérer la perte des données et la demande de retransmission.

Lorsqu'un serveur est plein, nous avons choisis de ne pas répondre au client, ce choix n'est lié en rien à la technique puisqu'il s'appuie uniquement sur une condition mais plus sur un choix logique (un utilisateur ne veut pas être géné dans sa sélection par un serveur qui ne pourra pas, dans tous les cas, l'accueillir).

### Déroulement d'une manche

Le déroulement d'une manche se passe comme suit, avec une étape se passant après la suivante si la précédente a été validée.

* Etape 1 : On regarde si un joueur est disponible
* Etape 2 : On éjecte les joueurs ne pouvant plus miser
* Etape 3 : On regarde de nouveau si il y a des joueurs disponibles
* Etape 4 : Si la table n'est pas pleine, on attend 10 secondes (pour laisser d'autres personnes se connecter)
* Etape 5 : On 'donne un jeton' au joueur pour spécifier qu'il participe à la manche en cours
* Etape 6 : On brule les 5 premières cartes
* Etape 7 : On lance le tour de mise, celui-ci dure 10 secondes
* Etape 8 : On éjecte du serveur les personnes n'ayant pas misé dans les 10 secondes
* Etape 9 : On réalise la distribution initiale des cartes, puis on informe tout les joueurs de la donne.
* Etape 10 : On rentre dans la boucle de jeu principale en demandant à chaque joueurs ces actions.
* Etape 11 : On distribue les gains
* Etape 12 : On réinitialise les joueurs et on recommence une manche (après les 10 secondes d'attente, permettant aux joueurs de la manche de voir le résultat de la table finale).

## Fonctionnement serveur

Le serveur fonctionne assez simplement en essayant de maintenir un nombre de thread minimal.
Tout d'abord, au lancement du serveur, ce dernier lance un thread qui va écouter tous les messages UDP et un deuxiéme qui s'occupe d'écouter toutes les demandes de session de TCP.

Lorsqu'un client se connecte que ce soit en UDP ou TCP, pour une raison d'efficacité, un nouveau thread est lancé pour gérer cette évènement, permettant de redonner tout de suite la main à l'autre thread pour réceptionner un nouveau message.

Pour réaliser ce lancement de thread proprement, un poll est mit en place qui regarde le socket (UDP ou TCP en fonction du thread) et qui lorsqu'il reçoit un évènement, permet de lancer un nouveau thread pour le gérer. Ce poll a une deuxième utilité. Permettre de proprement quitter le serveur. Puisqu'on ne reste bloqué que par tranche de 2 secondes sur le poll puis on revérifier que le serveur n'attend pas notre fin pour se terminer (à la réception d'un signal CTRL+C par exemple).

Une fois en jeu, nous sommes parti sur l'utilisation d'un unique socket de communication à la fois correspondant au joueur couramment en train de jouer. 
Cette solution nous a convenue mais elle a posé un problème lors de la déconnexion des joueurs. En effet, si ce n'est pas le tour du joueur, il nous est impossible de savoir en l'état si un joueur se déconnecte ou pas. 
Pour régler ce problème, nous avons réalisé une technique et une manière de penser qui fonctionne très bien :
* Si le joueur se déconnecte et que c'est son tour : Le serveur le saura en recevant un message null dans le socket et pourra ainsi propager la déconnexion aux clients, de même pour un envoi impossible.
* Si ce n'est pas son tour, durant le tour d'un autre joueur, on envoie toutes les secondes des messages aux autres clients excepté celui en train de jouer pour savoir si ils sont la, si l'envoi est raté, le client n'est plus la, on peut donc avertir tous les autres.

Cette solution nous plait moyennement, mais l'architecture que nous avons réalisé depuis le début ne permet pas de réaliser autre chose et d'avoir un visuel correct en même temps (ie : Les joueurs ont besoin de savoir rapidement lorsqu'un joueur se déconnecte et ne doivent pas attendre son tour).

Finalement, pour couper proprement le serveur, il suffit de faire un CTRL+C sur le processus, si une partie est en cours, le serveur attendra la fin de la partie pour avertir proprement de la fermeture du serveur et libérer ses ressources. De même, si aucune partie est en cours, le serveur peut prendre jusqu'à 2 secondes pour se terminer car il attend les threads qui sont en état de poll.

## Fonctionnement client

Du côté client, nous avons réalisé une interface épurée mais il n'était pas facile de placer tous les éléments au vu du nombre potentiel de 3 mains par joueur.
De ce fait, nous avons opté pour la visualisation des cartes via un système de scroll avec la souris.
Ainsi un joueur voulant visualiser une main devra d'abord cliquer sur cette dernière puis scroller avec sa souris vers le haut ou vers le bas pour regarder les cartes. Nous avons gérer le fait que 2 cartes similiaires (même couleur et même figure) se superposent par une petite pastille verte et bleue qui s'alterne de carte en carte. Lors de l'ajout d'une carte, la main est mise à jour en montrant ce tirage. En revanche, si cette main a été selectionnée par le joueur, celle ci restera a la même position de carte pour ne pas rendre la lecture difficile. (Impossibilité de pouvoir regarder la main d'un autre joueur en scrollant si ce dernier tire plusieurs cartes d'affilé sinon).

Nous avons aussi réalisé une petite boite de message pour que l'utilisateur puisse facilement voir les différentes actions réalisés par les joueurs et aussi les éventuelles déconnexions et les gains des joueurs.

Au niveau protocole, c'est le serveur qui prend toutes les décisions, nous conservons en cache plusieurs informations du côté client comme l'argent actuellement disponible du joueur. On a aussi une technique qui est d'envoyer un message donnant les possibilités des actions du joueur. Cependant, même si ces actions sont censés forcer le joueur a jouer correctement, le serveur réalise la vérification et envoie un message d'acceptation ou de rejet en fonction.

## Les difficultés 

Concernant le serveur, 2 problémes se sont posés.

* Le premier est la réalisation de la déconnexion des joueurs. En effet comme expliqué dans le fonctionnement du serveur, il était impossible de recevoir la déconnexion programmé d'un joueur car le serveur n'était pas spécialement à l'écoute du joueur, il a donc fallu programmer la technique expliquée au dessus pour palier à ce probléme. 
* Le deuxiéme probléme était la gestion de la connexion d'un joueur. Nous nous sommes rendu compte de ce problème assez tard dans le projet et la correction a du être réalisé avec l'architecture présente ce qui a complexifié la chose. Pour simplifier l'explication, une fonction avait besoin que le joueur soit ajouter à la liste des joueurs (impliquant que le joueur est disponible pour la réception de n'importe quel message) pour fonctionner, mais il fallait d'un autre côté que ce même joueur ne soit pas dans cette liste avant d'avoir reçu l'initialisation du plateau (les différentes informations déjà présentes sur la table). Cela posait probléme lorsque le thread qui initialise le joueur était stoppé avant d'envoyer l'initialisation du plateau, et que ce client maintenant disponible recevait les informations de la partie en cours, on avait ainsi une incohérence. Pour régler ce probléme, nous avons réalisé une fonction simulant ce nouveau client, ainsi avec cette fausse nouvelle identité présente, nous avons pu lancer la premiéère fonction tout en permettant de ne pas ajouter le nouveau dans la liste et permettre un déroulement normal de la fonction.

Concernant le client, nous avons eu aussi 2 problémes majeurs:

* Le premier probléme touche à l'interface et la représentation. En effet, nous voulions réaliser une application avec une taille de fenêtre assez petite pour toutes les résolutions et dans un même temps pouvoir afficher le plus d'informations possibles sans surcharger l'interface et rendre celle-ci lisible. C'est ainsi que nous avons décidé de réaliser le système de scrolling présenté en haut. C'est une solution à la fois lisible et élégante puisqu'on peut facilement se retrouver dans la main, et il est de plus facile de voir la somme totale de la main avec la valeur au dessus.

* Le deuxiéme problème a concerné la réaction aux différentes actions.

## Les choses qui ne fonctionnent pas 

A l'heure actuelle, et au vu des différents tests réalisés, toutes les fonctionnalités demandaient semblent présentes et correctes. La gestion des différentes deconnexions brutales ou programmées semblent gérées et valgrind sur le serveur renvoie un total de fuite égale à 0 et aucune erreur de contexte.

## Les ajouts bonus

Nous avons essayé de beaucoup travailler sur l'interface pour proposer une vue agréable et interactive où le joueur ne se perd pas. 
De plus, une IA basique est présente.
