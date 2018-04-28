/*
* Name: Game.cpp
* Authors: Alex Bakus, John Kirchenbauer, Zach Miller
* Description: This file contains the definitions for the Game class, which is an abstract class meant to be used as
* the base class for different types of card games.
*/

#pragma once

#include "stdafx.h"
#include "Game.h"
#include "kbmlab4.h"
#include "FiveCardDraw.h"
#include "SevenCardStud.h"
#include <iostream>
#include <fstream>
#include <vector>


using namespace std;
shared_ptr<Game> Game::p = nullptr;

/*Destructor*/
 Game::~Game() {
	 /*for (vector<shared_ptr<Player>>::iterator it = players.begin(); it != players.end(); ++it) {
		 delete (*it).get();  < ----------------------------------------------------------------------------dis here be rong
	 }*/
}

 /*Returns a copy of the static pointer member variable*/
shared_ptr<Game> Game::instance() {
	//if the pointer is singular, throw an exception
	if (p == nullptr) {
		throw InstanceNotAvailable;
	}
	
	//return a copy of the pointer
	shared_ptr<Game> pointer_copy(p);
	return pointer_copy;

}

/*Dynamically allocates an instance of the FiveCardDraw class and stores its address in the static pointer member variable*/
void Game::start_game(const string & s) {
	
	//if the pointer is not null, i.e. a game already exists, throw an exception
	if (p != nullptr) {
		throw GameAlreadyStarted;
	}

	//search for "FiveCardDraw" in the string, and throw an exception if no match is found
	if ((s.find("FiveCardDraw") != string::npos)) {
		//make new FiveCardDraw, and store its address into the static pointer member variable
		shared_ptr<Game> fcd = make_shared<FiveCardDraw>();
		p = fcd;
	}
	else if (s.find("SevenCardStud") != string::npos) {
		//make new FiveCardDraw, and store its address into the static pointer member variable
		shared_ptr<Game> fcd = make_shared<SevenCardStud>();
		p = fcd;
	}
	else {
		throw UnknownGame;
	}

	
	
}

/*Sets the static pointer member variable to be singular.*/
void Game::stop_game() {

	//if the pointer is singular, i.e. there is no game, throw an exception.
	if (p == nullptr) {
		throw NoGameInProgress;
	}

	p = nullptr;
}

/*Dynamically allocates a Player with that name, and then pushes a pointer to the object back into the game's vector of pointers to players.*/
void Game::add_player(const string & s) {
	

	//compare player names to the input string.  If a match is found, throw an exception.
	for (vector<shared_ptr<Player>>::iterator it = players.begin(); it != players.end(); ++it) {
		string name = (*it)->playerName;
		if (name.compare(s) == 0) {
			throw PlayerAlreadyPlaying;
		}
	}
	
	//add the player

	try {
		shared_ptr<Player> new_player = make_shared<Player>((char *)s.c_str());
		players.push_back(new_player);
	}
	catch (ErrorCode) {
		throw;
	}
	//players.push_back(new_player);

}

/*Returns a pointer (or shared_ptr) to the matching Player object or returns a singular pointer if there is no player with that name.*/
shared_ptr<Player> Game::find_player(const string & s) {

	//go through the players, and try to find a match
	for (vector<shared_ptr<Player>>::iterator it = players.begin(); it != players.end(); ++it) {
		string name = (*it)->playerName;
		if (name.compare(s) == zero) {
			return *it;
		}
	}

	//return a singular pointer if no match is found
	return shared_ptr<Player>(nullptr); 
}

void Game::emptyPlayers() {
	for (vector<shared_ptr<Player>>::iterator it = players.begin(); it != players.end(); ++it) {
		string name = (*it)->playerName;
		string numOfWins = to_string((*it)->hands_won);
		string numOfLosses = to_string((*it)->hands_lost);
		string chipBalance = to_string((*it)->player_chips);


		ofstream newFile(name + ".txt", ofstream::out);
		if (newFile.is_open()) {
			newFile << name << " " << numOfWins << " " << numOfLosses << " " << chipBalance;
		}
		else {
			std::cout << "Unable to Open File For This Player: " << name << endl;

		}
		newFile.close();
		players.erase(it);

		std::cout << "Player has left the game: " << name << endl;




	}
}