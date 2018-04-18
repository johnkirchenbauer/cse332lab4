/*
* Name: card.cpp
* Author: Zach Miller (zach.miller@wustl.edu)
* Description: This file contains the definitions for the Card struct.  A Card has a suit and a rank.  Cards are ranked by rank first (2 is the smallest and ace is the largest), 
* and then by suit (alphabetically)
*/


#pragma once
#include "stdafx.h"
#include "card.h"
#include <iostream>

using namespace std;

/*Less-than operator: compare the rank first, if the rank is the same then compare the suit.  Suits are ordered alphabetically, e.g. clubs < diamonds */
bool Card::operator< (Card c) const{
	
	if (this->rank < c.rank) {
		return true;
	}
	else if (this->rank > c.rank) {
		return false;
	}
	else {
		return this->suit < c.suit;
	}
}
