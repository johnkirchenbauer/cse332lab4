/*
* Name: hand.cpp
* Author: Zach Miller (zach.miller@wustl.edu)
* Description: This file contains the definitions for the Hand class.  Hands have a vector containing Card objects.  Cards are inserted into the hand (from a Deck object) with the insertion operator.
*/

#pragma once

#include "stdafx.h"
#include "hand.h"
#include "kbmlab4.h"
#include <map>
#include <iostream>
#include <algorithm>
#include "Game.h"

using namespace std;

//These 2 maps are used by the as_String() function.  The integer values for the suit and rank are mapped to the appropriate strings
map<int, string> rank_map = {
	{ 2,  "2" },
	{ 3,  "3" },
	{ 4,  "4" },
	{ 5,  "5" },
	{ 6,  "6" },
	{ 7,  "7" },
	{ 8,  "8" },
	{ 9,  "9" },
	{ 10, "10" },
	{ 11, "J" },
	{ 12, "Q" },
	{ 13, "K" },
	{ 14, "A" }
};

map<int, string> suit_map = {
	{ 0, "C" },
	{ 1, "D" },
	{ 2, "H" },
	{ 3, "S" }
};

/*This method returns the size of the deck (i.e. the number of cards in its member variable*/
const int Hand::size() {
	return this->cards.size();
}

/*Copy constructor: the member variable is overwritten by the member variable of the other hand*/
Hand::Hand(const Hand & h) {
	this->cards = h.cards;
}


Hand::Hand(vector<Card> cards) {
	this->cards = cards;
}

/*Assignment operator:  if the other hand is not 'this', then overwrite the contents of the member variable*/
Hand & Hand::operator=(const Hand & h) {
	//check for self-assignment
	if (this == &h) {
		return *this;
	}
	else {
		//clear the vector and overwrite it
		this->cards.clear();
		this->cards = h.cards;
		return *this;
	}

}

/*Equivalence operator:  goes through each of the cards of the member variable, and if they are at any point different, then false is returned*/
bool Hand::operator==(const Hand & h) const {
	//make non-const copies of the hands
	Hand h1 = *this;
	Hand h2 = h;

	//if the size of the hands are different, return false
	if (h1.cards.size() != h2.cards.size()) {
		return false;
	}

	else {
		//go through the 2 vectors and compare the cards at index i.  If the two cards aren't equal, return false.
		for (unsigned int i = zero; i < h2.cards.size(); ++i) {
			if (h1.cards[i] < h2.cards[i] || h2.cards[i] < h1.cards[i]) {
				return false;
			}
		}
	}
	return true;
}

/*Less-than operator: goes through the 2 hands from left to right, and compares the cards.  If they're the same cards, move on to the next set of cards, otherwise
* return the appropriate bool
*/
bool Hand::operator<(const Hand & h) const {
	
	bool h1_smaller = false;  //true if the first hand has less cards
	bool same_size = true;   //true if both hands have the same size

	//make a copy of the cards
	vector<Card> h1_cards = this->cards;
	vector<Card> h2_cards = h.cards;

	//get the size of the hands
	int h1_size = h1_cards.size();
	int h2_size = h2_cards.size();

	//compare the sizes, update the booleans if necessary
	if (h1_size != h2_size) {
		same_size = false;

		if (h1_size < h2_size) {
			h1_smaller = true;
		}
	}

	//If the hands are equal size or if the first hand is smaller, go through the cards and compare them, returning if necessary
	if (same_size || h1_smaller) {
		for (int i = zero; i < h1_size; ++i) {
			if (h1_cards[i] < h2_cards[i]) {
				return true;
			}
			else if (h2_cards[i] < h1_cards[i]) {
				return false;
			}
		}
		return h1_smaller;
	}

	//If the second hand is smaller, go through the cards and compare them, returning if necessary.
	if (!h1_smaller) {
		for (int i = zero; i < h2_size; ++i) {
			if (h1_cards[i] < h2_cards[i]) {
				return true;
			}
			else if (h2_cards[i] < h1_cards[i]) {
				return false;
			}
		}
		return false;
	}




	

	return false;
}

/*This method converts the member variable to a string of card definitions separated by spaces, e.g 6h 7h 8h 9h 10h */
const string Hand::as_String() {

	//return error if the vector is empty
	if (this->cards.size() == zero) {
		return "Empty Hand";
	}

	string result = "";  //start building the string

	//go through the vector, and use the rank map and suit map to convert the ints to strings
	for (vector<Card>::iterator it = this->cards.begin(); it != this->cards.end(); ++it) {
		Card c = *it;
		int suit = c.suit;
		int rank = c.rank;
		result += rank_map.find(rank)->second + suit_map.find(suit)->second + " ";
	} 
	
	return result;
}

/*Insertion operator: calls the as_String() method, and inserts it into an output stream*/
ostream & operator<<(ostream & out, const Hand & hand) {
	Hand h = hand;
	std::cout << h.as_String();
	return out;
}

/*Insertion operator: inserts a card from the deck into the hand*/
Hand & operator<<(Hand & hand, Deck & deck) {

	//if the deck has no more cards, return the hand
	if (deck.size() == zero) {
		return hand;
	}

	hand.cards.push_back(deck.getCards()[deck.size() - one]);  //push the last card on the deck to the hand
	deck.getCards().pop_back();								 //remove the last card from the deck
	sort(hand.cards.begin(), hand.cards.end());				 //sort the hand

	return hand;
}


void add_card(Hand & h1, Hand & h2, Deck & deck) {
	if (deck.size() == zero) {
		return;
	}

	h1.cards.push_back(deck.getCards()[deck.size() - one]);  //push the last card on the deck to the hand
	h2.cards.push_back(deck.getCards()[deck.size() - one]);
	deck.getCards().pop_back();								 //remove the last card from the deck
	sort(h1.cards.begin(), h1.cards.end());				 //sort the hand
	sort(h2.cards.begin(), h2.cards.end());

}

/*getter method that returns the member variable*/
vector<Card> & Hand::getCards() {
	return this->cards;
}


Card Hand::operator[](size_t index) {
	if (index >= this->cards.size() || index < zero || this->cards.size() == zero) {
		throw "Invalid indexing";
	}
	//std::cout << "[] operator size_t is: " << index << endl;
	return this->cards[index];
}

void Hand::remove_card(size_t index) {
	if (index >= this->cards.size() || index < zero || this->cards.size() == zero) {
		throw "no card to remove at this index";
	
	}
	//std::cout << "remove_card() size_t is: " << index << endl;
	this->cards.erase(this->cards.begin() + index);
	
}

void Hand::clear_hand() {
	this->cards.clear();
}

void Hand::add_card(Card c) {
	this->cards.push_back(c);
	sort(this->cards.begin(), this->cards.end());
}