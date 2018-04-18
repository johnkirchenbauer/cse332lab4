/*
* Name: deck.h
* Author: Zach Miller (zach.miller@wustl.edu)
* Description: This file contains the declarations for the Deck class.  Decks have a vector containing Card objects.  Cards are inserted into the deck with the addCardsFromFile function. The deck
* can be shuffled using the shuffleDeck function
*/

#ifndef DECK_H
#define DECK_H

#include <vector>
#include "card.h"
using namespace std;



class Deck {
	friend ostream & operator<<(ostream & out, const Deck & deck);  //ostream insertion operator
public:
	Deck() {};  //default constructor
	void shuffleDeck();    //shuffles the deck
	const int size();    //returns the number of cards in the deck
	vector<Card> & getCards(); //returns the cards in the deck
	void add_card(Card c);

private:
	vector<Card> cards;   //the cards in the deck
};

#endif