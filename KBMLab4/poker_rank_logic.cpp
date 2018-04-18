/*
* Name: poker_rank_logic.cpp
* Author: Zach Miller (zach.miller@wustl.edu)
* Description: This file contains the definitions for the poker_rank_logic class.  getHandRank returns an integer representing the poker hand of the input vector.  poker_rank takes in
* two hands and determines which hand has a higher poker rank.
*/


#pragma once
#include "stdafx.h"
#include "poker_rank_logic.h"
#include "kbmlab4.h"
#include <map>
#include <vector>
#include <algorithm>
#include "Game.h"

using namespace std;

//enum of the possible poker hands
enum hand_ranks { no_hand, one_pair, two_pair, three_of_a_kind, straight, flush_hand, full_house, four_of_a_kind, straight_flush };

unsigned const int hand_size = five;   //number of cards in a hand

//map that keeps track of how many times a rank occurs in a hand
map<int, int> rank_count = {
	{ 2, 0 },
	{ 3, 0 },
	{ 4, 0 },
	{ 5, 0 },
	{ 6, 0 },
	{ 7, 0 },
	{ 8, 0 },
	{ 9, 0 },
	{ 10, 0 },
	{ 11, 0 },
	{ 12, 0 },
	{ 13, 0 },
	{ 14, 0 }
};

/*This is a helper method that sets all the values in a map to zero*/
void clear_map(map<int, int> & m) {
	for (map<int, int>::iterator it = m.begin(); it != m.end(); ++it) {
		it->second = zero;
	}
}

/*This is a helper method that goes through the hand, and updates the rank_count map*/
void update_map(map<int, int> & m, vector<Card> v) {
	for (unsigned int i = zero; i < hand_size; ++i) {
		int rank = v[i].rank;
		m.find(rank)->second++;
	}
}

/*This function returns the numeric rank of the input poker hand (straight flush > four of a kind > full house > etc)*/
int getHandRank(const vector<Card> & v) {

	//return failure if the vector is empty
	if (v.size() == zero) {
		return Failure;
	}

	bool threeOfAKind = false;       //true if the hand contains 3 cards of the same rank
	bool fourOfAKind = false;        //true if the hand contains 4 cards of the same rank
	bool onePair = false;			 //true if there is one and ONLY one pair of cards with the same rank
	bool twoPair = false;			 //true if there are two pairs of cards with the same rank

									 //this for-loop copies the 5 cards into another vector, and updates the rank_count map accordingly
	for (unsigned int i = zero; i < hand_size; ++i) {
		int rank = v[i].rank;                  //get the rank of each card
		rank_count.find(rank)->second++;               //increment the count for that rank by 1
	}

	//this for-loop iterates through the map of the counts and sets the booleans above accordingly
	for (map<int, int>::iterator it = rank_count.begin(); it != rank_count.end(); ++it) {
		int rank = it->first;

		if (rank_count.find(rank)->second == four) {
			fourOfAKind = true;
		}

		if (rank_count.find(rank)->second == three) {
			threeOfAKind = true;
		}

		if (rank_count.find(rank)->second == two) {
			if (!onePair) {
				onePair = true;
			}
			else {
				onePair = false;
				twoPair = true;
			}
		}

	} //end of for-loop that iterates through the rank_count map


	int output = no_hand;

	if (twoPair) {
		output = two_pair;
	}

	//if threeOfAKind is true, we have to also check onePair to see if the hand is a full house.  If not, then it is just a three of a kind.
	//If threeOfAKind is false and onePair is true, then one pair is the only possible hand
	if (threeOfAKind) {
		if (onePair) {
			output = full_house;
		}
		else {
			output = three_of_a_kind;
		}
	}
	else if (onePair) {
		output = one_pair;
	}

	//if fourOfAKind is true, then the hand must be a four of a kind.  No other hands are possible.
	if (fourOfAKind) {
		output = four_of_a_kind;
	}

	//these variables are used to keep track of how many cards of the same suit we have (flushCounter), and 
	//how many cards in order we have (straightCounter).  We can do this after checking for a full house, two pair,
	//etc. because straights and flushes are mutually exclusive from all those other hands
	int flushCounter = one;
	int straightCounter = one;

	vector<Card> temp = v;
	sort(temp.begin(), temp.end());             //sort the temporary vector

												//This if statement is used to detect low (A-5) straights.  If the last 2 cards in the sorted vector are 5 and an ace, then the counter is incremented.
	int lastIndex = v.size() - one;
	if (temp[lastIndex - one].rank == Card::five && temp[lastIndex].rank == Card::ace) {
		straightCounter++;
	}

	//this for-loop goes through the sorted temp vector and increments the counters for straights and flushes if necessary
	for (unsigned int j = one; j < v.size(); ++j) {

		if (temp[j - one].rank + one == temp[j].rank) {
			straightCounter++;  //increment if the rank increases by 1
		}
		if (temp[j - one].suit == temp[j].suit) {
			flushCounter++;    //increment if the suit is the same
		}
	}

	//This if-statement checks both counters to see if we have a straight or flush
	if (straightCounter == hand_size && flushCounter == hand_size) {
		output = straight_flush;
	}
	else if (flushCounter == hand_size && straightCounter != hand_size) {
		output = flush_hand;
	}
	else if (flushCounter != hand_size && straightCounter == hand_size) {
		output = straight;
	}

	temp.clear();       //clear the temporary vector

						//this for-loop iterates through the entire rank_count map and resets all the counts to 0
	for (map<int, int>::iterator it = rank_count.begin(); it != rank_count.end(); ++it) {
		it->second = zero;
	}

	return output;
}

/*This function compares two poker hands, and returns 1 if the first hand is better than the second hand*/
bool poker_rank(const Hand & h1, const Hand & h2) {
	const int hands_are_equal = zero;  //this value is returned if the hands are equal in rank

	if (h1 == h2) {
		return false;
	}

	//make non-const copies of the hands
	Hand first_hand = h1;
	Hand second_hand = h2;

	//get the cards in the hands
	vector<Card> first_hand_cards = first_hand.getCards();
	vector<Card> second_hand_cards = second_hand.getCards();

	//sort(first_hand_cards.begin(), first_hand_cards.end());
	//sort(second_hand_cards.begin(), second_hand_cards.end());

	//if the hand lengths are invalid, throw an error
	if (first_hand_cards.size() != hand_size || second_hand_cards.size() != hand_size) {
		throw TooFewCards;
	}

	//get the ranks of the hands
	int first_hand_rank = getHandRank(first_hand_cards);
	int second_hand_rank = getHandRank(second_hand_cards);

	//when the ranks of the hands are not the same
	if (first_hand_rank != second_hand_rank) {
		return first_hand_rank > second_hand_rank;
	}

	//tiebreaker for straight flush and straight
	if (first_hand_rank == straight_flush || first_hand_rank == straight) {
		int high_card_first_hand = first_hand_cards[hand_size - one].rank;
		int high_card_second_hand = second_hand_cards[hand_size - one].rank;
		if (high_card_first_hand == Card::ace) {
			int fourth_card_first_hand = first_hand_cards[hand_size - two].rank;
			if (fourth_card_first_hand == Card::five) {
				high_card_first_hand = Card::five;
			}
		}
		if (high_card_second_hand == Card::ace) {
			int fourth_card_second_hand = second_hand_cards[hand_size - two].rank;
			if (fourth_card_second_hand == Card::five) {
				high_card_second_hand = Card::five;
			}
		}
		return high_card_first_hand > high_card_second_hand;
	}

	//tiebreaker for flush and no hand
	if (first_hand_rank == flush_hand || first_hand_rank == no_hand) {
		for (int i = hand_size - 1; i >= zero; --i) {

			int first_hand_high = first_hand_cards[i].rank;
			int second_hand_high = second_hand_cards[i].rank;

			if (first_hand_high != second_hand_high) {
				return first_hand_high > second_hand_high;
			}
		}
		return Success;
	}

	//tiebreaker for four of a kind
	if (first_hand_rank == four_of_a_kind) {

		update_map(rank_count, first_hand_cards);

		int fourOfAKind_1;

		for (map<int, int>::iterator it = rank_count.begin(); it != rank_count.end(); ++it) {
			int rank = it->first;

			if (rank_count.find(rank)->second == four) {
				fourOfAKind_1 = rank;
			}
		}

		clear_map(rank_count);
		update_map(rank_count, second_hand_cards);

		int fourOfAKind_2;

		for (map<int, int>::iterator it = rank_count.begin(); it != rank_count.end(); ++it) {
			int rank = it->first;

			if (rank_count.find(rank)->second == four) {
				fourOfAKind_2 = rank;
			}
		}

		clear_map(rank_count);

		if (fourOfAKind_1 != fourOfAKind_2) {
			return fourOfAKind_1 > fourOfAKind_2;
		}
		else {
			return hands_are_equal;
		}

	}

	//tiebreaker for full house
	if (first_hand_rank == full_house) {

		update_map(rank_count, first_hand_cards);

		int threeOfAKind_1;
		int pair_1;

		for (map<int, int>::iterator it = rank_count.begin(); it != rank_count.end(); ++it) {
			int rank = it->first;

			if (rank_count.find(rank)->second == three) {
				threeOfAKind_1 = rank;
			}
			if (rank_count.find(rank)->second == two) {
				pair_1 = rank;
			}
		}

		clear_map(rank_count);
		update_map(rank_count, second_hand_cards);

		int threeOfAKind_2;
		int pair_2;

		for (map<int, int>::iterator it = rank_count.begin(); it != rank_count.end(); ++it) {
			int rank = it->first;

			if (rank_count.find(rank)->second == three) {
				threeOfAKind_2 = rank;
			}
			if (rank_count.find(rank)->second == two) {
				pair_2 = rank;
			}
		}

		clear_map(rank_count);

		if (threeOfAKind_1 != threeOfAKind_2) {
			return threeOfAKind_1 > threeOfAKind_2;
		}

		if (pair_1 != pair_2) {
			return pair_1 > pair_2;
		}
		else {
			return hands_are_equal;
		}
	}

	//tiebreaker for two pair
	if (first_hand_rank == two_pair) {

		update_map(rank_count, first_hand_cards);

		int high_pair_first_hand;
		int low_pair_first_hand;
		int first_hand_kicker;
		bool first_pair_found = false;

		for (map<int, int>::iterator it = rank_count.begin(); it != rank_count.end(); ++it) {
			int rank = it->first;

			if (rank_count.find(rank)->second == two) {
				if (first_pair_found) {
					low_pair_first_hand = rank;
				}
				else {
					first_pair_found = true;
					high_pair_first_hand = rank;
				}
			}
			if (rank_count.find(rank)->second == one) {
				first_hand_kicker = rank;
			}
		} //end for-loop

		  //switch the low and high pair if necessary
		if (low_pair_first_hand > high_pair_first_hand) {
			int temp = low_pair_first_hand;
			low_pair_first_hand = high_pair_first_hand;
			high_pair_first_hand = temp;
		}

		clear_map(rank_count);
		update_map(rank_count, second_hand_cards);

		int high_pair_second_hand;
		int low_pair_second_hand;
		int second_hand_kicker;
		first_pair_found = false;

		for (map<int, int>::iterator it = rank_count.begin(); it != rank_count.end(); ++it) {
			int rank = it->first;

			if (rank_count.find(rank)->second == two) {
				if (first_pair_found) {
					low_pair_second_hand = rank;
				}
				else {
					first_pair_found = true;
					high_pair_second_hand = rank;
				}
			}
			if (rank_count.find(rank)->second == one) {
				second_hand_kicker = rank;
			}
		} //end for-loop

		  //switch the low and high pair if necessary
		if (low_pair_second_hand > high_pair_second_hand) {
			int temp = low_pair_second_hand;
			low_pair_second_hand = high_pair_second_hand;
			high_pair_second_hand = temp;
		}

		clear_map(rank_count);

		//compare the high pair, return if not equal
		if (high_pair_first_hand != high_pair_second_hand) {
			return high_pair_first_hand > high_pair_second_hand;
		}

		if (low_pair_first_hand != low_pair_second_hand) {
			return low_pair_first_hand > low_pair_second_hand;
		}

		if (first_hand_kicker != second_hand_kicker) {
			return first_hand_kicker > second_hand_kicker;
		}
		else {
			return hands_are_equal;
		}
	}

	//tiebreaker for three of a kind
	if (first_hand_rank == three_of_a_kind) {

		update_map(rank_count, first_hand_cards);

		int threeOfAKind_1;

		for (map<int, int>::iterator it = rank_count.begin(); it != rank_count.end(); ++it) {
			int rank = it->first;

			if (rank_count.find(rank)->second == three) {
				threeOfAKind_1 = rank;
			}
		}

		clear_map(rank_count);
		update_map(rank_count, second_hand_cards);

		int threeOfAKind_2;

		for (map<int, int>::iterator it = rank_count.begin(); it != rank_count.end(); ++it) {
			int rank = it->first;

			if (rank_count.find(rank)->second == three) {
				threeOfAKind_2 = rank;
			}
		}

		clear_map(rank_count);

		if (threeOfAKind_1 != threeOfAKind_2) {
			return threeOfAKind_1 > threeOfAKind_2;
		}
		else {
			return hands_are_equal;
		}
	}

	//tiebreaker for one pair
	if (first_hand_rank == one_pair) {
		update_map(rank_count, first_hand_cards);

		int first_hand_pair;
		vector<int> first_hand_kickers;

		for (map<int, int>::iterator it = rank_count.begin(); it != rank_count.end(); ++it) {
			int rank = it->first;

			if (rank_count.find(rank)->second == two) {
				first_hand_pair = rank;
			}
			if (rank_count.find(rank)->second == one) {
				first_hand_kickers.push_back(rank);
			}
		}
		sort(first_hand_kickers.begin(), first_hand_kickers.end());

		clear_map(rank_count);
		update_map(rank_count, second_hand_cards);

		int second_hand_pair;
		vector<int> second_hand_kickers;

		for (map<int, int>::iterator it = rank_count.begin(); it != rank_count.end(); ++it) {
			int rank = it->first;

			if (rank_count.find(rank)->second == two) {
				second_hand_pair = rank;
			}
			if (rank_count.find(rank)->second == one) {
				second_hand_kickers.push_back(rank);
			}
		}
		sort(second_hand_kickers.begin(), second_hand_kickers.end());
		clear_map(rank_count);

		if (first_hand_pair != second_hand_pair) {
			return first_hand_pair > second_hand_pair;
		}

		for (unsigned int i = first_hand_kickers.size() - one; i >= zero; --i) {
			if (first_hand_kickers[i] != second_hand_kickers[i]) {
				return first_hand_kickers[i] > second_hand_kickers[i];
			}
		}

		return hands_are_equal;
	}

	return hands_are_equal;
}

/*Returns a vector of the indices of cards to remove if the hand is a four of a kind or a two pair*/
vector<size_t> fourOfAKind_or_twoPair_DiscardIndices(vector<Card> hand) {
	vector<size_t> result;

	update_map(rank_count, hand);

	size_t singleCardRank;

	//find the single card in the map and store the rank 
	for (map<int, int>::iterator it = rank_count.begin(); it != rank_count.end(); ++it) {

		int rank = it->first;

		if (rank_count.find(rank)->second == one) {
			singleCardRank = rank;
			break;
		}
	}

	//find the single card in the hand, add the index to the vector containing the valid indices
	for (size_t i = zero; i < hand.size(); ++i) {
		if (hand[i].rank == singleCardRank) {
			result.push_back(i);
			break;
		}
	}

	clear_map(rank_count);

	return result;
}

/*Returns a vector of the indices of cards to remove if the hand is a pair*/
vector<size_t> onePairDiscardIndices(vector<Card> hand) {
	vector<size_t> result;

	update_map(rank_count, hand);

	bool firstCardFound = false;
	bool secondCardFound = false;

	size_t singleCard1;
	size_t singleCard2;
	size_t singleCard3;

	for (map<int, int>::iterator it = rank_count.begin(); it != rank_count.end(); ++it) {
		int rank = it->first;

		if (rank_count.find(rank)->second == one && !firstCardFound && !secondCardFound) {
			singleCard1 = rank;
			firstCardFound = true;
		}
		else if (rank_count.find(rank)->second == one && firstCardFound && !secondCardFound) {
			singleCard2 = rank;
			secondCardFound = true;
		}
		else if (rank_count.find(rank)->second == one && firstCardFound && secondCardFound) {
			singleCard3 = rank;
		}

	}

	clear_map(rank_count);

	for (size_t i = zero; i < hand.size(); ++i) {
		if (hand[i].rank == singleCard1 || hand[i].rank == singleCard2 || hand[i].rank == singleCard3) {
			result.push_back(i);
		}
	}

	return result;
}

/*Returns a vector of the indices of cards to remove if the hand is a three of a kind*/
vector<size_t> threeOfAKindDiscardIndices(vector<Card> hand) {

	vector<size_t> result;

	update_map(rank_count, hand);
	
	bool firstCardFound = false;   //true if the first single card is found
	size_t singleCard1;            //rank of first single card
	size_t singleCard2;			   //rank of second single card

	//go through the map, get the ranks of the 2 single cards
	for (map<int, int>::iterator it = rank_count.begin(); it != rank_count.end(); ++it) {
		int rank = it->first;

		if (rank_count.find(rank)->second == one && !firstCardFound) {
			singleCard1 = rank;
			firstCardFound = true;
		}

		if (rank_count.find(rank)->second == one && firstCardFound) {
			singleCard2 = rank;
		}

	}

	clear_map(rank_count);


	//go through the hand, push back the appropriate indices to remove
	for (size_t i = zero; i < hand.size(); ++i) {
		if (hand[i].rank == singleCard1 || hand[i].rank == singleCard2) {
			result.push_back(i);
		}
	}

	return result;
}

/*Returns a vector of the indices of cards to remove if the hand has no rank*/
vector<size_t> noHandDiscardIndices(vector<Card> hand) {
	
	vector<size_t> result;

	//remove the lowest 3 cards
	result.push_back(0);
	result.push_back(1);
	result.push_back(2);

	//if the last card is an ace, remove the the 2nd highest card
	if (hand[hand.size() - 1].rank == Card::ace) {
		result.push_back(3);
	}

	return result;
}