#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>

#include "networkToolbox.h"
#include "server.h"
#include "game.h"
#include "messageBuilder.h"

#define MAX_ON_TABLE 7
#define NB_DECK 6
#define START_MONEY 100
#define MIN_BET 5
#define MAX_BET 100

Server* server;

void interrupt()
{
  printf("/!\\ INTERRUPTION RECEIVE /!\\\n");
  server->isRunning = 0;
}

int main(void)
{
  int status;

  /* ============================ */
  /*        DEMARRAGE RANDOM      */
  /* ============================ */
  srand(time(NULL));

  /* ============================ */
  /*    GESTION HANDLER SIGINT    */
  /* ============================ */
  struct sigaction intHandler;
  
  intHandler.sa_handler = interrupt;
  intHandler.sa_flags = SA_RESTART;
  sigemptyset(&intHandler.sa_mask);
  
  if(sigaction(SIGINT,&intHandler,NULL) < 0)
    {
      fprintf(stderr,"Fatal error at sigaction in child\n");
    }
  printf("******         SIGINT HANDLER DONE         ******\n");

  
  /* ============================ */
  /*   OUVERTURE SERVEUR ET JEU   */
  /* ============================ */
  printf("======   PREPARING SERVER INITIALIZATION   ======\n");
  printf("====== PREPARING SERVER GAME INIIALIZATION ======\n");
  if((server=constructServer(&status,2301,2302, NB_DECK,START_MONEY, MIN_BET, MAX_BET)) == NULL)
    {
      printf("Error at constructing server and game -- error %d\n",status);
      return 1;
    }
  else
    {
      printf("======     SERVER INITIALIZATION DONE      ======\n");
      printf("======    LAUNCH THREAD CONNECTION DONE    ======\n");
      printf("======   LAUNCH THREAD WANT TO PLAY DONE   ======\n");
      printf("======  LAUNCH THREAD CONNECT CHECKER DONE ======\n");
      printf("=====       GAME INITIALIZATION DONE       ======\n");

      printf("\n");
      printServer(server);
      printf("\n");
      
      while(server->isRunning)
	{
	  /* La boucle du serveur se passe comme suit 
	     - 1: On s'assure que tout les joueurs présents peuvent miser la mise minimal
	     - 2: On attend qu'au moins un joueur soit disponible pour lancer un round
	     - 3: On valide l'entrée dans le round des joueurs actuellemnt présent 
	     - 4: On réalise le brulage des premières cartes comme les règles
	     - 5: On réalise le tour de mise, c'est à dire :
	     5.1 : Pour chaque joueur, on demande la mise
	     5.2 : On attend la réponse du joueur à chaque fois
	     5.3 : On renvoie un message d'acceptation / de refus de la mise
	     - 6: On vérifie que tout les joueurs présent au round ont bien misé
	     - 7: On réalise la distribution des cartes pour les joueurs, pour cela:
	     7.1 : On distribue dans le serveur les différents joueurs
	     7.2 : On envoie à tout les joueurs les cartes des différents joueurs
	     - 8: Demander action joueurs - Fin joueur courant qd action = stay ou abandonner
	  */

	  /* Etape 1 : Mise minimale */
	  if(nbPlayerOnTable(server->game) >= 1)
	    { 
	      int i;
	      int continueWaitingLoop = 1;
		  
	      /* Waiting 10 seconds for more player except if already 7 */
	      printf("----------------------------------------------------\n");
	      printf("Préparation de la manche - 10 secondes restantes... \n");
	      printf("Démarrage d'un compteur via une méthode de sleep... \n");
	      printf("Réalisation d'un décompte pour pouvoir accueillir...\n");
	      printf("  - d'avantage de joueur et pas lancer manche à 1...\n");
	      printf("----------------------------------------------------\n\n");
		  
	      server->secondBeforeStart = 10;
	      sendGameStartIn(server, -1);

	      server->isWaitingForStart = 1;
	      for(i = 10; i >= 1 && continueWaitingLoop; i--)
		{
		  sendToAllChecking(server);
		  sendToAllChecking(server);
		  if(nbPlayerOnTable(server->game) == 0)
		    {
		      /* Reinitialisation des joueurs */
		      printf("----------------------------------------------------\n");
		      printf("Réinitialisation des données des joueurs...         \n");
		      printf("----------------------------------------------------\n\n");
		      server->actualPlayerPos = -1;
		      reinitRound(server->game);
		      continueWaitingLoop = 0;
		    }
				  
		  if(i == 10)
		    {
		      fprintf(stderr,"Waiting : %d ",i);
		    }
		  fprintf(stderr,"%d ",i);
		  sendToAllChecking(server);
		  sleep(1);
		  server->secondBeforeStart--;

		  sendToAllChecking(server);
		  sendToAllChecking(server);
		  if(nbPlayerOnTable(server->game) == 0)
		    {
		      /* Reinitialisation des joueurs */
		      printf("----------------------------------------------------\n");
		      printf("Réinitialisation des données des joueurs...         \n");
		      printf("----------------------------------------------------\n\n");
		      server->actualPlayerPos = -1;
		      reinitRound(server->game);
		      continueWaitingLoop = 0;
		    }
		}
	      server->isWaitingForStart = 0;
	      fprintf(stderr," END\n");

	      if(!continueWaitingLoop)
		{
		  printf("** Plus personne pendant l'attente ; Réinitialisation... **\n");
		  /* Reinitialisation des joueurs */
		  printf("----------------------------------------------------\n");
		  printf("Réinitialisation des données des joueurs...         \n");
		  printf("----------------------------------------------------\n\n");
		  server->actualPlayerPos = -1;
		  reinitRound(server->game);
		  continueWaitingLoop = 0;
		  continue;
		}

	      if(nbPlayerOnTable(server->game) == 0)
		{
		  printf("** Il n'y a pas de joueur pour la manche ; Abandon... **\n");
		  /* Reinitialisation des joueurs */
		  printf("----------------------------------------------------\n");
		  printf("Réinitialisation des données des joueurs...         \n");
		  printf("----------------------------------------------------\n\n");
		  server->actualPlayerPos = -1;
		  reinitRound(server->game);
		  continueWaitingLoop = 0;
		  continue;
		}

	      printf("----------------------------------------------------\n");
	      printf("Suppression des joueurs ne pouvant pas miser...     \n");
	      printf("----------------------------------------------------\n\n");
	      removePlayerUnableToBet(server);

	      /* Etape 2 : Au moins 1 joueur de présent */
	      if(nbPlayerOnTable(server->game) >= 1)
		{
		  /* Reinitialisation des joueurs */
		  printf("----------------------------------------------------\n");
		  printf("Réinitialisation des données des joueurs...         \n");
		  printf("----------------------------------------------------\n\n");
		  server->actualPlayerPos = -1;
		  reinitRound(server->game);
		  printGame(server->game);
		  
		  printf("----------------------------------------------------\n");
		  printf("Lancement de la partie...                           \n");
		  printf("----------------------------------------------------\n\n");
		  sendInfosTo(server,-1,-1,NULL);

		  printf("----------------------------------------------------\n");
		  printf("Distribution des jetons de participation...         \n");
		  printf("On procéde comme cela pour pouvoi vérifier si un... \n");
		  printf("  - joueur est actuellement en train de jouer...    \n");
		  printf("----------------------------------------------------\n\n");
		  tagPlayerAbleToRound(server->game);

		  printf("----------------------------------------------------\n");
		  printf("Brulage des %d premières cartes...                  \n",5);
		  printf("----------------------------------------------------\n\n");
		  burnFirstCards(server->game, 5);

		  printf("----------------------------------------------------\n");
		  printf("Démarrage du tour de mise - 10 secondes restantes...\n");
		  printf("Les joueurs disposent de 10 secondes pour miser...  \n");
		  printf("Si ces derniers n'ont pas miser, ils sont éjecter...\n");
		  printf("Dans la phase suivante, chaque mise est averti...   \n");
		  printf("  - aux autres joueurs...                           \n");
		  printf("----------------------------------------------------\n\n");

		  sendToAllChecking(server);
		  betTime(server,10);
		  
		  printf("----------------------------------------------------\n");
		  printf("Fin du tour de mise...                              \n");
		  printf("----------------------------------------------------\n\n");

		  /* Etape 6 : Ont tous bien misé ? */
		  printf("----------------------------------------------------\n");
		  printf("Suppression des joueurs n'ayant pas misé...         \n");
		  printf("Lorsqu'un joueur n'a pas misé, il est expulsé du... \n");
		  printf("  - du serveur et on averti chaque joueur de...     \n");
		  printf("  - la déconnexion de ce dernier du serveur...      \n");
		  printf("----------------------------------------------------\n\n");
		  if(checkingAndRemovingUnbet(server) > 0)
		    {
		      printf("\t** Certains joueurs ont été expulsé car pas de mise... **\n");
		    }

		  printf("A FINI DE CHECK LES UNBET\n");

		  if(nbPlayerOnTable(server->game) == 0)
		    {
		      printf("** Il n'y a pas de joueur pour la manche ; Abandon... **\n");
		      continue;
		    }
		  printf("Y A ENCORE DU MONDE\n");

		  
		  printGame(server->game);
		  
		  /* Etape 7 : Distribution des cartes */
		  printf("----------------------------------------------------\n");
		  printf("Distribution des cartes aux joueurs actifs de la... \n");
		  printf("  - manche...                                       \n");
		  printf("Distribution d'abord côté serveur puis ensuite...   \n");
		  printf("  - côté client en informant les joueurs de la...   \n");
		  printf("  - distribution effectuée...                       \n");
		  printf("----------------------------------------------------\n\n");

		  setFirstPlayerToPlay(server);
		  sendToAllChecking(server);
		  cardInitialDistribution(server);
		  printf("----------------------------------------------------\n");
		  printf("Distribution threadée côté serveur -- X secondes...  \n");
		  printf("----------------------------------------------------\n\n");
		  sleep((((nbPlayerOnTable(server->game) + 1) * 2) * 0.5f) + 0.5f);
		  printf("----------------------------------------------------\n");
		  printf("Fin distribution threadée...  \n");
		  printf("----------------------------------------------------\n\n");
		  
		  /* Etape 8 : les actions */
		 
		  sendToAllChecking(server);
		  if(nbPlayerOnTable(server->game) == 0)
		    {
		      server->actualPlayerPos = -1;
		      reinitRound(server->game);
		      printf("** Il n'y a pas de joueur pour la manche ; Abandon... **\n");
		      continue;
		    }
		  askingPlaysEveryPlayer(server);
		  printf("Fin asking plays\n");
		  
		  /* Tour du croupier */
		  printf("----------------------------------------------------\n");
		  printf("Tour du croupier jusqu'à >= à 17...                 \n");
		  printf("----------------------------------------------------\n\n");
		  sendToAllChecking(server);
		  if(nbPlayerOnTable(server->game) == 0)
		    {
		      server->actualPlayerPos = -1;
		      reinitRound(server->game);
		      printf("** Il n'y a pas de joueur pour la manche ; Abandon... **\n");
		      continue;
		    }
		  dealerTurn(server);
		  
		  /* Etape 9 : les gains */
		  printf("----------------------------------------------------\n");
		  printf("Distribution (ou non) des gains des joueurs...      \n");
		  printf("----------------------------------------------------\n\n");
		  sendToAllChecking(server);
		  if(nbPlayerOnTable(server->game) == 0)
		    {
		      server->actualPlayerPos = -1;
		      reinitRound(server->game);
		      printf("** Il n'y a pas de joueur pour la manche ; Abandon... **\n");
		      continue;
		    }
		  earningDistribution(server);

		  /* Etape 10 : On doit attendre 20 secondes pour relancer une partie */
		  /*printGame(server->game);*/
		  if(nbPlayerOnTable(server->game) == 0)
		    {
		      /* Reinitialisation des joueurs */
		      printf("----------------------------------------------------\n");
		      printf("Réinitialisation des données des joueurs...         \n");
		      printf("----------------------------------------------------\n\n");
		      server->actualPlayerPos = -1;
		      reinitRound(server->game);
		      printGame(server->game);
		    }
		  
		}
	    }
	}
      
      /* ============================ */
      /*   FERMETURE SERVEUR ET JEU   */
      /* ============================ */
      printf("======       PREPARING SERVER CLOSING      ======\n");
      freeServer(server);
      printf("======         SERVER CLOSING DONE         ======\n");
      printf("======   STOPPING THREAD CONNECTION DONE   ======\n");
      printf("======  STOPPING THREAD WANT TO PLAY DONE  ======\n");
      printf("====== STOPPING THREAD CONNECT CHECK DONE  ======\n");
      printf("======          GAME CLOSING DONE          ======\n");
      return 0;
    }
}
