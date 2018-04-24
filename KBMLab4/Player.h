/*
* Name: Player.h
* Authors: Alex Bakus, John Kirchenbauer, Zach Miller
* Description: This file contains the declarations for the Player struct.  Each player has a name, tallies for rounds won and lost,
* and a hand
*/

#ifndef PLAYER_H
#define PLAYER_H

#include "hand.h"
#include <string>


struct Player {
	Player(char * name);

	string playerName;
	Hand player_cards;
	Hand faceup_cards;
	Hand facedown_cards;
	unsigned int hands_won;
	unsigned int hands_lost;
	int player_chips;
	bool still_betting;
	int current_bet;
	bool all_in;
};

ostream & operator<<(ostream & out, const Player & p);


#endif