/*
* Name: hand.h
* Author: Zach Miller (zach.miller@wustl.edu)
* Description: This file contains the declarations for the Hand class.  Hands have a vector containing Card objects.  Cards are inserted into the hand (from a Deck object) with the insertion operator.
*/


#ifndef HAND_H
#define HAND_H

#include <vector>
#include <string>
#include "deck.h"


using namespace std;

class Hand {
	friend ostream & operator<<(ostream & out, const Hand & hand);  //ostream insertion operator
	friend Hand & operator<<(Hand & hand, Deck & deck);				//insertion operator that inserts a card from the deck into the hand
	friend void add_card(Hand & h1, Hand & h2, Deck & deck);
public:
	Hand(){};	//default constructor
	Hand(const Hand & h);	//copy constructor
	Hand(vector<Card> cards);
	Hand & operator=(const Hand & h);  //assignment operator
	bool operator== (const Hand & h) const; //equivalence operator
	bool operator<(const Hand & h) const;  //less-than operator
	const int size();   //returns the size of the 'cards' member variable
	const string as_String();   //returns the string of card definitions from the 'cards' member variable
	vector<Card> & getCards();  //getter method for 'cards'
	Card operator[](size_t index);
	void remove_card(size_t index);
	void clear_hand();
	void add_card(Card c);

private:
	vector<Card> cards;   //the cards in the hand
};

#endif