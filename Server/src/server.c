#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <signal.h>
#include <poll.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#include "networkToolbox.h"
#include "server.h"
#include "error.h"

int stringIsDigital(char* string)
{
  unsigned int i;
  for(i = 0; i < strlen(string); i++)
    {
      if(!isdigit(string[i])) 
	return 0;
    }
  return 1;
}

int isFloat(const char *s, float *dest) {
  if (s == NULL) {
    return 0;
  }
  char *endptr;
  *dest = (float) strtod(s, &endptr);
  if (s == endptr) {
    return 0; // no conversion;
  }
  // Look at trailing text
  while (isspace((unsigned char ) *endptr))
    endptr++;
  return *endptr == '\0';
}

/*==================================*/
/*       SECTION DES ENVOIS         */
/*==================================*/
/*
 - Dans cette section, on va utiliser des fonctions qu'on pourra
 appeller partout pour réaliser les envois. L'avantage de ces
 fonctions est qu'elle s'occupe de tout, ainsi, on va pouvoir 
 les utiliser facilement.
 - Si "positionToSend" == -1 : on veut envoyer à tout le monde
   Sinon, on veut envoyer à positionToSend
 - Si "infoPositionToSend" == -1 : on veut envoyer toutes les infos
   Sinon, on veut envoyer les infos d'une position précise
 */

int isValidAnswerToAction(char* message, int currentPlayerBetAsked, BlackjackPlayer* player, Server* server)
{
  int itLine;
  char* tokenLine;
  
  char wantedType[10] = "POST";
  char wantedStartPath[20] = "/game/action/";
  char httpProto[20] = "HTTP/1.1";

  int itPath;
  char* path;

  char* position;

  char* datas;

  /* Geting first Line */
  itLine = 0;
  tokenLine = strtok(message,"\n");
  while(tokenLine != NULL)
    {
      char* save;
      if(itLine == 0)
	{
	  path = tokenLine;
	}
      itLine++;
      save = tokenLine;
      tokenLine = strtok(NULL,"\n");
      /* Dernier ligne, ligne des donnes */
      if(tokenLine == NULL)
	{
	  datas = save;
	}
    }

  /* Getting ok first line */
  itPath = 0;
  path = strtok(path," ");
  while(path != NULL)
    {
      if(itPath == 0 && strcmp(path,wantedType) != 0)
	{
	  return 0;
	}
      
      if(itPath == 1)
	{
	  if(strncmp(path,wantedStartPath,strlen(wantedStartPath)) != 0)
	    {
	      return 0;
	    }
	  else
	    {
	      position = path;
	    }
	}
      
      if(itPath == 2 && strcmp(path,httpProto) != 0)
	{
	  return 0;
	}
      
      itPath++;
      path = strtok(NULL," ");
    }

  /* Checking bet is according to bet asked */
  position = strtok(position,"/");
  while(position != NULL)
    {
      char* save;
      save = position;
      position = strtok(NULL,"/");
      if(position == NULL)
	{
	  /* We find the integer representing player */
	  if(!stringIsDigital(save))
	    {
	      return 0;
	    }
	  else if(atoi(save) != currentPlayerBetAsked)
	    {
	      printf("NOT CURRENT PLAYER\n");
	      return 0;
	    }
	}
    }

  /* Everything is ok we should get the data */
  unsigned int i;
  int itKey = 0;
  int itData = 0;
  int isReadingData = 0;
  char key[20] = "";
  char data[40] = "";

  for(i = 0; i < strlen(datas); i++)
    {
      if(datas[i] == '=')
	{
	  isReadingData = 1;
	  continue;
	}

      if(!isReadingData)
	{
	  key[itKey++] = datas[i];
	}
      else
	{
	  data[itData++] = datas[i];
	}
    }
  key[itKey++] = '\0';
  data[itData++] = '\0';

  if(strcmp(key,"action") != 0 || strcmp(data,"") == 0)
    {
      printf("STRANGE PARSE\n");
      return 0;
    }
  else
    {
      /* On doit regarder que l'action est valide */
      if(strcmp(data,"hit") == 0)
	{
	  return !playerHitCard(server->game,player->positionOnTable);
	}
      else if(strcmp(data,"surrend") == 0)
	{
	  return !playerSurrend(server->game,player->positionOnTable);
	}
      else if(strcmp(data,"stand") == 0)
	{
	  return !playerStand(server->game,player->positionOnTable);
	}
      else if(strcmp(data,"double") == 0)
	{
	  return !playerDouble(server->game,player->positionOnTable);
	}
      else if(strcmp(data,"split") == 0)
	{
	  return !playerSplit(server->game,player->positionOnTable);
	}
      else
	{
	  return 0;
	}
    }
}

int isValidAnswerToBet(char* message, int currentPlayerBetAsked, BlackjackPlayer* player)
{
  int itLine;
  char* tokenLine;
  
  char wantedType[10] = "POST";
  char wantedStartPath[20] = "/game/bet/";
  char httpProto[20] = "HTTP/1.1";

  int itPath;
  char* path;

  char* position;

  char* datas;

  /* Geting first Line */
  itLine = 0;
  tokenLine = strtok(message,"\n");
  while(tokenLine != NULL)
    {
      char* save;
      if(itLine == 0)
	{
	  path = tokenLine;
	}
      printf("Line = %s\n",tokenLine);
      itLine++;
      save = tokenLine;
      tokenLine = strtok(NULL,"\n");
      /* Dernier ligne, ligne des donnes */
      if(tokenLine == NULL)
	{
	  datas = save;
	}
    }

  /* Getting ok first line */
  itPath = 0;
  path = strtok(path," ");
  while(path != NULL)
    {
      if(itPath == 0 && strcmp(path,wantedType) != 0)
	{
	  return 0;
	}
      
      if(itPath == 1)
	{
	  if(strncmp(path,wantedStartPath,strlen(wantedStartPath)) != 0)
	    {
	      return 0;
	    }
	  else
	    {
	      position = path;
	    }
	}
      
      if(itPath == 2 && strcmp(path,httpProto) != 0)
	{
	  return 0;
	}
      
      itPath++;
      path = strtok(NULL," ");
    }

  /* Checking bet is according to bet asked */
  position = strtok(position,"/");
  while(position != NULL)
    {
      char* save;
      save = position;
      position = strtok(NULL,"/");
      if(position == NULL)
	{
	  /* We find the integer representing player */
	  if(!stringIsDigital(save))
	    {
	      return 0;
	    }
	  else if(atoi(save) != currentPlayerBetAsked)
	    {
	      return 0;
	    }
	}
    }

  /* Everything is ok we should get the data */
  unsigned int i;
  int itKey = 0;
  int itData = 0;
  int isReadingData = 0;
  char key[20] = "";
  char data[40] = "";

  for(i = 0; i < strlen(datas); i++)
    {
      if(datas[i] == '=')
	{
	  isReadingData = 1;
	  continue;
	}

      if(!isReadingData)
	{
	  key[itKey++] = datas[i];
	}
      else
	{
	  data[itData++] = datas[i];
	}
    }

  
  if(strcmp(key,"bet") != 0 || strcmp(data,"") == 0)
    {
      return 0;
    }
  else
    {
      float value;
      if(!isFloat(data,&value))
	{
	  return 0;
	}

      /* On essaye de faire la mise */
      if(setBetBlackjackPlayer(player,value) != 0)
	{
	  printf("Wrong bet ! Value is too high\n");
	  return 0;
	}
    }
  
  return 1;
}

void sendMessageActionPossible(Server* server, MessageType type, int position)
{
  BlackjackPlayer* player = getBlackjackPlayerFromPosition(server->game->table, position);
  if(player != NULL)
    {
      char seat[15];
      char* message;
      char* finalMessage;

      sprintf(seat,"seat %d",player->positionOnTable);
      message = buildPlayerPossibleAction(player);
      finalMessage = buildMessageHeader(type,"game/action/possible",seat,server->IPAddr,message);
      if(sendDataTCP(server->listSockets[player->positionOnTable],finalMessage, server) < 0)
	{
	  fprintf(stderr,"Error at sending data TCP\n");
	}

      free(message);
      free(finalMessage);
    }
}

void sendInformationMessage(Server* server, char* message)
{
  char path[40];
  int i;

  sprintf(path,"%s","game/message");

  for(i = 0; i < server->game->table->maxPlayer; i++)
    {
      BlackjackPlayer* player = server->game->table->players[i];
      if(player != NULL)
	{
	  sendMessageTo(server,GIVE_INFORMATIONS,player->positionOnTable,path, message);
	}
    }
}

void sendMessageTo(Server* server, MessageType type, int positionToSend, char* path, char* message)
{
  BlackjackPlayer* player = getBlackjackPlayerFromPosition(server->game->table,positionToSend);
  if(player != NULL)
    {
      char seat[15];
      char* finalMessage;

      sprintf(seat,"seat %d",player->positionOnTable);
      finalMessage = buildMessageHeader(type,path,seat,server->IPAddr,message);
      if(sendDataTCP(server->listSockets[player->positionOnTable],finalMessage, server) < 0)
	{
	  fprintf(stderr,"Error at sending data TCP\n");
	}

      free(finalMessage);
    }
}

void sendDeconnectionTo(Server* server, int positionToSend, int positionDeconnected, char* message)
{
  char datas[60];
  char path[30];

  datas[0] = '\0';
  sprintf(path,"game/disconnection/%d",positionDeconnected);
  /*sprintf(datas,"[next-player=%d:reason=%s]",getNextPlayerPositionAtDeconnection(server,positionDeconnected),message);*/
  sprintf(datas,"[reason=%s]",message);
  
  if(positionToSend == -1)
    {
      int i;
      for(i = 0; i < server->game->table->maxPlayer; i++)
	{
	  BlackjackPlayer* player = server->game->table->players[i];
	  if(player != NULL)
	    {
	      int position = player->positionOnTable;
	      sendMessageTo(server,GIVE_INFORMATIONS,position,path,datas);
	    }
	}
    }
  else
    {
      BlackjackPlayer* player = getBlackjackPlayerFromPosition(server->game->table,positionToSend);
      int position = player->positionOnTable;
      sendMessageTo(server,GIVE_INFORMATIONS,position,path,datas);
    }
}

void sendActualSeat(Server* server, int seatGonnaPlay)
{
  int i;
  char path[30];

  sprintf(path,"game/actual-seat/%d",seatGonnaPlay);
  for(i = 0; i < server->game->table->maxPlayer; i++)
    {
      BlackjackPlayer* player = server->game->table->players[i];
      if(player != NULL)
	{
	  sendMessageTo(server, GIVE_INFORMATIONS,player->positionOnTable,path,"");
	}
    }
}

void sendInfosToConnection(Server* server, int socket, int futurPosition, char* futurName)
{
  int status;
  BlackjackPlayer* futurPlayer = constructBlackjackPlayer(&status,futurPosition,futurName,server->game->startMoney);
  char* datas = NULL;
  char* additionalData = NULL;
  char* finalDatas = NULL;
  char path[50];
  char seat[15];
  char* finalMessage = NULL;
      
  sprintf(path,"%s","game/init");
  sprintf(seat,"seat %d",futurPlayer->positionOnTable);

  datas = prepareMessageInfoEveryPlayer(server);
  additionalData = buildPlayerInformations(futurPlayer);
  
  datas = realloc(datas, (strlen(datas) + 300) * sizeof(char));
  strcat(datas,additionalData);

  finalDatas = finalizeMsgInfo(server,datas,futurPlayer->positionOnTable);
  finalMessage = buildMessageHeader(GIVE_INFORMATIONS,path,seat,server->IPAddr,finalDatas);

  if(sendDataTCP(socket,finalMessage, server) < 0)
    {
      fprintf(stderr,"Error at sending data TCP\n");
    }

  if(datas != NULL)
    {
      free(datas);
    }
  if(additionalData != NULL)
    {
      free(additionalData);
    }
  if(finalDatas != NULL)
    {
      free(finalDatas);
    }
  if(finalMessage != NULL)
    {
      free(finalMessage);
    }
  freeBlackjackPlayer(futurPlayer);
}

void sendInfosTo(Server* server, int positionToSend, int infoPositionToSend, char* pathGiven)
{
  char* datas = NULL;
  char* finalDatas = NULL;
  char path[50];
  
  /* On veut envoyer toutes les informations */
  if(infoPositionToSend == -1)
    {
      sprintf(path,"game/infos-player");
      datas = prepareMessageInfoEveryPlayer(server);
    }
  /* On veut envoyer seulement l'info d'un joueur */
  else
    {
      sprintf(path,"game/infos-player/%d",infoPositionToSend);
      datas = prepareMessageInfoSinglePlayer(server,infoPositionToSend);
    }
  
  if(pathGiven != NULL)
    {
      strcpy(path,pathGiven);
    }
  
  if(positionToSend == -1)
    {
      int i;
      for(i = 0; i < server->game->table->maxPlayer; i++)
	{
	  BlackjackPlayer* player = server->game->table->players[i];
	  if(player != NULL)
	    {
	      int position = player->positionOnTable;
	      finalDatas = finalizeMsgInfo(server,datas,position);
	      sendMessageTo(server,GIVE_INFORMATIONS,position,path,finalDatas);
	    }
	}
    }
  else
    {
      BlackjackPlayer* player = getBlackjackPlayerFromPosition(server->game->table,positionToSend);
      int position = player->positionOnTable;
      finalDatas = finalizeMsgInfo(server,datas,position);
      sendMessageTo(server,GIVE_INFORMATIONS,position,path,finalDatas);
    }

  if(datas != NULL)
    {
      free(datas);
    }
  if(finalDatas != NULL)
    {
      free(finalDatas);
    }
}

void sendEarnings(Server* server, char* message)
{
  int i;
  char path[20];

  if(strcmp(message,"") != 0)
    {
      sprintf(path,"%s","game/earnings");
      for(i = 0; i < server->game->table->maxPlayer; i++)
	{
	  BlackjackPlayer* player = server->game->table->players[i];
	  if(player != NULL)
	    {
	      sendMessageTo(server,GIVE_INFORMATIONS, player->positionOnTable,path,message);
	    }
	}
    }
}

void sendGameStartIn(Server* server, int positionToSend)
{
  int i;
  char path[20];
  char message[20];
  sprintf(path,"%s","game/start");
  sprintf(message,"[startCounter=%d]",server->secondBeforeStart);

  if(positionToSend == -1)
    {
      for(i = 0; i < server->game->table->maxPlayer; i++)
	{
	  BlackjackPlayer* player = server->game->table->players[i];
	  if(player != NULL)
	    {
	      sendMessageTo(server,GIVE_INFORMATIONS,player->positionOnTable,path,message);
	    }
	}
    }
  else
    {
      sendMessageTo(server,GIVE_INFORMATIONS,positionToSend,path,message);
    }
}

void sendResponseAgreement(Server* server, int positionToSend, int agree)
{
  char path[30];
  char message[200];
  char date[100];
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  
  sprintf(date,"%d-%d-%d %d:%d:%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

  if(!agree)
    {
      sprintf(path,"HTTP/1.1 %d %s",403, "FORBIDDEN");
    }
  else
    {
      sprintf(path,"HTTP/1.1 %d %s",202,"ACCEPTED");
    }
  
  sprintf(message,"%s\n"
	          "Date: %s\n"
	          "Content-length: %d\n\n"
	          "%s",
	  path,
	  date,
	  0,
	  "");

  if(sendDataTCP(server->listSockets[positionToSend], message, server) < 0)
    {
      fprintf(stderr,"Error at sending TCP\n");
    }
	  
}

int addClientToList(Server* server, int socket, char* name)
{
  int i;

  for(i = 0; i < MAX_PLAYER_SERVER; i++)
    {
      if(server->listSockets[i] == -1)
	{
	  server->listSockets[i] = socket;
	  newPlayerJoinGame(server->game, name, i);
	  return 0;
	}
    }

  return NO_SPACE_ON_SERVER_ERROR;
}

int removeClientFromList(Server* server, int position)
{
  server->listSockets[position] = -1;
  return playerLeftGame(server->game,position);
}


Server* constructServer(int* status, int portBroad, int portConnection, int nbDeck, float startMoney, float betMin, float betMax)
{
  /* Variables */
  Server* server;

  /* Instanciation */
  server = calloc(1,sizeof(Server));
  if(server == NULL)
    {
      *status = ALLOCATION_ERROR;
      return NULL;
    }

  /* Instanciation game */
  server->game = constructGame(status,nbDeck, MAX_PLAYER_SERVER, startMoney, betMin, betMax);
  if(server->game == NULL)
    {
      freeServer(server);
      return NULL;
    }
  
  /* Initialisation */
  if((*status=initServer(server,portBroad,portConnection,nbDeck, startMoney, betMin, betMax)) != 0)
    {
      freeServer(server);
      return NULL;
    }

  *status = 0;
  return server;
}

int initServer(Server* server, int portBroad, int portConnection, int nbDeck, float startMoney, float betMin, float betMax)
{
  int i;
  int status;

  struct ifaddrs *ifap, *ifa;
  struct sockaddr_in *sa;
  char *addr;

  server->actualPlayerPos = -1;
  server->isRunning = 1;
  server->portBroadcast = portBroad;
  server->portConnection = portConnection;
  server->nbDeck = nbDeck;
  server->startMoney = startMoney;
  server->betMin = betMin;
  server->betMax = betMax;
  server->IPAddr = NULL;
  server->secondBeforeStart = 10;

  /* Opening */
  if((server->listenSocket = openSocketUDP()) == -1)
    {
      return OPEN_ERROR;
    }

  if((server->enterSocket = openSocketTCP()) == -1)
    {
      return OPEN_ERROR;
    }

  /* Binding */
  if(bindUDP(server->listenSocket,server->portBroadcast) != 0)
    {
      return BIND_ERROR;
    }

  if(bindTCP(server->enterSocket,server->portConnection) != 0)
    {
      return BIND_ERROR;
    }

  /* Retriving address */
  
  getifaddrs (&ifap);
  for (ifa = ifap; ifa; ifa = ifa->ifa_next)
    {
      if (ifa->ifa_addr->sa_family==AF_INET)
	{
	  sa = (struct sockaddr_in *) ifa->ifa_addr;
	  addr = inet_ntoa(sa->sin_addr);
	  if(strcmp(ifa->ifa_name,"lo") != 0)
	    {
	      server->IPAddr = calloc(strlen(addr)+1,sizeof(char));
	      if(server->IPAddr == NULL)
		{
		  freeifaddrs(ifap);
		  return ALLOCATION_ERROR;
		}
	      strcpy(server->IPAddr,addr);
	    }
	}
    }
  
  freeifaddrs(ifap);

  /* Initialise player */
  for(i = 0; i < MAX_PLAYER_SERVER; i++)
    {
      server->listSockets[i] = -1;
    }

  /* Launching thread */
  if((status=createForkWantToPlay(server)) != 0)
    {
      return status;
    }
  if((status=createForkWaiterConnection(server)) != 0)
    {
      return status;
    }
  
  return 0;
}

void freeServer(Server* server)
{
  int i;
  server->isRunning = 0;
  
  if(pthread_join(server->connection, NULL) < 0)
    {
      perror("Error at waiting thread connection : ");
    }
  
  if(pthread_join(server->wantPlay, NULL) < 0)
    {
      perror("Error at waiting thread want play\n");
    }
  
  closeSocketTCP(server->enterSocket);
  closeSocketUDP(server->listenSocket);

  /* Deconnection des différentes joueurs */
  for(i = 0; i < server->game->table->maxPlayer; i++)
    {
      BlackjackPlayer* player = server->game->table->players[i];
      if(player != NULL)
	{
	  sendDeconnectionTo(server,player->positionOnTable,player->positionOnTable,"end");
	  removeClientFromList(server,player->positionOnTable);
	}
    }
  
  if(server->IPAddr != NULL)
    {
      free(server->IPAddr);
    }
  if(server->game != NULL)
    {
      freeGame(server->game);
    }
  
  free(server);
}

void setFirstPlayerToPlay(Server* server)
{
  int i;
  for(i = 0; i < server->game->table->maxPlayer; i++)
    {
      BlackjackPlayer* player = server->game->table->players[i];
      if(player != NULL && player->isPlayingOnRound)
	{
	  server->actualPlayerPos = player->positionOnTable;
	  
	}
    }
}

int getNextPlayerPositionAtDeconnection(Server* server, int playerDeconnected)
{
  if(server->actualPlayerPos == playerDeconnected)
    {
      int i;
      for(i = playerDeconnected; i < server->game->table->maxPlayer; i++)
	{
	  BlackjackPlayer* player = server->game->table->players[i];
	  if(player != NULL && player->isPlayingOnRound)
	    {
	      return player->positionOnTable;
	    }
	}

      return -1;
    }
  return server->actualPlayerPos;
}

int getClientPositionFromSocketID(Server* server, int ID)
{
  int i;
  for(i = 0; i < MAX_PLAYER_SERVER; i++)
    {
      if(server->listSockets[i] == ID)
	{
	  return i;
	}
    }
  return -1;
}

void* scriptResponseWantToPlay(void* arg)
{
  Server* server = (Server*)arg;
  int socketResponse;
  char* message = NULL;
  char* response = NULL;
  char* IPClient = NULL;

  /* Receiving message */
  if(receiveUDPMessage(server->listenSocket, server->portBroadcast,&message,&IPClient) < 0)
    {
      fprintf(stderr,"Error at receiveUDPMessage\n");
    }
  /* Analyse and answer */
  if(strcmp(message,"I WANT TO PLAY ANDNIC") == 0)
    {
      printf("Client with IP \"%s\" has send message \"%s\"\n",IPClient, message);
      socketResponse = openSocketUDP();
      if(socketResponse == -1)
	{
	  fprintf(stderr,"Error at opening socket UDP\n");
	  pthread_exit(0);
	}
      else
	{
	  response = createResponseToWantToPlay(server);
	  if(response == NULL)
	    {
	      fprintf(stderr,"Error at creating response\n");
	    }
	  else
	    {
	      if(nbPlayerOnTable(server->game) < 7)
		{
		  printf("Sending = %d\n",sendUDPMessage(socketResponse,IPClient,2303,response));
		}
	      free(response);
	    }
	}
    }
  else
    {
      fprintf(stderr,"Wrong message, trying intrusion\n");
    }

  if(message != NULL) free(message);
  if(IPClient != NULL) free(IPClient);
  pthread_exit(0);
}

void* scriptWantToPlay(void* arg)
{
  Server* server = arg;
  
  /* On va mettre en place un système de poll,
     c'est à dire que notre thread fait une attente passive
     sous réserve d'un certain délai, si il recoit un event
     alors on sait qu'il vient de recevoir une demande de connexion
     dans ce cas, on peut réaliser le accept. De cette façon, si on recoit
     un sigkill qui met à jour la condition, on va pouvoir sortir de notre boucle */

  /* Lors d'une demande de serveur, c'est un autre thread qui va gérer la reponse au client
     pour ne pas bloquer la réception des nouvelles demandes, on va donc créer
     un thread en mode détaché */
  pthread_attr_t threadAttr;
  if(pthread_attr_init(&threadAttr) != 0)
    {
      fprintf(stderr,"Error at pthread_attr_init\n");
    }
  if(pthread_attr_setdetachstate(&threadAttr,PTHREAD_CREATE_DETACHED) != 0)
    {
      fprintf(stderr,"Error at pthread_attr_setdetachstate\n");
    }

  while(server->isRunning)
    {
      int pollVar = -1;
     
      struct pollfd ufds[1];
      ufds[0].fd = server->listenSocket;
      ufds[0].events = POLLIN;
      ufds[0].revents = 0;
  
      pollVar = poll(ufds,1,2000);
      if(pollVar != 0)
	{
          if(ufds[0].revents && POLLIN)
	    {
	      pthread_t thResponse;
	      if(pthread_create(&thResponse,&threadAttr,scriptResponseWantToPlay,server) < 0)
		{
		  fprintf(stderr,"pthread_create for want to play response error\n");
		}
	    }
	}
    }

  pthread_exit(0);
}

void* scriptConnectionResponse(void* arg)
{
  ConnectStruct* connect = (ConnectStruct*)arg;
  BlackjackPlayer* player;
  char* receiveName;
  char* correctName;
  int futurPosition;
  int status;

  printf("Client with SOCKET IDENTIFICATOR %d is connected\n",connect->clientFDSocket);
  
  if((receiveName=receiveTCPMessage(connect->clientFDSocket,&status)) == NULL)
    {
      fprintf(stderr,"Error at receiving TCP message -- error %d\n",status);
    }

  correctName = newPossibleName(connect->serverRef->game,receiveName);
  futurPosition = getNextPositionPossible(connect->serverRef);
  
  sendInfosToConnection(connect->serverRef,connect->clientFDSocket,futurPosition,correctName);
  addClientToList(connect->serverRef,connect->clientFDSocket,correctName);
  
  player = getBlackjackPlayerFromName(connect->serverRef->game->table, correctName);
  /* On envoie la donnée du nouveau joueur à tout les joueurs */
  sendInfosTo(connect->serverRef,-1,player->positionOnTable,NULL);
  
  if(connect->serverRef->isWaitingForStart)
    {
      sendGameStartIn(connect->serverRef,player->positionOnTable);
    }
  
  /* Freeing variables */
  free(receiveName);
  free(correctName);
  
  pthread_exit(0);
}

int getNextPositionPossible(Server* server)
{
  int i;
  for(i = 0; i < MAX_PLAYER_SERVER; i++)
    {
      if(server->listSockets[i] == -1)
	{
	  return i;
	}
    }
  return -1;
}

void* scriptConnectionWaiter(void* arg)
{
  Server* server = arg;

  /* On va mettre en place un système de poll,
     c'est à dire que notre thread fait une attente passive
     sous réserve d'un certain délai, si il recoit un event
     alors on sait qu'il vient de recevoir une demande de connexion
     dans ce cas, on peut réaliser le accept. De cette façon, si on recoit
     un sigkill qui met à jour la condition, on va pouvoir sortir de notre boucle */

  /* Lors d'une connexion, c'est un autre thread qui va gérer la response au client
     pour ne pas bloquer la réception des nouvelles connexions, on va donc créer
     un thread en mode détaché */
  pthread_attr_t threadAttr;
  if(pthread_attr_init(&threadAttr) != 0)
    {
      fprintf(stderr,"Error at pthread_attr_init\n");
    }
  if(pthread_attr_setdetachstate(&threadAttr,PTHREAD_CREATE_DETACHED) != 0)
    {
      fprintf(stderr,"Error at pthread_attr_setdetachstate\n");
    }
  
  while(server->isRunning)
    {
      int client;
      int pollVar = -1;

      struct pollfd ufds[1];
      ufds[0].fd = server->enterSocket;
      ufds[0].events = POLLIN;
      ufds[0].revents = 0;
      
      pollVar = poll(ufds,1,2000);
      if(pollVar != 0)
	{
	  if(ufds[0].revents && POLLIN)
	    {
	      ConnectStruct conAnsw;
	      pthread_t thResponse;
	      
	      if((client=waitTCP(server->enterSocket)) == -1)
		{
		  fprintf(stderr,"Error at wait TCp\n");
		}

	      conAnsw.serverRef = server;
	      conAnsw.clientFDSocket = client;

	      if(nbPlayerOnTable(server->game) < 7)
		{
		  if(pthread_create(&thResponse,&threadAttr,scriptConnectionResponse,&conAnsw) < 0)
		    {
		      fprintf(stderr,"phtread_create for connection response error\n");
		    }
		}
	    }
	}
    }

  pthread_exit(0);
}

int createForkWantToPlay(Server* server)
{
  if(pthread_create(&server->wantPlay,NULL,scriptWantToPlay,server) == -1)
    {
      return THREAD_ERROR;
    }

  return 0;
}

int createForkWaiterConnection(Server* server)
{
  if(pthread_create(&server->connection,NULL, scriptConnectionWaiter, server) == -1)
    {
      return THREAD_ERROR;
    }

  return 0;
}

char* createResponseToWantToPlay(Server* server)
{
  char* sentence = calloc(MAX_SIZE_RESPONSE_WANT_TO_PLAY, sizeof(char));
  int res;
  
  res = sprintf(sentence,"COME HERE TO HAVE FUN - %s:%d {%d:%d:%f:%f:%f}", server->IPAddr,server->portConnection, nbPlayerOnTable(server->game), server->nbDeck, server->startMoney, server->betMin, server->betMax);

  if(res < 0)
    {
      free(sentence);
      return NULL;
      /* QUAND JE QUITTE ALORS QU'IL REPOND */
    }

  return sentence;
}

void printServer(const Server* server)
{
  int i;

  printf("================================================\n");
  printf("Server is open \n");
  printf("Server address: %s\n", server->IPAddr);
  printf("Listening for broadcast on port : %d\n",server->portBroadcast);
  printf("Listening for connection on port : %d\n",server->portConnection);
  printf("Here is the map of connection established : \n");

  for(i = 0; i < MAX_PLAYER_SERVER ; i++)
    {
      printf("At room %d -- IDENTIFIER SOCKET : %d\n",i+1,server->listSockets[i]);
    }
  
  printf("================================================\n");
}

int removePlayerUnableToBet(Server* server)
{
  int i;
  int* toRemove;
  int current = 0;

  toRemove = calloc(server->game->table->maxPlayer,sizeof(int));
  if(toRemove == NULL)
    {
      return ALLOCATION_ERROR;
    }
  
  /* Enregistrement pour enlever */
  for(i = 0; i < server->game->table->maxPlayer; i++)
    {
      BlackjackPlayer* focus = server->game->table->players[i];
      if(focus != NULL && !canBetAtMinimumBlackjackPlayer(focus,server->betMin))
	{
	  toRemove[current++] = focus->positionOnTable;
	}
    }

  /* Suppression */
  for(i = 0; i < current; i++)
    {
      printf("passage\n");
      sendDeconnectionTo(server,-1, toRemove[i],"money");
      removeClientFromList(server,toRemove[i]);
    }

  free(toRemove);
  return 0;
}

char* prepareMessageInfoSinglePlayer(Server* server, int position)
{
  int i;
  char* playerInfo;

  if(position == 7)
    {
      playerInfo = buildPlayerInformations(server->game->table->dealer);
    }
  else
    {
      for(i = 0; i < server->game->table->maxPlayer; i++)
	{
	  BlackjackPlayer* player = server->game->table->players[i];
	  if(player != NULL && player->positionOnTable == position)
	    {
	      playerInfo = buildPlayerInformations(player);
	    }
	}
    }

  return playerInfo;
}

char* prepareMessageInfoEveryPlayer(Server* server)
{
  int i;
  int currentPlayerInfo;
  char** playersInfos;
  char* finalMessage;

  currentPlayerInfo = 0;
  playersInfos = calloc(server->game->table->maxPlayer + 1, sizeof(char*));
  finalMessage = calloc(300 * (server->game->table->maxPlayer + 1), sizeof(char));
  finalMessage[0] = '\0';

  playersInfos[currentPlayerInfo++] = buildPlayerInformations(server->game->table->dealer);
  for(i = 0; i < server->game->table->maxPlayer; i++)
    {
      BlackjackPlayer* player = server->game->table->players[i];
      if(player != NULL)
	{
	  char* message = buildPlayerInformations(player);
	  playersInfos[currentPlayerInfo++] = message;
	}
    }

  for(i = 0; i < currentPlayerInfo; i++)
    {
      strcat(finalMessage,playersInfos[i]);
    }

  for(i = 0; i < currentPlayerInfo; i++)
    {
      free(playersInfos[i]);
    }
  free(playersInfos);
  return finalMessage;
}

char* finalizeMsgInfo(Server* server, char* message, int position)
{
  char* finalize = calloc(strlen(message) + 60, sizeof(char));
  char actualPlayer[15];
  char actualHand[15];
  char positionCurrent[15];

  strcpy(finalize,message);
  sprintf(actualPlayer,"|actualSit=%d",server->actualPlayerPos);
  if(currentHandOfCurrentPlayer(server) == -1)
    {
      sprintf(actualHand,"|actualHand=%d",0);
    }
  else
    {
      sprintf(actualHand,"|actualHand=%d",currentHandOfCurrentPlayer(server));
    }
  sprintf(positionCurrent,"|yours=%d",position);
  /* We need to set datas in message as playerCurrent and tag of the player */
  strcat(finalize,actualPlayer);
  strcat(finalize,actualHand);
  strcat(finalize,positionCurrent);

  return finalize;
}

int currentHandOfCurrentPlayer(Server* server)
{
  int i,j;
  for(i = 0; i < server->game->table->maxPlayer; i++)
    {
      BlackjackPlayer* player = server->game->table->players[i];
      if(player != NULL && player->positionOnTable == server->actualPlayerPos)
	{
	  for(j = 0; j < player->currentHand; j++)
	    {
	      if(!(player->hands[j]->handIsDone))
		{
		  return j;
		}
	    }
	}
    }
  
  return -1;
}

void* betThread(void* arg)
{
  BetStruct* bet = (BetStruct*)arg;
  Server* server = bet->serverRef;
  int playerPosition = bet->playerPosition;
  int betTime = bet->betTime;
  BlackjackPlayer* player = getBlackjackPlayerFromPosition(server->game->table,playerPosition);

  char seatString[8];
  char* messageToSend;
  
  struct pollfd ufds[1];
  int pollVar = -1;
  int okGoOn = 0;
  
  ufds[0].fd = bet->serverRef->listSockets[playerPosition];
  ufds[0].events = POLLIN;
  ufds[0].revents = 0;

  sprintf(seatString,"Seat %d",playerPosition);
  messageToSend = buildMessageHeader(ASK_INFORMATIONS,"game/bet",seatString,bet->serverRef->IPAddr,"");

  while(!okGoOn)
    {
      /* Sending message */
      if(sendDataTCP(server->listSockets[playerPosition],messageToSend, server) < 0)
	{
	  perror("Error at sending data TCP in betThread\n");
	}
  
      /* Waiting message (10 seconds) */
      pollVar = poll(ufds,1,betTime*1000);
  
      /* player has answered */
      if(pollVar > 0)
	{
	  int status;
	  char* answer;
	  if((answer = receiveTCPMessage(server->listSockets[playerPosition],&status)) == NULL)
	    {
	      fprintf(stderr,"Error at receive\n");
	      removeClientFromList(server,playerPosition);
	      sendDeconnectionTo(server,-1,playerPosition, "general");
	      free(messageToSend);
	      pthread_exit(0);
	    }

	  if(isValidAnswerToBet(answer,playerPosition,player))
	    {
	      /* Send accept to player */
	      sendResponseAgreement(server,playerPosition,1);
	  
	      /* Send bet value to all */
	      sendInfosTo(server,-1,playerPosition, NULL);

	      okGoOn = 1;
	    }
	  else
	    {
	      sendResponseAgreement(server,playerPosition,0);
	    }
      
	  free(answer);
	}
      /* Error occured */
      else if(pollVar < 0)
	{
	  perror("Error in poll : ");
	}
      else if(pollVar == 0)
	{
	  okGoOn = 1;
	}
    }
  
  free(messageToSend);
  pthread_exit(0);
}

int betTime(Server* server, int maxSecond)
{
  int i;

  BetStruct bets[MAX_PLAYER_SERVER];
  pthread_t threads[MAX_PLAYER_SERVER];
  int currentThread = 0;
  int currentBet = 0;
  
  /* Starting all threads */
  for(i = 0; i < server->game->table->maxPlayer; i++)
    {
      BlackjackPlayer* player = server->game->table->players[i];
      if(player != NULL && player->isPlayingOnRound)
	{
	  bets[currentBet].serverRef = server;
	  bets[currentBet].playerPosition = player->positionOnTable;
	  bets[currentBet].betTime = maxSecond;
	  
	  printf("-- Launch thread bet for player %d --",player->positionOnTable);
	  if(pthread_create(&threads[currentThread++],NULL,betThread,&bets[currentBet]) < 0)
	    {
	      perror("Error at pthread create : ");
	      return 1;
	    }
	  
	  currentBet++;
	}
    }

  /* Waiting all thread */
  for(i = 0; i < currentThread; i++)
    {
      pthread_join(threads[i],0);    
    }
  
  printf("-- All thread bet has finish --\n");
  return 0;
}

int checkingAndRemovingUnbet(Server* server)
{
  int hasRemove = 0;
  int i;

  int toRemove[MAX_PLAYER_SERVER];
  int currentToRemove = 0;

  /* Détection des joueurs à éjecter */
  for(i = 0; i < server->game->table->maxPlayer; i++)
    {
      BlackjackPlayer* player = server->game->table->players[i];
      if(player != NULL && player->isPlayingOnRound && !playerHasBet(player))
	{
	  toRemove[currentToRemove++] = player->positionOnTable;
	}
    }

  /* Ejection des joueurs */
  for(i = 0; i < currentToRemove; i++)
    {
      printf("Player at position %d on table has not bet -- Expulsing him\n",toRemove[i]);
      sendDeconnectionTo(server,-1,toRemove[i], "bet");
      removeClientFromList(server,toRemove[i]);
      hasRemove = 1;
    }
  
  return hasRemove;
}

int cardInitialDistribution(Server* server)
{
  /* Distribution côté serveur */
  if(initialCardDistribution(server->game) != 0)
    {
      fprintf(stderr,"Error during initial card distribution\n");
      return 1;
    }

  /* On envoie toutes les données à tout les gens de la table */
  sendInfosTo(server,-1,-1, "game/firstDistribution");
  
  return 0;
}

void* threadCheckConnection(void* arg)
{
  CheckingStruct* check = (CheckingStruct*)arg;
  int i;

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  for(i = 0; i < 10; i++)
    {
      sendToAllCheckingExceptPlaying(check->serverRef,check->positionExecuting);
      sleep(1);
      pthread_testcancel();
    }

  pthread_exit(0);
}

int askingPlays(Server* server, int position, BlackjackPlayer* player)
{
  if(player != NULL)
    {
      int status = 0;
      int goOn = 1;
      char seat[7];

      struct pollfd ufds[1];
      int pollVar = -1;
      
      ufds[0].fd = server->listSockets[position];
      ufds[0].events = POLLIN;
      ufds[0].revents = 0;
  
      sprintf(seat,"seat %d",position);
  
      while(goOn)
	{
	  char messageOfSurrend[70];
	  char* answer;
	  char* message;
	  char* messageInfo;
	  pthread_t thCheck;
	  CheckingStruct checking;
	  checking.serverRef = server;
	  checking.positionExecuting = player->positionOnTable;
	  

	  /* What do he want to play ? */
	  message = buildMessageHeader(ASK_INFORMATIONS,"game/action/ask",seat,server->IPAddr,"\0");

	  /*printf("Want to send : %s\n",message);*/
	  if(sendDataTCP(server->listSockets[position],message, server) < 0)
	    {
	      fprintf(stderr,"Error at sending what you want play\n");
	    }
	  free(message);

	  /* Here's are your actions */
	  sendMessageActionPossible(server,GIVE_INFORMATIONS,player->positionOnTable);

	  /* Thread that can check exit from another player */
	  if(pthread_create(&thCheck,NULL,threadCheckConnection,&checking) < 0)
	    {
	      fprintf(stderr,"phtread_create for connection response error\n");
	    }
	  
	  /* Waiting for answer */
	  pollVar = poll(ufds,1,10 * 1000);

	  if(pthread_cancel(thCheck) != 0)
	    {
	      fprintf(stderr,"Error at cancelling thread checking\n");
	    }
	  pthread_join(thCheck,0);
	  
	  if(pollVar > 0)
	    {
	      if((answer = receiveTCPMessage(server->listSockets[position],&status)) == NULL)
		{
		  fprintf(stderr,"Error at receiving answer TCP -- error %d\n",status);
		  removeClientFromList(server,position);
		  sendDeconnectionTo(server,-1,position,"general");
		  return 0;
		}
	  
	      printf("Answer = %s\n",answer);
	      if(isValidAnswerToAction(answer,position,player,server))
		{
		  /* Send accepted */
		  sendResponseAgreement(server,position,1);

		  /* Si le joueur a surrend, on doit avertir tout le monde 
		     que le joueur a récupérer la moitier de ces gains */
		  if(player->hasSurrendCurrentRound)
		    {
		      float earnings = player->hands[0]->bet / 2;
		      messageOfSurrend[0] = '\0';
		      sprintf(messageOfSurrend,"[name=%s:earnings=%f:hasSurrend=%d]",player->name, earnings,1);
		      sendEarnings(server,messageOfSurrend);
		    }
      
		  /* On envoie l'action du joueur à tout le monde */
		  sendInfosTo(server,-1,position, NULL);

		  /* On envoie le message pour dire ce qu'il s'est passé */
		  messageInfo = getLastInformation(player);
		  if(messageInfo != NULL)
		    {
		      sendInformationMessage(server,messageInfo);
		      free(messageInfo);
		    }
		}
	      else
		{
		  /* Send reject */
		  sendResponseAgreement(server,position,0);
		}
	  
	      free(answer);
	    }
	  else if(pollVar == 0)
	    {
	      /* Le joueur n'a pas répondu, on le stand sur toutes les mains */
	      while(!playerIsDone(player))
		{
		  playerStand(server->game,position);
		}

	      /* On envoie le fait que le joueur n'a pas jouer */
	      messageInfo = calloc(40,sizeof(char));
	      sprintf(messageInfo,"[message=%s n'a pas répondu.]",player->name);
	      sendInformationMessage(server,messageInfo);
	      free(messageInfo);
	    }
	  else if(pollVar < 0)
	    {
	      perror("Error in poll : ");
	    }
	      
	  /* On regarde si joueur surrend ou stand sur toutes les mains */
	  if(playerIsDone(player))
	    {
	      goOn = 0;
	    }
	}

      return 0;
    }
  
  return 1;
}

int askingPlaysEveryPlayer(Server* server)
{
  int status;
  int i;
  
  for(i = 0; i < server->game->table->maxPlayer; i++)
    {
      BlackjackPlayer* player = server->game->table->players[i];
      sendToAllChecking(server);
      if(player != NULL && player->isPlayingOnRound)
	{
	  resetHasHitAtInitialDistribution(player);
	  server->actualPlayerPos = player->positionOnTable;
	  sendActualSeat(server,server->actualPlayerPos);
	  if((status = askingPlays(server,player->positionOnTable,player)) != 0)
	    {
	      return status;
	    }
	}
    }
  
  return 0;
}

int dealerTurn(Server* server)
{
  int status;
  int posDealer = server->game->table->dealer->positionOnTable;
  
  server->actualPlayerPos = posDealer;
  sendActualSeat(server,server->actualPlayerPos);

  while(currentDeckValue(server->game->table->dealer) < 17)
    {
      char* messageInfo;
      
      strcpy(server->game->table->dealer->lastAction,"hit");
      /* Distribution */
      if((status=distribCardToPlayer(server->game,server->game->table->dealer)) != 0)
	{
	  return status;
	}

      /* Avertissement à tous les joueurs du plateau */
      sendInfosTo(server,-1,posDealer, NULL);

      /* Envoie message pour savoir ce qu'a obtenu le dealer */
      messageInfo = getLastInformation(server->game->table->dealer);
      if(messageInfo != NULL)
	{
	  sendInformationMessage(server,messageInfo);
	  free(messageInfo);
	}
      
      sleep(1);
    }

  return 0;
}

int earningDistribution(Server* server)
{
  int i,j;
  char message[750];
  message[0] = '\0';
  
  /* Distribution des gains logique */
  for(i = 0; i < server->game->table->maxPlayer; i++)
    {
      BlackjackPlayer* dealer = server->game->table->dealer;
      BlackjackPlayer* player = server->game->table->players[i];
      
      if(player != NULL && player->isPlayingOnRound && !player->hasSurrendCurrentRound)
	{
	  int sumDealer = getTotalValueDeckPlayer(dealer,0);
	  int bjDealer = playerHasMadeBlackjack(dealer);
	  int bjPlayer = playerHasMadeBlackjack(player);
	  float earnings = 0.f;
	  /* Si joueur a fait blackjack, il n'avait qu'1 main */
	  if(bjPlayer && !bjDealer)
	    {
	      earnings = player->hands[0]->bet + (player->hands[0]->bet * 1.5f);
	    }
	  else
	    {
	      for(j = 0; j < player->currentHand; j++)
		{
		  int sumHand = getTotalValueDeckPlayer(player,j);
		  if(!bjDealer)
		    {
		      if(sumDealer == sumHand && sumDealer <= 21)
			{
			  earnings += player->hands[j]->bet; 
			}
		      else if(sumDealer > 21 && sumHand > 21)
			{
			  earnings += player->hands[j]->bet;
			}
		      else if((sumDealer < sumHand && sumHand <= 21) || (sumDealer > 21 && sumHand <= 21))
			{
			  /* On remporte 2 * sa mise */
			  earnings += (player->hands[j]->bet * 2);
			}
		    }
		}
	    }
	  
	  sprintf(message,"%s[name=%s:earnings=%f:hasSurrend=%d]",message,player->name,earnings,0);
	  player->money += earnings;
	  player->earnings = earnings;
	}
    }

  /* On averti les différents clients des gains etc ... */
  if(strcmp(message,"") == 0)
    {
      sprintf(message,"%s","[]");
    }
  sendEarnings(server,message);
  
  return 0;
}

int sendToAllChecking(Server* server)
{
  int i;
  for(i = 0; i < MAX_PLAYER_SERVER; i++)
    {
      BlackjackPlayer* player = server->game->table->players[i];
      if(player != NULL)
	{
	  char msg[10] = "ping";
	  sendDataTCP(server->listSockets[player->positionOnTable],msg, server);
	}
    }
  return 0;
}

int sendToAllCheckingExceptPlaying(Server* server, int isPlayingPos)
{
  int i;
  for(i = 0; i < MAX_PLAYER_SERVER; i++)
    {
      BlackjackPlayer* player = server->game->table->players[i];
      if(player != NULL && player->positionOnTable != isPlayingPos)
	{
	  char msg[10] = "ping";
	  sendDataTCP(server->listSockets[player->positionOnTable],msg, server);
	}
    }
  return 0;
}

int reinit(Server* server)
{
  server->actualPlayerPos = -1;
  reinitRound(server->game);
  return 0;
}
