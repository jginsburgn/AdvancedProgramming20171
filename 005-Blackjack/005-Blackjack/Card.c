//
//  Card.c
//  005-Blackjack
//
//  Created by Jonathan Ginsburg on 3/5/17.
//  Copyright © 2017 Jonathan Ginsburg. All rights reserved.
//

#include "Card.h"

char * getCardDescription(const struct Card card) {
    char * retVal = calloc(CARD_DESCRIPTION_SIZE, sizeof(char));
    switch (card.rank) {
        case ace:
            strcat(retVal, "Ace of ");
            break;
        case two:
            strcat(retVal, "Two of ");
            break;
        case three:
            strcat(retVal, "Three of ");
            break;
        case four:
            strcat(retVal, "Four of ");
            break;
        case five:
            strcat(retVal, "Five of ");
            break;
        case six:
            strcat(retVal, "Six of ");
            break;
        case seven:
            strcat(retVal, "Seven of ");
            break;
        case eight:
            strcat(retVal, "Eight of ");
            break;
        case nine:
            strcat(retVal, "Nine of ");
            break;
        case ten:
            strcat(retVal, "Ten of ");
            break;
        case jack:
            strcat(retVal, "Jack of ");
            break;
        case queen:
            strcat(retVal, "Queen of ");
            break;
        case king:
            strcat(retVal, "King of ");
            break;
    }
    switch (card.suit) {
        case clubs:
            strcat(retVal, "\u2667 Clubs");
            break;
        case diamonds:
            strcat(retVal, "\u2662 Diamonds");
            break;
        case hearts:
            strcat(retVal, "\u2661 Hearts");
            break;
        case spades:
            strcat(retVal, "\u2664 Spades");
            break;
    }
    return retVal;
}

struct Card * getNewDeck() {
    struct Card * deck = malloc(sizeof(struct Card) * DECK_SIZE);
    for (int i = 0; i < SUIT_COUNT; ++i) {
        for (int j = 0; j < RANK_COUNT; ++j) {
            struct Card * currentCard = &deck[j + (i * RANK_COUNT)];
            currentCard->suit = i;
            currentCard->rank = j + 1;
        }
    }
    return deck;
}

void shuffleDeck(struct Card * deck) {
    struct Card aidDeck[DECK_SIZE];
    memcpy(aidDeck, deck, sizeof(struct Card) * DECK_SIZE);
    for (int i = 0; i < DECK_SIZE; ++i) {
        int randomIndex = getRandomNumberLimited(DECK_SIZE - 1 - i);
        deck[i] = aidDeck[randomIndex];
        aidDeck[randomIndex] = aidDeck[DECK_SIZE - 1 - i];
    }
}

//Returns the value of the card -card-; if -card- is ace and -aceTop- is true, the value is 11, if -aceTop- is false the value is 1.
int getCardValue(const struct Card card, const bool aceTop) {
    switch (card.rank) {
        case ace:
            if (aceTop) return 11;
            else return 1;
            break;
        case jack:
            return 10;
            break;
        case queen:
            return 10;
            break;
        case king:
            return 10;
            break;
        default:
            return (int)card.rank;
            break;
    }
}

//Returns the Blackjack value of the -hand-.
int getHandValue(const struct Card hand[], const int handSize) {
    int acesInHand = 0;
    int retVal = 0;
    for (int i = 0; i < handSize; ++i) {
        if (hand[i].rank == ace) ++acesInHand;
        retVal += getCardValue(hand[i], true);
    }
    if (retVal > BLACK_JACK) {
        for (int i = 0; i < acesInHand; ++i) {
            retVal -= (i + 1) * 10;
            if (retVal <= BLACK_JACK) {
                return retVal;
            }
        }
    }
    return retVal;
}

//Returns a new string with the description of the hand, i.e. cards and total value.
char * getHandDescription(const struct Card hand[], const int handSize, const bool dealerStyle) {
    char * retVal = malloc(sizeof(char) * CARD_DESCRIPTION_SIZE * handSize);
    bzero(retVal, CARD_DESCRIPTION_SIZE * handSize);
    char buffer[CARD_DESCRIPTION_SIZE];
    for (int i = 0; i < handSize; ++i) {
        if (i == handSize - 1 && dealerStyle) {
            bzero(buffer, CARD_DESCRIPTION_SIZE);
            sprintf(buffer, "*Hidden card*\n");
            strcat(retVal, buffer);
        }
        else {
            bzero(buffer, CARD_DESCRIPTION_SIZE);
            sprintf(buffer, "%s\n", getCardDescription(hand[i]));
            strcat(retVal, buffer);
        }
    }
    bzero(buffer, CARD_DESCRIPTION_SIZE);
    if (dealerStyle) {
        sprintf(buffer, "\tTotal: %i", getHandValue(hand, handSize - 1));
    }
    else {
        sprintf(buffer, "\tTotal: %i", getHandValue(hand, handSize));
    }
    strcat(retVal, buffer);
    return retVal;
}
