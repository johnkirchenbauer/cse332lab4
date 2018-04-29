/*
* Name: Game.h
* Authors: Alex Bakus, John Kirchenbauer, Zach Miller
* Description: This file contains the declarations for the Game class, which is an abstract class meant to be used as
* the base class for different types of card games.
*/

#ifndef GAME_H
#define GAME_H

#include <memory>
#include "deck.h"
#include "Player.h"
#include <vector>
#include <string>

// enum created to avoid hard coding
enum numbersUsed {
	zero,
	one,
	two,
	three,
	four,
	five,
	six,
	seven,
	eight,
	nine,
	ten,
	eleven,
	twelve,
	thirteen,
	twenty = 20,
	twentythree = 23,
	twentyfour = 24,
	negOne = -1
};	



class Game {

protected:
	static shared_ptr<Game> p;
	Deck mainDeck;
	vector<shared_ptr<Player>> players;
	

public:
	
	static shared_ptr<Game> instance();
	static void start_game(const string & s);
	static void stop_game();
	void add_player(const string & s);
	shared_ptr<Player> find_player(const string & s);
	void emptyPlayers();
	virtual ~Game();
	virtual int before_turn(Player & p) = 0;
	virtual int turn(Player & p) = 0;
	virtual int after_turn(Player & p) = 0;
	virtual int betting_one() = 0;
	virtual int betting_zero() = 0;
	virtual int before_round() = 0;
	virtual int round() = 0;
	virtual int after_round() = 0;
};


#endif