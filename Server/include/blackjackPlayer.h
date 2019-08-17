#ifndef __BLACKJACK_PLAYER__
#define __BLACKJACK_PLAYER__

#include "deck.h"

#define MAX_REF_CARD_BASE 1
#define MAX_HAND_POSSIBLE 3

struct Hand
{
  Card** cardsRef;
  int maxRefCard;
  int currentHand;
  
  int handIsDone;
  int hasAlreadyHitOnHand;
  int hasAlreadyDoubleOnHand;

  float bet;
};

struct BlackjackPlayer
{
  char lastAction[60];
  int positionOnTable;
  char* name;
  float money;
  float earnings;

  int currentHandPlaying;
  int isPlayingOnRound;
  int hasMadeAction;
  
  int hasSurrendCurrentRound;

  int currentHand;
  struct Hand** hands;
};

typedef struct Hand Hand;
typedef struct BlackjackPlayer BlackjackPlayer;


Hand* constructHand(int* status);
void freeHand(Hand* hand);

int addCardToHand(Hand* hand, Card* card);


BlackjackPlayer* constructBlackjackPlayer(int* status, int positionOnTable, const char* name, float money);
int initBlackjackPlayer(BlackjackPlayer* player, int positionOnTable, const char* name, float money);
void freeBlackjackPlayer(BlackjackPlayer* player);

int playerCanHit(const BlackjackPlayer* player);
int playerCanDouble(const BlackjackPlayer* player);
int playerCanSplit(const BlackjackPlayer* player);
int playerCanSurrend(const BlackjackPlayer* player);

int playerHasBet(const BlackjackPlayer* player);
int playerHasHit(const BlackjackPlayer* player);
int playerHasDouble(const BlackjackPlayer* player);
int currentDeckValue(const BlackjackPlayer* player);
int currentBetHand(const BlackjackPlayer* player);
int playerIsDone(const BlackjackPlayer* player);
void resetHasHitAtInitialDistribution(BlackjackPlayer* player);

int canBetAtMinimumBlackjackPlayer(BlackjackPlayer* player, int minbet);
int setBetBlackjackPlayer(BlackjackPlayer* player, float bet);
int splitHandOfPlayer(BlackjackPlayer* player);

int getTotalValueDeckPlayer(const BlackjackPlayer* player, int hand);
int addRefCardToPlayerDeck(BlackjackPlayer* player, Card* card);
int playerHasMadeBlackjack(const BlackjackPlayer* player);
int playerHasSameCards(const BlackjackPlayer* player);

void printBlackjackPlayer(const BlackjackPlayer* player);
char* buildPlayerInformations(const BlackjackPlayer* player);
char* buildPlayerPossibleAction(const BlackjackPlayer* player);
char* getLastInformation(const BlackjackPlayer* player);

#endif
