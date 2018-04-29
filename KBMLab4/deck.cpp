/*
* Name: deck.cpp
* Author: Zach Miller (zach.miller@wustl.edu)
* Description: This file contains the definitions for the Deck class.  Decks have a vector containing Card objects.  Cards are inserted into the deck with the addCardsFromFile function. The deck
* can be shuffled using the shuffleDeck function
*/


#pragma once

#include "stdafx.h"
#include "deck.h"
#include "Game.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <random>
#include <algorithm>

using namespace std;


//These 2 maps are used by the output stream insertion operator.  The integer values for the suit and rank are mapped to the appropriate strings
map<int, string> rankMap = {
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

map<int, string> suitMap = {
	{ 0, "C" },
	{ 1, "D" },
	{ 2, "H" },
	{ 3, "S" }
};


/*Output stream insertion operator: Use the maps to convert the ints to strings, then insert those strings into the ostream*/
ostream & operator<<(std::ostream& out, const Deck & deck) {
	Deck d = deck;

	//print a message and return if the deck is empty
	if (d.size() == zero) {
		std::cout << "Empty Deck" << endl;
		return out;
	}
	for (size_t i = zero; i != d.size(); ++i) {
		int suit = d.cards[i].suit;
		int rank = d.cards[i].rank;
		std::cout << rankMap.find(rank)->second << suitMap.find(suit)->second << endl;
	}

	return out;
}

/*This method shuffles the cards in the deck*/
void Deck::shuffleDeck() {
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(this->cards.begin(), this->cards.end(), g);
}

/*Returns the size of the member variable, i.e. the number of cards in the deck*/
const int Deck::size() {
	return this->cards.size();
}

/*getter method that returns the cards in the deck*/
vector<Card> & Deck::getCards() {
	return this->cards;
}


void Deck::add_card(Card c) {
	this->cards.push_back(c);
}

