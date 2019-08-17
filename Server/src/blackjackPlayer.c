#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blackjackPlayer.h"
#include "error.h"

Hand* constructHand(int* status)
{
  Hand* hand = calloc(1,sizeof(Hand));
  if(hand == NULL)
    {
      *status = ALLOCATION_ERROR;
      return NULL;
    }

  hand->handIsDone = 0;
  hand->hasAlreadyHitOnHand = 0;
  hand->hasAlreadyDoubleOnHand = 0;

  hand->bet = 0;
  
  hand->maxRefCard = MAX_REF_CARD_BASE;
  hand->currentHand = 0;
  hand->cardsRef = calloc(MAX_REF_CARD_BASE, sizeof(Card*));
  if(hand->cardsRef == NULL)
    {
      *status = ALLOCATION_ERROR;
      return NULL;
    }

  *status = 0;
  return hand;
}

void freeHand(Hand* hand)
{
  free(hand->cardsRef);
  free(hand);
}

BlackjackPlayer* constructBlackjackPlayer(int* status, int positionOnTable, const char* name, float money)
{
  /* Variables */
  BlackjackPlayer* player;
  
  /* Instanciation */
  player = calloc(1,sizeof(BlackjackPlayer));
  if(player == NULL)
    {
      *status = ALLOCATION_ERROR;
      return NULL;
    }
  
  /* Initialisation */
  if((*status=initBlackjackPlayer(player,positionOnTable,name,money)) != 0)
    {
      printf("ICI6\n");
      freeBlackjackPlayer(player);
      return NULL;
    }

  *status = 0;
  return player;
}

int initBlackjackPlayer(BlackjackPlayer* player,int positionOnTable, const char* name, float money)
{
  player->hasMadeAction = 0;
  player->positionOnTable = positionOnTable;
  player->money = money;
  player->earnings = -1;
  player->name = calloc(strlen(name)+1,sizeof(char));
  if(player->name == NULL)
    {
      return ALLOCATION_ERROR;
    }

  strcpy(player->lastAction,"");
  strcpy(player->name,name);
  
  player->currentHandPlaying = 0;
  player->isPlayingOnRound = 0;

  player->hasSurrendCurrentRound = 0;
  player->currentHand = 1;

  /* Allocation des X mains possibles */
  player->hands = calloc(MAX_HAND_POSSIBLE,sizeof(Hand*));
  if(player->hands == NULL)
    {
      return ALLOCATION_ERROR;
    }
  else
    {
      /* Allocation pour chaque main des deck */
      int i;
      int status;
      for(i = 0; i < MAX_HAND_POSSIBLE; i++)
	{
	  player->hands[i] = constructHand(&status);
	  if(player->hands[i] == NULL)
	    {
	      return status;
	    }
	}
    }
  
  return 0;
}

void freeBlackjackPlayer(BlackjackPlayer* player)
{
  if(player != NULL)
    {
      if(player->name != NULL)
	{
	  free(player->name);
	}
      if(player->hands != NULL)
	{
	  int i;
	  for(i = 0; i < MAX_HAND_POSSIBLE; i++)
	    {
	      if(player->hands[i] != NULL)
		{
		  freeHand(player->hands[i]);
		}
	    }
	  free(player->hands);
	}
    }
  
  if(player != NULL)
    {
      free(player);
    }
}

int canBetAtMinimumBlackjackPlayer(BlackjackPlayer* player, int minbet)
{
  return (player->money - minbet >= 0);
}

int setBetBlackjackPlayer(BlackjackPlayer* player, float bet)
{
  if(player->money - bet < 0)
    {
      return TRYING_BET_OVER_MONEY;
    }

  player->hands[player->currentHandPlaying]->bet = bet;
 
  player->money -= bet;
  return 0;
}

int getTotalValueDeckPlayer(const BlackjackPlayer* player, int hand)
{
  int i;
  int sumNoAs = 0;
  int nbAs = 0;
  int intermediate = 0;
  
  if(hand > player->currentHand)
    {
      return -1;
    }


  /* Let's make sum of all NONE as */
  for(i = 0; i < player->hands[hand]->currentHand; i++)
    {
      Card* card = player->hands[hand]->cardsRef[i];
      if(!card->figure == AS)
	{
	  sumNoAs += getValueFromFigure(card->figure);
	}
    }

  /* Let's counter nb AS */
  for(i = 0; i < player->hands[hand]->currentHand; i++)
    {
      Card* card = player->hands[hand]->cardsRef[i];
      if(card->figure == AS)
	{
	  nbAs++;
	}
    }

  if(nbAs > 0)
    {
      intermediate = sumNoAs + ( 1 * nbAs);
      if(intermediate + 10 <= 21)
	{
	  return intermediate + 10;
	}
      else
	{
	  return intermediate;
	}
    }
  else
    {
      return sumNoAs;
    }
}

int playerHasMadeBlackjack(const BlackjackPlayer* player)
{
  if(player->currentHand == 1 && player->hands[0]->currentHand == 2)
    {
      if(player->hands[0]->cardsRef[0]->figure == AS &&
	 getValueFromFigure(player->hands[0]->cardsRef[1]->figure) == 10)
	{
	  return 1;
	}
      else if(getValueFromFigure(player->hands[0]->cardsRef[0]->figure) == 10 &&
	      player->hands[0]->cardsRef[1]->figure == AS)
	{
	  return 1;
	}

      return 0;
    }

  return 0;
}

int playerHasSameCards(const BlackjackPlayer* player)
{
  int hand = player->currentHandPlaying;
  if(player->hands[hand]->currentHand != 2)
    {
      return 0;
    }

  return (player->hands[hand]->cardsRef[0]->figure == player->hands[hand]->cardsRef[1]->figure);
}

int splitHandOfPlayer(BlackjackPlayer* player)
{
  Card* splitted;
  int status;
  int saveCurrentHand;
  int hand = player->currentHandPlaying;

  splitted = player->hands[hand]->cardsRef[1];

  player->currentHand++;
  player->hands[hand]->cardsRef[1] = NULL;
  player->hands[hand]->currentHand--;

  saveCurrentHand = player->currentHandPlaying;
  player->currentHandPlaying = player->currentHand - 1;
  
  if((status=addRefCardToPlayerDeck(player,splitted)) != 0)
    {
      return status;
    }
  
  player->hands[player->currentHand-1]->hasAlreadyHitOnHand = 0;
  player->currentHandPlaying = saveCurrentHand;
  player->hands[player->currentHand-1]->bet = currentBetHand(player);
  player->money -= currentBetHand(player);
  
  return 0;
}

int addRefCardToPlayerDeck(BlackjackPlayer* player, Card* card)
{
  int hand = player->currentHandPlaying;
  
  if(hand > player->currentHand)
    {
      return TRYING_GET_UNCREATE_HAND;
    }

  if(player->hands[hand]->maxRefCard == player->hands[hand]->currentHand)
    {
      player->hands[hand]->maxRefCard *= 2;
      player->hands[hand]->cardsRef = realloc(player->hands[hand]->cardsRef,player->hands[hand]->maxRefCard * sizeof(Card*));
      if(player->hands[hand]->cardsRef == NULL)
	{
	  return REALLOCATION_ERROR;
	}
    }
  player->hands[hand]->hasAlreadyHitOnHand = 1;
  player->hands[hand]->cardsRef[player->hands[hand]->currentHand++] = card;
  return 0;
}

void printBlackjackPlayer(const BlackjackPlayer* player)
{
  int i,j;
  printf("Position on table : %d\n",player->positionOnTable);
  printf("Name : %s\n",player->name);
  printf("Is playing current round : %d\n",player->isPlayingOnRound);
  
  printf("Has surrend round : %d\n",player->hasSurrendCurrentRound);
  printf("Money : %f\n",player->money);
  printf("Number of hand : %d\n",player->currentHand);
  for(j = 0; j < player->currentHand; j++)
    {
      printf("\tIn hand : %d\n",j);
      for(i = 0; i < player->hands[j]->currentHand; i++)
	{
	  printf("\t");
	  printCard(player->hands[j]->cardsRef[i]);
	}
      printf("\tCurrent sum hand : %d\n",getTotalValueDeckPlayer(player,j));
    }
}

int playerHasHit(const BlackjackPlayer* player)
{
  return player->hands[player->currentHandPlaying]->hasAlreadyHitOnHand;
}

int playerHasDouble(const BlackjackPlayer* player)
{
  return player->hands[player->currentHandPlaying]->hasAlreadyDoubleOnHand;
}

int playerHasBet(const BlackjackPlayer* player)
{
  int i;

  for(i = 0; i < player->currentHand; i++)
    {
      if(player->hands[i]->bet <= 0)
	{
	  return 0;
	}
    }

  return 1;
}

void resetHasHitAtInitialDistribution(BlackjackPlayer* player)
{
  player->hands[0]->hasAlreadyHitOnHand = 0;
}

int currentBetHand(const BlackjackPlayer* player)
{
  return player->hands[player->currentHandPlaying]->bet;
}

int currentDeckValue(const BlackjackPlayer* player)
{
  int i;
  int sumNoAs = 0;
  int nbAs = 0;
  int intermediate = 0;

  /* Let's make sum of all NONE as */
  for(i = 0; i < player->hands[player->currentHandPlaying]->currentHand; i++)
    {
      Card* card = player->hands[player->currentHandPlaying]->cardsRef[i];
      if(!card->figure == AS)
	{
	  sumNoAs += getValueFromFigure(card->figure);
	}
    }

  /* Let's make sum of all AS */
  for(i = 0; i < player->hands[player->currentHandPlaying]->currentHand; i++)
    {
      Card* card = player->hands[player->currentHandPlaying]->cardsRef[i];
      if(card->figure == AS)
	{
	  nbAs++;
	}
    }

    if(nbAs > 0)
    {
      intermediate = sumNoAs + ( 1 * nbAs);
      if(intermediate + 10 <= 21)
	{
	  return intermediate + 10;
	}
      else
	{
	  return intermediate;
	}
    }
  else
    {
      return sumNoAs;
    }
}

int playerCanHit(const BlackjackPlayer* player)
{
  if(currentDeckValue(player) >= 21)
    {
      return 0;
    }
  
  return 1;
}

int playerCanDouble(const BlackjackPlayer* player)
{
  if(player->currentHand > 1)
    {
      return 0;
    }
  if(playerHasDouble(player))
    {
      return 0;
    }
  if(playerHasHit(player))
    {
      return 0;
    }
  if(player->money - currentBetHand(player) < 0)
    {
      return 0;
    }
  if(currentDeckValue(player) >= 21)
    {
      return 0;
    }

  return 1;
}

int playerCanSplit(const BlackjackPlayer* player)
{
  if(player->currentHand == 3)
    {
      return 0;
    }
  if(!playerHasSameCards(player))
    {
      return 0;
    }
  if(player->money - currentBetHand(player) < 0)
    {
      return 0;
    }

  return 1;
}

int playerCanSurrend(const BlackjackPlayer* player)
{
  if(player->hasMadeAction)
    {
      return 0;
    }

  return 1;
}

char* buildPlayerInformations(const BlackjackPlayer* player)
{
  int i,j;
  
  /* Variables */
  char name[20];
  char position[15];
  char money[20];
  char isPlaying[15];
  char hasSurrend[15];

  
  char handLine[240];
  char hands[3][70];

  char* totalSentence;
  hands[0][0] = '\0';
  hands[1][0] = '\0';
  hands[2][0] = '\0';

  /* Feeling simple variables */
  sprintf(name,"name=%s",player->name);
  sprintf(position,"position=%d",player->positionOnTable);
  sprintf(money,"money=%f",player->money);
  sprintf(isPlaying,"isPlaying=%d",player->isPlayingOnRound);
  sprintf(hasSurrend,"hasSurrend=%d",player->hasSurrendCurrentRound);
  
  /* Feeling hands */
  for(i = 0; i <  player->currentHand; i++)
    {
      Hand* hand = player->hands[i];
      char bet[15];
      char pairs[40];
      sprintf(bet,"bet=%f",hand->bet);
      pairs[0] = '\0';
      for(j = 0; j < hand->currentHand; j++)
	{
	  Card* card = hand->cardsRef[j];
	  char pair[8];
	  sprintf(pair,"(%d,%d)",card->figure,card->color);
	  strcat(pairs,pair);  
	}

      sprintf(hands[i],"{%s;cards=%s;value=%d}",bet,pairs,getTotalValueDeckPlayer(player,i));
      
    }
  
  sprintf(handLine,"hands=");
  for(i = 0; i < player->currentHand; i++)
    {
      strcat(handLine,hands[i]);
    }

  totalSentence = calloc(300, sizeof(char));
  if(totalSentence == NULL)
    {
      return NULL;
    }
  
  sprintf(totalSentence,"[%s:%s:%s:%s:%s:%s:earnings=%f]",name,position,money,isPlaying,hasSurrend,handLine, player->earnings);
  
  return totalSentence;
  /* Va ressembler à :
     [name=X:position=X:money=X:isPlaying=X:hasSurrend=X:hands={ {bet=X:cards={(10,2);(3,3);(2,1)}} {bet=X:cards={(4,2);(6,2)}} }]   
   */
}

char* buildPlayerPossibleAction(const BlackjackPlayer* player)
{
  /* Va ressembler a [hit=X:stand=X:surrend=X:split=X:double=X] */
  char* final = calloc(70,sizeof(char));
  char hit[10];  char stand[10]; char surrend[10]; char split[10]; char doubleHand[10];
  int canHit = playerCanHit(player);
  int canSurrend = playerCanSurrend(player);
  int canSplit = playerCanSplit(player);
  int canDouble = playerCanDouble(player);
  
  
  sprintf(hit,"hit=%d",canHit);
  sprintf(stand,"stand=%d",1);
  sprintf(surrend,"surrend=%d",canSurrend);
  sprintf(split,"split=%d",canSplit);
  sprintf(doubleHand,"double=%d",canDouble);
  sprintf(final,"[%s:%s:%s:%s:%s]",hit,stand,doubleHand,surrend,split);

  return final;
}

char* getLastInformation(const BlackjackPlayer* player)
{
  char* info;

  Card* lastCard;
  Hand* currentHand;
  const char* figure;
  const char* color;

  currentHand = player->hands[player->currentHand-1];
  lastCard = currentHand->cardsRef[currentHand->currentHand - 1];
  figure = getNameFromFigure(lastCard->figure);
  printCard(lastCard);
  color = getNameFromColor(lastCard->color);
  
  info = calloc(150,sizeof(char));

  if(strcmp(player->lastAction,"") != 0)
    {
      if(strcmp(player->lastAction,"hit") == 0)
	{
	  sprintf(info,"[message=%s a tiré %s de %s.]",player->name,figure,color);
	}
      else if(strcmp(player->lastAction,"stand") == 0)
	{
	  sprintf(info,"[message=%s reste sur sa main courante.]",player->name);
	}
      else if(strcmp(player->lastAction,"surrend") == 0)
	{
	  return NULL;
	}
      else if(strcmp(player->lastAction,"split") == 0)
	{
	  sprintf(info,"[message=%s a splitté sur la carte %s.]",player->name,figure);
	}
      else if(strcmp(player->lastAction,"double") == 0)
	{
	  sprintf(info,"[message=%s a doublé sa mise et tire un %s de %s.]",player->name,figure,color);
	}

      return info;
    }
  else
    {
      return NULL;
    }
}

int playerIsDone(const BlackjackPlayer* player)
{
  if(player->hasSurrendCurrentRound)
    {
      return 1;
    }
  else
    {
      int i;
      for(i = 0; i < player->currentHand; i++)
	{
	  if(!player->hands[i]->handIsDone)
	    {
	      return 0;
	    }
	}

      return 1;
    }
}
