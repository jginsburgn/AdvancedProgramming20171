//
//  Card.h
//  005-Blackjack
//
//  Created by Jonathan Ginsburg on 3/5/17.
//  Copyright © 2017 Jonathan Ginsburg. All rights reserved.
//

#ifndef Card_h
#define Card_h

#include "../../Libraries/JG.h"
#include <math.h>
#include <signal.h>

#define CARD_DESCRIPTION_SIZE 40
#define DECK_SIZE 52
#define SUIT_COUNT 4
#define RANK_COUNT 13
#define BLACK_JACK 21
#define MIN_DEALERS_HAND 17

// Card suits
enum Suit {
    clubs = 0,
    diamonds = 1,
    hearts = 2,
    spades = 3
};

//Card ranks
enum Rank {
    ace = 1,
    two = 2,
    three = 3,
    four = 4,
    five = 5,
    six = 6,
    seven = 7,
    eight = 8,
    nine = 9,
    ten = 10,
    jack = 11,
    queen = 12,
    king = 13
};

//Card structure
struct Card {
    enum Suit suit;
    enum Rank rank;
};

//Returns a newly allocated string with the human readable description of the card.
char * getCardDescription(const struct Card card);
//Returns a newly allocated standard deck.
struct Card * getNewDeck();
//Suffles a deck.
void shuffleDeck(struct Card * deck);
//Returns the value of the card -card-; if -card- is ace and -aceTop- is true, the value is 11, if -aceTop- is false the value is 1.
int getCardValue(const struct Card card, const bool aceTop);
//Returns the Blackjack value of the -hand-.
int getHandValue(const struct Card hand[], const int handSize);
//Returns a new string with the description of the hand, i.e. cards and total value.
char * getHandDescription(const struct Card hand[], const int handSize, const bool dealerStyle);

#endif /* Card_h */
