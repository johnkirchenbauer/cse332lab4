/*
* Name: FiveCardDraw.h 
* Authors: Alex Bakus, John Kirchenbauer, Zach Miller
* Description: This file contains the declarations for the FiveCardDraw class, which inherits from the abstract class Game, and is meant to 
* represent the five-card draw poker variant, where each player is dealt 5 cards and has the choice to discard their cards.
*/

#ifndef FIVECARDDRAW_H
#define FIVECARDDRAW_H

#include "Game.h"

class FiveCardDraw : public Game {

protected: 
	size_t dealerPosition;   //keeps track of the dealer
	Deck discardDeck;		 //the cards discarded by the players
	int pot;
public: 

	FiveCardDraw();
	
	virtual int before_turn(Player & player);
	virtual int turn(Player & p);
	virtual int after_turn(Player & p);

	virtual int betting_zero();
	virtual int betting_one();
	virtual int before_round();
	virtual int round();
	virtual int after_round();
};


#endif