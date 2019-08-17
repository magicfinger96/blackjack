#ifndef __SERVER__
#define __SERVER__

#define MAX_PLAYER_SERVER 7
#define MAX_SIZE_RESPONSE_WANT_TO_PLAY 200

#include "game.h"
#include "messageBuilder.h"

struct Server
{
  Game* game;
  int isRunning;
  int actualPlayerPos;
  int isWaitingForStart;

  int secondBeforeStart;
  int nbDeck;
  float startMoney;
  float betMin;
  float betMax;
  
  int enterSocket;
  int listenSocket;

  char* IPAddr;
  int portBroadcast;
  int portConnection;

  pthread_t wantPlay;
  pthread_t connection;
  pthread_t checkConnection;

  int listSockets[MAX_PLAYER_SERVER];
};


struct BetStruct
{
  struct Server* serverRef;
  int playerPosition;
  int betTime;
};

struct ConnectStruct
{
  struct Server* serverRef;
  int clientFDSocket;
};

struct CheckingStruct
{
  struct Server* serverRef;
  int positionExecuting;
};

typedef struct CheckingStruct CheckingStruct;
typedef struct ConnectStruct ConnectStruct;
typedef struct BetStruct BetStruct;
typedef struct Server Server;

int addClientToList(Server* server, int socket, char* name);
int removeClientFromList(Server* server, int socket);

Server* constructServer(int* status, int portBroad, int portConnection, int nbDeck, float startMoney, float betMin, float betMax);
int initServer(Server* server, int portBroad, int portConnection, int nbDeck, float startMoney, float betMin, float betMax);
void freeServer(Server* server);

int isValidAnswerToAction(char* message, int currentPlayerBetAsked, BlackjackPlayer* player, Server* server);
int isValidAnswerToBet(char* message, int currentPlayerBetAsked, BlackjackPlayer* player);

int getNextPositionPossible(Server* server);
int getNextPlayerPositionAtDeconnection(Server* server, int playerDeconnected);
void setFirstPlayerToPlay(Server* server);
int getClientPositionFromSocketID(Server* server, int ID);

void sendMessageActionPossible(Server* server, MessageType type, int position);
void sendMessageTo(Server* server,MessageType type,int positionToSend,char* path,char* message);
void sendDeconnectionTo(Server* server, int positionToSend, int positionDeconnected, char* message);
void sendInfosTo(Server* server, int positionToSend, int infoPositionToSend, char* pathGiven);
void sendInfosToConnection(Server* server, int socket, int futurPosition, char* futurName);
void sendResponseAgreement(Server* server, int positionToSend, int agree);
void sendEarnings(Server* server, char* message);
void sendActualSeat(Server* server, int seatGonnaPlay);
void sendGameStartIn(Server* server, int positionToSend);
void sendInformationMessage(Server* server, char* message);
char* createResponseToWantToPlay(Server* server);

int currentHandOfCurrentPlayer(Server* server);
char* prepareMessageInfoEveryPlayer(Server* server);
char* prepareMessageInfoSinglePlayer(Server* server, int position);
char* finalizeMsgInfo(Server* server, char* message, int position);

int removePlayerUnableToBet(Server* server);
int betTime(Server* server, int maxSecond);
int checkingAndRemovingUnbet(Server* server);
int cardInitialDistribution(Server* server);
int askingPlays(Server* server, int position, BlackjackPlayer* player);
int askingPlaysEveryPlayer(Server* server);
int dealerTurn(Server* server);
int earningDistribution(Server* server);
int reinit(Server* server);

int createForkWaiterConnection(Server* server);
int createForkWantToPlay(Server* server);
int createThreadCheckConnection(Server* server);

int sendToAllCheckingExceptPlaying(Server* server, int isPlayingPos);
int sendToAllChecking(Server* server);

void printServer(const Server* server);


#endif
