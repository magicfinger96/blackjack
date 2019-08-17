#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "game.h"
#include "error.h"

Game* constructGame(int* status, int nbDeck, int maxOnTable, float startMoney, float minBet, float maxBet)
{
  /* Variables */
  Game* game;
  
  /* Instanciation */
  game = calloc(1,sizeof(Game));
  if(game == NULL)
    {
      *status = ALLOCATION_ERROR;
      return NULL;
    }
  
  /* Initialisation */
  if((*status=initGame(game, nbDeck, maxOnTable, startMoney, minBet, maxBet)) != 0)
    {
      freeGame(game);
      return NULL;
    }

  *status = 0;
  return game;
}

int initGame(Game* game, int nbDeck, int maxOnTable, float startMoney, float minBet, float maxBet)
{
  int status;
  int i;
  
  game->startMoney = startMoney;
  game->minBet = minBet;
  game->maxBet = maxBet;
  game->nbDeck = nbDeck;
  game->currentDeckPicking = 0;
  game->currentCardPicking = 0;

  if((game->table = constructBlackjackTable(&status,1, maxOnTable)) == NULL)
    {
      return status;
    }

  game->decks = calloc(nbDeck, sizeof(Deck*));
  if(game->decks == NULL)
    {
      return ALLOCATION_ERROR;
    }

  for(i = 0; i < nbDeck; i++)
    {
      game->decks[i] = constructDeck(&status,i);
      if(game->decks[i] == NULL)
	{
	  return status;
	}
    }

  game->decks[0]->cards[5]->figure = AS;
  game->decks[0]->cards[6]->figure = AS;
  game->decks[0]->cards[7]->figure = AS;
  game->decks[0]->cards[8]->figure = AS;
  
  return 0;
}

void freeGame(Game* game)
{
  int i;
  
  if(game->table != NULL)
    freeBlackjackTable(game->table);

  if(game->decks != NULL)
    {
      for(i = 0; i < game->nbDeck; i++)
	{
	  if(game->decks[i] != NULL)
	    {
	      freeDeck(game->decks[i]);
	    }
	}
      free(game->decks);
    }
    
  free(game);
}

int nbPlayerOnTable(const Game* game)
{
  return game->table->currentPlayer;
}

int newPlayerJoinGame(Game* game, const char* name, int position)
{
  BlackjackPlayer* player;
  int status;
  

  player = constructBlackjackPlayer(&status, position, name,game->startMoney);
  if(player == NULL)
    {
      return status;
    }
      
  if((status = addPlayerToBlackjackTable(game->table,player)) != 0)
    {
      freeBlackjackPlayer(player);
      return status;
    }
  return 0;
}

int playerLeftGame(Game* game, int positionTable)
{
  return removePlayerFromBlackjackTable(game->table, positionTable);
}

char* newPossibleName(Game* game, char* name)
{
  int i;
  int it = 1;
  int newSize = strlen(name)+2;
  char* newName = calloc(newSize,sizeof(char));
  strcpy(newName,name);
  
  for(i = 0; i < game->table->maxPlayer; i++)
    {
      if(game->table->players[i] != NULL)
	{
	  printf("Found : %s\n",game->table->players[i]->name);
	  if(strcmp(newName,game->table->players[i]->name) == 0)
	    {
	      while(strcmp(newName,game->table->players[i]->name) == 0)
		{
		  newName[strlen(name)] = it+'0';
		  newName[strlen(name)+1] = '\0';
		  it++;
		}
	    }
	}
    }
  newName[strlen(name)+1] = '\0';
  return newName;
}


void tagPlayerAbleToRound(Game* game)
{
  int i;
  for(i = 0; i < game->table->maxPlayer; i++)
    {
      if(game->table->players[i] != NULL)
	{
	  game->table->players[i]->isPlayingOnRound = 1;
	}
    }
}

void printGame(const Game* game)
{
  printf("================================================\n");
  printf("Welcome to the game\n");
  printf("You are playing on the table of ID : %d\n",game->table->id);
  printf("Settings of the table : \n");
  printBlackjackTable(game->table);
  printf("================================================\n");
}

void burnFirstCards(Game* game, int nbCard)
{
  game->currentCardPicking += nbCard;
}

int betFromPlayer(Game* game, int playerID, float bet)
{
  BlackjackPlayer* player = getBlackjackPlayerFromPosition(game->table,playerID);
  if(player != NULL)
    {
      return setBetBlackjackPlayer(player,bet);
    }
  else
    {
      return PLAYER_IS_NOT_ON_TABLE;
    }
}

BlackjackPlayer** removePlayersUnableToBet(Game* game, int* totalRemoved, int* status)
{
  int i;
  BlackjackPlayer** removing;
  int current = 0;

  removing = calloc(game->table->maxPlayer,sizeof(BlackjackPlayer*));
  if(removing == NULL)
    {
      *status = ALLOCATION_ERROR;
      return NULL;
    }
  
  for(i = 0; i < game->table->maxPlayer; i++)
    {
      BlackjackPlayer* focus = game->table->players[i];
      if(focus != NULL && !canBetAtMinimumBlackjackPlayer(focus,game->minBet))
	{
	  removing[current++] = focus;
	  game->table->players[i] = NULL;
	}
    }

  *totalRemoved = current;
  *status = 0;
  return removing;
}

int checkAllPlayersHasBet(Game* game)
{
  int i;
  for(i = 0; i < game->table->maxPlayer; i++)
    {
      BlackjackPlayer* player = game->table->players[i];
      if(player != NULL && player->isPlayingOnRound && playerHasBet(player))
	{
	  return 0;
	}
    }

  return 1;
}

int distribCardToPlayer(Game* game, BlackjackPlayer* player)
{
  int status;
  Card* cardRef;

  int totalPlayed;
  int totalCard;
  int i;

  if(game->currentCardPicking >= 52)
    {
      game->currentCardPicking = 0;
      game->currentDeckPicking++;
    }
      
  totalPlayed = (game->currentDeckPicking * 52) + game->currentCardPicking;
  totalCard = game->nbDeck * 52;
      
  if((float)totalPlayed/(float)totalCard >= 0.75f)
    {
      for(i = 0; i < game->nbDeck; i++)
	{
	  shuffleDeck(game->decks[i]);
	}
	  
      game->currentDeckPicking = 0;
      game->currentCardPicking = 0;
    }

  cardRef = game->decks[game->currentDeckPicking]->cards[game->currentCardPicking];
  game->currentCardPicking++;

  if((status=addRefCardToPlayerDeck(player, cardRef)) != 0)
    {
      return status;
    }
  else
    {
      return 0;
    }
}

int initialCardDistribution(Game* game)
{
  int i;
  int status;
  
  /* Dealer distribution */
  if((status=distribCardToPlayer(game,game->table->dealer)) != 0)
    return status;

  /* Players distribution */
  for(i = 0; i < game->table->maxPlayer; i++)
    {
      if(game->table->players[i] != NULL && game->table->players[i]->isPlayingOnRound)
	{
	  if((status=distribCardToPlayer(game,game->table->players[i])) != 0)
	    return status;
	  if((status=distribCardToPlayer(game,game->table->players[i])) != 0)
	    return status;
	}
    }
  
  return 0;
}

int playerHitCard(Game* game, int playerPosition)
{
  int statusHit;
  BlackjackPlayer* player = getBlackjackPlayerFromPosition(game->table,playerPosition);
  if(player == NULL)
    {
      return PLAYER_IS_NOT_ON_TABLE;
    }
  else
    {
      if(currentDeckValue(player) >= 21)
	{
	  return TRYING_HIT_OVER_21;
	}
      
      if((statusHit=distribCardToPlayer(game,player)) != 0)
	{
	  return statusHit;
	}

      player->hasMadeAction = 1;
      strcpy(player->lastAction,"hit");
      return 0;
    }
}

int playerSurrend(Game* game, int playerPosition)
{
  BlackjackPlayer* player = getBlackjackPlayerFromPosition(game->table,playerPosition);
  if(player == NULL)
    {
      return PLAYER_IS_NOT_ON_TABLE;
    }
  else if(player->hasMadeAction)
    {
      return TRYING_SURREND_ALREADY_ACTION_MADE;
    }
  else
    {
      /* Quand on surrend, on a rien fait avant */
      player->money += player->hands[0]->bet / 2;
      player->earnings = player->hands[0]->bet / 2.f;
      player->hasSurrendCurrentRound = 1;
      player->hasMadeAction = 1;
      strcpy(player->lastAction,"surrend");
      return 0;
    }
}

int playerStand(Game* game, int playerPosition)
{
  BlackjackPlayer* player = getBlackjackPlayerFromPosition(game->table,playerPosition);
  if(player == NULL)
    {
      return PLAYER_IS_NOT_ON_TABLE;
    }
  else
    {
      player->hands[player->currentHandPlaying++]->handIsDone = 1;
      player->hasMadeAction = 1;
      strcpy(player->lastAction,"stand");
      return 0;
    }
}

int playerDouble(Game* game, int playerPosition)
{
  int statusHit;
  BlackjackPlayer* player = getBlackjackPlayerFromPosition(game->table,playerPosition);
  if(player == NULL)
    {
      return PLAYER_IS_NOT_ON_TABLE;
    }
  else
    {
      if(player->currentHand > 1)
	{
	  return TRYING_DOUBLE_WHILE_SPLIT;
	}
      if(playerHasDouble(player))
	{
	  return TRYING_DOUBLE_WHEN_ALREADY_DONE;
	}
      if(playerHasHit(player))
	{
	  return TRYING_DOUBLE_WHEN_ALREADY_HIT;
	}
      if(player->money - currentBetHand(player) < 0)
	{
	  return TRYING_DOUBLE_OVER_MONEY;
	}
      if(currentDeckValue(player) >= 21)
	{
	  return TRYING_DOUBLE_WITH_BLACKJACK;
	}

      /* DOUBLE BET AND HIT CARD */
      player->money -= currentBetHand(player);
      player->hands[player->currentHandPlaying]->bet *= 2;
      if((statusHit=distribCardToPlayer(game,player)) != 0)
	{
	  return statusHit;
	}
      player->hands[player->currentHandPlaying]->hasAlreadyDoubleOnHand = 1;
      player->hasMadeAction = 1;
      strcpy(player->lastAction,"double");
      return 0;
    }
}

int playerSplit(Game* game, int playerPosition)
{
  BlackjackPlayer* player = getBlackjackPlayerFromPosition(game->table,playerPosition);
  if(player == NULL)
    {
      return PLAYER_IS_NOT_ON_TABLE;
    }
  else
    {
      if(player->currentHand == 3)
	{
	  return TRYING_SPLIT_WITH_3_ALREADY;
	}

      if(!playerHasSameCards(player))
	{
	  return TRYING_SPLIT_WITHOUT_DOUBLE;
	}

      if(player->money - currentBetHand(player) < 0)
	{
	  return TRYING_SPLIT_OVER_MONEY;
	}

      splitHandOfPlayer(player);
      player->hasMadeAction = 1;
      strcpy(player->lastAction,"split");
      return 0;
    }
}

int reinitRound(Game* game)
{
  int i,j,k;
  BlackjackPlayer* dealer = game->table->dealer;

  /* Gestion dealer */
  dealer->currentHandPlaying = 0;
  dealer->isPlayingOnRound = 0;
  dealer->hasSurrendCurrentRound = 0;
  dealer->hasMadeAction = 0;
  dealer->earnings = -1;
  strcpy(dealer->lastAction,"");

  dealer->currentHand = 1;
  for(j = 0; j < dealer->currentHand; j++)
    {
      dealer->hands[j]->handIsDone = 0;
      dealer->hands[j]->hasAlreadyHitOnHand = 0;
      dealer->hands[j]->hasAlreadyDoubleOnHand = 0;
      dealer->hands[j]->bet = 0.f;
      for(k = 0; k < dealer->hands[j]->currentHand; k++)
	{
	  dealer->hands[j]->cardsRef[k] = NULL;
	}
      dealer->hands[j]->currentHand = 0;
    }
  
  /* Gestion joueur */
  
  for(i = 0; i < game->table->maxPlayer; i++)
    {
      BlackjackPlayer* player = game->table->players[i];
      if(player != NULL)
	{
	  player->currentHandPlaying = 0;
	  player->isPlayingOnRound = 0;
	  player->hasSurrendCurrentRound = 0;
	  player->hasMadeAction = 0;
	  player->earnings = -1;
	  strcpy(player->lastAction,"");

	  for(j = 0; j < player->currentHand; j++)
	    {
	      player->hands[j]->handIsDone = 0;
	      player->hands[j]->hasAlreadyHitOnHand = 0;
	      player->hands[j]->hasAlreadyDoubleOnHand = 0;
	      player->hands[j]->bet = 0.f;
	      for(k = 0; k < player->hands[j]->currentHand; k++)
		{
		  player->hands[j]->cardsRef[k] = NULL;
		}
	      player->hands[j]->currentHand = 0;
	    }
	  player->currentHand = 1;
	}
    }

  return 0;
}
