/*
* Name: poker_rank_logic.h
* Author: Zach Miller (zach.miller@wustl.edu)
* Description: This file contains the declarations for the poker_rank_logic class.  getHandRank returns an integer representing the poker hand of the input vector.  poker_rank takes in 
* two hands and determines which hand has a higher poker rank
*/

#ifndef POKER_RANK_LOGIC_H
#define POKER_RANK_LOGIC_H
 
#include "Card.h"
#include "hand.h"
#include <map>

/*Clears all the values in a map*/
void clear_map(map<int, int> & m);

/*Updates the map based on the cards in the input vector (hand)*/
void update_map(map<int, int> & m, vector<Card> v);

/*Returns a numeric value for the rank of a hand (straight flush > four of a kind > full house > etc)*/
int getHandRank(const vector<Card> & v);

/*Returns true if the first hand is a better poker hand than the second hand*/
bool poker_rank(const Hand & h1, const Hand & h2);


vector<size_t> fourOfAKind_or_twoPair_DiscardIndices(vector<Card> hand);

vector<size_t> onePairDiscardIndices(vector<Card> hand);

vector<size_t> threeOfAKindDiscardIndices(vector<Card> hand);

vector<size_t> noHandDiscardIndices(vector<Card> hand);

#endif