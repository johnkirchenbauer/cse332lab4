#ifndef TEXASHOLDEM_H
#define TEXASHOLDEM_H

#include "Game.h"
#include <vector>

class TexasHoldEm : public Game {

protected:
	size_t dealerPosition;   //keeps track of the dealer
	Deck discardDeck;		 //the cards discarded by the players
	int pot;
	Hand public_cards;

public:

	TexasHoldEm();

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