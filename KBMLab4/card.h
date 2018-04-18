/*
* Name: card.h
* Author: Zach Miller (zach.miller@wustl.edu)
* Description: This file contains the declarations for the Card struct.  A Card has a suit and a rank 
*/

#ifndef CARD_H
#define CARD_H

using namespace std;

struct Card {
	
	enum Suit { clubs, diamonds, hearts, spades };
	enum Rank { two = 2, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace };
	Suit suit;
	Rank rank;

	bool operator< (Card c) const;
};



#endif