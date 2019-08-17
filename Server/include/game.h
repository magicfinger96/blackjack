#ifndef __GAME__
#define __GAME__

#include "blackjackTable.h"
#include "deck.h"

enum ActionPlayer
  {
    HIT,
    STAND,
    SPLIT,
    DOUBLE,
    SURREND
  };

struct Game
{
  BlackjackTable* table;
  Deck** decks;
  int nbDeck;
  float startMoney;
  float minBet;
  float maxBet;
  
  int currentDeckPicking;
  int currentCardPicking;
};

typedef enum ActionPlayer ActionPlayer;
typedef struct Game Game;

Game* constructGame(int* status, int nbDeck, int maxOnTable, float startMoney, float minBet, float maxBet);
int initGame(Game* game, int nbDeck, int maxOnTable, float startMoney, float minBet, float maxBet);
void freeGame(Game* game);

int nbPlayerOnTable(const Game* game);
int newPlayerJoinGame(Game* game, const char* name, int position);
int playerLeftGame(Game* game, int positionTable);
char* newPossibleName(Game* game, char* name);

void tagPlayerAbleToRound(Game* game);

void burnFirstCards(Game* game, int nbCard);
BlackjackPlayer** removePlayersUnableToBet(Game* game, int* totalRemoved, int* status);

int playerHitCard(Game* game, int playerPosition);
int playerSurrend(Game* game, int playerPosition);
int playerSplit(Game* game, int playerPosition);
int playerDouble(Game* game, int playerPosition);
int playerStand(Game* game, int playerPosition);

int betFromPlayer(Game* game, int playerID, float bet);
int checkAllPlayersHasBet(Game* game);
int distribCardToPlayer(Game* game, BlackjackPlayer* player);
int initialCardDistribution(Game* game);
int endTurnGame(Game* game);
int reinitRound(Game* game);

void printGame(const Game* game);

#endif
