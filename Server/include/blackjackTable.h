#ifndef __BLACKJACK_TABLE__
#define __BLACKJACK_TABLE__

#include "blackjackPlayer.h"

struct BlackjackTable
{
  int id;
  int maxPlayer;
  int currentPlayer;
  int playerTurn;
  
  BlackjackPlayer* dealer;
  BlackjackPlayer** players;
};

typedef struct BlackjackTable BlackjackTable;

BlackjackTable* constructBlackjackTable(int* status, int id, int maxPlayer);
int initBlackjackTable(BlackjackTable* table, int id, int maxPlayer);
void freeBlackjackTable(BlackjackTable* table);

BlackjackPlayer* getBlackjackPlayerFromName(BlackjackTable* table, char* name);
BlackjackPlayer* getBlackjackPlayerFromPosition(BlackjackTable* table, int position);
int addPlayerToBlackjackTable(BlackjackTable* table, BlackjackPlayer* player);
int removePlayerFromBlackjackTable(BlackjackTable* table, int position);

void printBlackjackTable(const BlackjackTable* table);


#endif
