#ifndef __DECK__
#define __DECK__

enum Color
  {
    HEARTS,DIAMOND,CLUB,SPADE,NB_COLOR
  };

enum Figure
  {
    AS,TWO,THREE,FOUR,FIVE,SIX,SEVEN,EIGHT,NINE,TEN,JACK,QUEEN,KING, NB_FIGURE
  };

struct Card
{
  enum Color color;
  enum Figure figure;
  int value;
};

struct Deck
{
  int idDeck;
  int sizeDeck;
  struct Card* cards[52];
};

typedef struct Card Card;
typedef struct Deck Deck;
typedef enum Color Color;
typedef enum Figure Figure;

int getValueFromFigure(Figure figure);
const char* getNameFromFigure(Figure figure);
const char* getNameFromColor(Color color);

/*======================*/
/*          CARD        */
/*======================*/

Card* constructCard(int* status, Color color, Figure figure, int value);
int initCard(Card* card, Color color, Figure figure, int value);
void freeCard(Card* card);

void printCard(const Card* card);

/*======================*/
/*          DECK        */
/*======================*/

Deck* constructDeck(int* status, int idDeck);
int initDeck(Deck* deck, int idDeck);
int buildDeck(Deck* deck);
void freeDeck(Deck* deck);
void shuffleDeck(Deck* deck);

void printDeck(const Deck* deck);

#endif
