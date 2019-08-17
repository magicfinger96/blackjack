#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "deck.h"
#include "error.h"

Card* constructCard(int* status, Color color, Figure figure, int value)
{
  /* Variables */
  Card* card;

  /* Instanciation */
  card = calloc(1,sizeof(Card));
  if(card == NULL)
    {
      *status = ALLOCATION_ERROR;
      return NULL;
    }
  
  /* Initialisation */
  if((*status=initCard(card,color,figure,value)) != 0)
    {
      freeCard(card);
      return NULL;
    }

  *status = 0;
  return card;
}

int initCard(Card* card, Color color, Figure figure, int value)
{
  card->color = color;
  card->figure = figure;
  card->value = value;
  return 0;
}

void freeCard(Card* card)
{
  free(card);
}

void printCard(const Card* card)
{
  printf("Color : %s ; Figure : %s\n",getNameFromColor(card->color), getNameFromFigure(card->figure));
}


Deck* constructDeck(int* status, int idDeck)
{
  /* Variables */
  Deck* deck;
  
  /* Instanciation */
  deck = calloc(1,sizeof(Deck));
  if(deck == NULL)
    {
      *status = ALLOCATION_ERROR;
      return NULL;
    }
  
  /* Initialisation */
  if((*status=initDeck(deck, idDeck)) != 0)
    {
      freeDeck(deck);
      return NULL;
    }

  /* Building deck */
  if((*status=buildDeck(deck)) != 0)
    {
      freeDeck(deck);
      return NULL;
    }

  /* Shuffling */
  shuffleDeck(deck);
     
  *status = 0;
  return deck;
}

int initDeck(Deck* deck, int idDeck)
{
  int i;
  
  deck->idDeck = idDeck;
  deck->sizeDeck = 52;
  for(i = 0; i < deck->sizeDeck; i++)
    {
      deck->cards[i] = NULL;
    }
  return 0;
}

int buildDeck(Deck* deck)
{
  int status;
  int iterator;
  int color;
  int figure;

  iterator = 0;
  
  for(color = 0; color < NB_COLOR; color++)
    {
      for(figure = 0; figure < NB_FIGURE; figure++)
	{
	  deck->cards[iterator] = constructCard(&status,color,figure,getValueFromFigure(figure));
	  if(deck->cards[iterator] == NULL)
	    {
	      return status;
	    }
	  iterator++;
	}
    }
  
  return 0;
}

void freeDeck(Deck* deck)
{
  int i;
  for(i = 0; i < deck->sizeDeck; i++)
    {
      if(deck->cards[i] != NULL)
	{
	  freeCard(deck->cards[i]);
	}
    }
  free(deck);
}

void shuffleDeck(Deck* deck)
{
  int i;
  for(i = 0; i < deck->sizeDeck; i++)
    {
      int j = i + rand() / (RAND_MAX / (deck->sizeDeck - i) + 1);
      
      Card* tmp = deck->cards[j];
      deck->cards[j] = deck->cards[i];
      deck->cards[i] = tmp;
    }
}

void printDeck(const Deck* deck)
{
  int i;
  for(i = 0; i < deck->sizeDeck; i++)
    {
      printf("====================\n");
      printCard(deck->cards[i]);
    }
}

int getValueFromFigure(Figure figure)
{
  if(figure >= AS && figure <= NINE)
    {
      return (int)(figure+1);
    }
  else
    {
      return 10;
    }
}

const char* getNameFromColor(Color color)
{
  if(color == SPADE)
    {
      return "Pique";
    }
  else if(color == CLUB)
    {
      return "Trefle";
    }
  else if(color == HEARTS)
    {
      return "Coeur";
    }
  else if(color == DIAMOND)
    {
      return "Carreau";
    }
  return "Unknow";
}

const char* getNameFromFigure(Figure figure)
{
  if(figure == AS)
    return "As";
  else if(figure == TWO)
    return "2";
  else if(figure == THREE)
    return "3";
  else if(figure == FOUR)
    return "4";
  else if(figure == FIVE)
    return "5";
  else if(figure == SIX)
    return "6";
  else if(figure == SEVEN)
    return "7";
  else if(figure == EIGHT)
    return "8";
  else if(figure == NINE)
    return "9";
  else if(figure == TEN)
    return "10";
  else if(figure == JACK)
    return "Valet";
  else if(figure == QUEEN)
    return "Reine";
  else if(figure == KING)
    return "Roi";

  return "Unknow";
}

 
