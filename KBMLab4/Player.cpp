/*
* Name: Player.cpp
* Authors: Alex Bakus, John Kirchenbauer, Zach Miller
* Description: This file contains the definitions for the Player struct.  Each player has a name, tallies for rounds won and lost,
* and a hand
*/

#pragma once

#include "stdafx.h"
#include "player.h"
#include "kbmlab4.h"
#include <sstream>
#include <fstream>
#include<cstring>
#include <string>
#include <iostream>
#include "Game.h"

using namespace std;

/*Constructor that initializes a Player's name member variable, and initializes its wins and losses member variables*/
Player::Player(char * name) :  playerName(name), hands_won(zero), hands_lost(zero), player_cards(),faceup_cards(), facedown_cards(), player_chips(twenty),still_betting(true),current_bet(0),all_in(false) {
	bool isBot = false;



	if (name[strlen(name)-1] == '*') {
		name[strlen(name) - 1] = 0;
		isBot = true;
	}

	if (string(name) == "no" || string(name) == "No" ) {
		throw InvalidName;
	}

	//check non-alphanumeric characters
	for (int i = 0; i < strlen(name); i++)
	{
		if (!isalnum(name[i])) {
			throw InvalidName;
		}
	}

	ifstream file;

	string path = string(name) + ".txt";
	file.open(path);

	//Check if the function was able to open the file. 
	if (!file.is_open()) {
		if (isBot) {
			this->playerName = string(name) + '*';
		}
		return;
	}

	string word;  //variable used to store each word in the file

	vector<string> player_data;

	//store each word into a vector
	while (file >> word) {
		player_data.push_back(word);
	}

	//if the format of the file is invalid, return
	if (player_data.size() != four) {
		return;
	}

	//store the info into the appropriate member variable

	if (isBot) {
		this->playerName = player_data[zero] + '*';
	}
	else {
		this->playerName = player_data[zero];
	}

	this->hands_won = stoi(player_data[one]);
	this->hands_lost = stoi(player_data[two]);

	int curr_chips = stoi(player_data[three]);

	if (curr_chips > 0) {
		this->player_chips = curr_chips;
	}
	else {
		cout << this->playerName <<" needs a non zero chip balance to play. Would they like to add more chips? (yes or no)" << endl;
		string ans;
		bool validResponse = false;
		while (!validResponse) {

			getline(cin, ans);

			if (ans == "yes") {
				this->player_chips = 20;
				validResponse = true;
			}
			else if (ans == "no") {
				throw "BitchBroke";
				//throw or return s.t. player not added
				
			}
			else {
				cout <<"Please answer yes or no!"
					<< "Would you like to add more chips?" << endl;
			}
		}

	}

}

/*Prints out the player's name, number of wins, and number of losses with spaces between them*/
ostream & operator<<(ostream & out, const Player & p) {
	std::cout << "Player Name: " << p.playerName << " Hands Won: " << p.hands_won << " Hands Lost: " << p.hands_lost;
	return out;
}
