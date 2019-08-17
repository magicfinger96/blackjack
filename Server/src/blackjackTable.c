#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "blackjackTable.h"
#include "error.h"

BlackjackTable* constructBlackjackTable(int* status, int id, int maxPlayer)
{
  /* Variables */
  BlackjackTable* table;
  
  /* Instanciation */
  table = calloc(1,sizeof(BlackjackTable));
  if(table == NULL)
    {
      *status = ALLOCATION_ERROR;
      return NULL;
    }
  
  /* Initialisation */
  if((*status=initBlackjackTable(table,id,maxPlayer)) != 0)
    {
      freeBlackjackTable(table);
      return NULL;
    }

  *status = 0;
  return table;
}

int initBlackjackTable(BlackjackTable* table, int id, int maxPlayer)
{
  int statusDealer;
  
  table->id = id;
  table->maxPlayer = maxPlayer;
  table->currentPlayer = 0;
  table->playerTurn = 0;

  table->players = calloc(table->maxPlayer,sizeof(BlackjackPlayer*));
  if(table->players == NULL)
    {
      return ALLOCATION_ERROR;
    }

  table->dealer = constructBlackjackPlayer(&statusDealer,7,"Le croupier",10000);
  if(table->dealer == NULL)
    {
      return statusDealer;
    }

  return 0;
}

void freeBlackjackTable(BlackjackTable* table)
{
  if(table->players != NULL)
    {
      int i;
      for(i = 0; i < table->maxPlayer; i++)
	{
	  if(table->players[i] != NULL)
	    {
	      freeBlackjackPlayer(table->players[i]);
	    }
	}
      free(table->players);
    }

  if(table->dealer != NULL)
    {
      freeBlackjackPlayer(table->dealer);
    }
  free(table);
}

BlackjackPlayer* getBlackjackPlayerFromName(BlackjackTable* table, char* name)
{
  int i;
  for(i = 0; i < table->maxPlayer; i++)
    {
      if(table->players[i] != NULL && strcmp(table->players[i]->name,name) == 0)
	{
	  return table->players[i];
	}
    }
  return NULL;
}

BlackjackPlayer* getBlackjackPlayerFromPosition(BlackjackTable* table, int position)
{
  int i;
  for(i = 0; i < table->maxPlayer; i++)
    {
      if(table->players[i] != NULL && table->players[i]->positionOnTable == position)
	{
	  return table->players[i];
	}
    }
  return NULL;
}

int addPlayerToBlackjackTable(BlackjackTable* table, BlackjackPlayer* player)
{
  int i;
  int index;

  index = -1;
  /* Searching free index for new player */
  for(i = 0; i < table->maxPlayer; i++)
    {
      if(table->players[i] == NULL)
	{
	  index = i;
	  break;
	}
    }

  /* Assigning (or not) the new player */
  if(index == -1)
    {
      return NO_MORE_SPACE_ON_TABLE;
    }
  else
    {
      table->players[index] = player;
      table->currentPlayer++;
      return 0;
    }
}

int removePlayerFromBlackjackTable(BlackjackTable* table, int position)
{
  int i;
  int index;

  index = -1;
  /* Searching index to remove player */
  for(i = 0; i < table->maxPlayer; i++)
    {
      if(table->players[i] != NULL && table->players[i]->positionOnTable == position)
	{
	  index = i;
	  break;
	}
    }

  /* Remove (or not) the player from table */
  if(index == -1)
    {
      return PLAYER_IS_NOT_ON_TABLE;
    }
  else
    {
      freeBlackjackPlayer(table->players[index]);
      table->players[index] = NULL;
      table->currentPlayer--;
      return 0;
    }
}

void printBlackjackTable(const BlackjackTable* table)
{
  int i;

  printf("========================================\n");
  printf("Showing informations about table ID : %d\n",table->id);
  printf("Max player allowed in table : %d\n",table->maxPlayer);
  printf("Actual persons playing on table : %d\n",table->currentPlayer);
  printf("---- Dealer ----\n");
  printBlackjackPlayer(table->dealer);
  
  printf("---- PLAYERS ---- : \n");
  
  for(i = 0; i < table->maxPlayer; i++)
    {
      printf("---- Player N° %d ----\n",i+1);
      if(table->players[i] == NULL)
	{
	  printf("No player\n");
	}
      else
	{
	  printBlackjackPlayer(table->players[i]);
	}
    }
}
