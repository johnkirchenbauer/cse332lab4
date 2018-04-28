// KBMLab4.cpp : Defines the entry point for the console application.
/*

* Authors: Alex Bakus, John Kirchenbauer, Zach Miller
* Description: This file contains the main function of the program
**/

#pragma once
#include "stdafx.h"
#include "card.h"
#include "hand.h"
#include "deck.h"
#include "poker_rank_logic.h"
#include "FiveCardDraw.h"
#include "Player.h"
#include "Game.h"
#include "kbmlab4.h"
#include <iostream>
#include <map>
#include <algorithm>

using namespace std;

int main(int argc, char * argv[]) {

	//hardcode for debugging
	argc = 5;
	argv[1] = "FiveCardDraw";
	argv[2] = "alex";
	argv[3] = "john";
	argv[4] = "zach";


	// check the number of arguments from user, need at least 4 and at most 12 with one deck of cards
	if (argc < four) {
		program_Usage(BadInput);
		return BadInput;
	}
	else if (argc > twelve) {
		program_Usage(TooFewCards);
		return TooFewCards;
	}

	//initialize the game specified by the user, placed in try/catch block to catch any propogated errors in main
	try {
		Game::start_game(argv[one]);
	}
	catch (ErrorCode e) {
		program_Usage(e);
		return e;
	}

	bool gameInProg = true;
	//start the game and deal cards to players, placed in try/catch block to catch any propogated errors in main
	try {

		shared_ptr<Game> gamePtr = Game::instance();

		try {
			for (int j = 2; j < argc; j++) {
				gamePtr->add_player(argv[j]);
			}
		}
		catch (ErrorCode e) {
			program_Usage(e);

		}
		//run the game, placed in try/catch block to catch any propogated errors in main
		while (gameInProg) {

			try {
				gamePtr->betting_zero();
				gamePtr->before_round();
				gamePtr->round();
				gamePtr->after_round();

			}
			catch (ErrorCode) {
				gameInProg = false;
				throw;
			}
		}
	}
	// catch any error codes thrown, stop the game, and echo that the game is complete
	catch (ErrorCode e) {
		//if the specific error code is everyone left the game, then the game terminated with no errors
		if (e == AllPlayersLeft) {
			program_Usage(e);
			Game::stop_game();
			std::cout << "Game finished." << endl;
			return e;
		}
		// all other caught errors terminates the game and are propogated out to the user
		else {
			program_Usage(e);
			Game::stop_game();
			std::cout << "Game terminated with error." << endl;
			return e;
		}
	}
}

// function that describes all the possible error codes thrown and caught in main
int program_Usage(ErrorCode code) {

	switch (code) {

	case Success:
		break;
	case BadInput:
		std::cout << endl << "ErrorCode[BadInput] : Incorrect number of inputs!" << endl
			<< "lab2.exe [GameType] [player names ... ]" << endl
			<< "Run this program with a single GameType argument" << endl
			<< "Followed by a list of players, minimum two, to add to the game sep. by whitespace." << endl
			<< "ErrorCodes[0:Success, 1:BadInput,  2:FileNotOpen 3:TooFewPlayers, 4:TooFewCards," << endl
			<< "5:InstanceNotAvailable, 6:GameAlreadyStarted, 7:NoGameInProgress, 8:PlayerAlreadyPlaying, 9:UnknownGame]" << endl << endl;
		break;
	case FileNotOpen:
		std::cout << endl << "ErrorCode[FileNotOpen] : Found corrupt file when reading in player history!" << endl
			<< "lab2.exe [GameType] [player names ... ]" << endl
			<< "Run this program with a single GameType argument" << endl
			<< "Followed by a list of players, minimum two, to add to the game sep. by whitespace." << endl
			<< "ErrorCodes[0:Success, 1:BadInput,  2:FileNotOpen 3:TooFewPlayers, 4:TooFewCards," << endl
			<< "5:InstanceNotAvailable, 6:GameAlreadyStarted, 7:NoGameInProgress, 8:PlayerAlreadyPlaying, 9:UnknownGame]" << endl << endl;
		break;
	case TooFewPlayers:
		std::cout << endl << "ErrorCode[NotEnoughPlayers] : Too few players to continue game!" << endl
			<< "lab2.exe [GameType] [player names ... ]" << endl
			<< "Run this program with a single GameType argument" << endl
			<< "Followed by a list of players, minimum two, to add to the game sep. by whitespace." << endl
			<< "ErrorCodes[0:Success, 1:BadInput,  2:FileNotOpen 3:TooFewPlayers, 4:TooFewCards," << endl
			<< "5:InstanceNotAvailable, 6:GameAlreadyStarted, 7:NoGameInProgress, 8:PlayerAlreadyPlaying, 9:UnknownGame]" << endl << endl;
		break;
	case TooFewCards:
		std::cout << endl << "ErrorCode[TooFewCards] : Too few cards for this many players to play game!" << endl
			<< "lab2.exe [GameType] [player names ... ]" << endl
			<< "Run this program with a single GameType argument" << endl
			<< "Followed by a list of players, minimum two, to add to the game sep. by whitespace." << endl
			<< "ErrorCodes[0:Success, 1:BadInput,  2:FileNotOpen 3:TooFewPlayers, 4:TooFewCards," << endl
			<< "5:InstanceNotAvailable, 6:GameAlreadyStarted, 7:NoGameInProgress, 8:PlayerAlreadyPlaying, 9:UnknownGame]" << endl << endl;
		break;
	case InstanceNotAvailable:
		std::cout << endl << "ErrorCode[InstanceNotAvailable] : Issue initializing game!" << endl
			<< "lab2.exe [GameType] [player names ... ]" << endl
			<< "Run this program with a single GameType argument" << endl
			<< "Followed by a list of players, minimum two, to add to the game sep. by whitespace." << endl
			<< "ErrorCodes[0:Success, 1:BadInput,  2:FileNotOpen 3:TooFewPlayers, 4:TooFewCards," << endl
			<< "5:InstanceNotAvailable, 6:GameAlreadyStarted, 7:NoGameInProgress, 8:PlayerAlreadyPlaying, 9:UnknownGame]" << endl << endl;
		break;
	case GameAlreadyStarted:
		std::cout << endl << "ErrorCode[GameAlreadyStarted] : Cannot start game, game already in progress!" << endl
			<< "lab2.exe [GameType] [player names ... ]" << endl
			<< "Run this program with a single GameType argument" << endl
			<< "Followed by a list of players, minimum two, to add to the game sep. by whitespace." << endl
			<< "ErrorCodes[0:Success, 1:BadInput,  2:FileNotOpen 3:TooFewPlayers, 4:TooFewCards," << endl
			<< "5:InstanceNotAvailable, 6:GameAlreadyStarted, 7:NoGameInProgress, 8:PlayerAlreadyPlaying, 9:UnknownGame]" << endl << endl;
		break;
	case NoGameInProgress:
		std::cout << endl << "ErrorCode[NoGameInProgress] : Cannot stop game, no game in progress!" << endl
			<< "lab2.exe [GameType] [player names ... ]" << endl
			<< "Run this program with a single GameType argument" << endl
			<< "Followed by a list of players, minimum two, to add to the game sep. by whitespace." << endl
			<< "ErrorCodes[0:Success, 1:BadInput,  2:FileNotOpen 3:TooFewPlayers, 4:TooFewCards," << endl
			<< "5:InstanceNotAvailable, 6:GameAlreadyStarted, 7:NoGameInProgress, 8:PlayerAlreadyPlaying, 9:UnknownGame]" << endl << endl;
		break;
	case PlayerAlreadyPlaying:
		std::cout << endl << "ErrorCode[PlayerAlreadyPlaying] : Cannot add player to game, player already playing!" << endl;
		break;
	case UnknownGame:
		std::cout << endl << "ErrorCode[UnknownGame] : Unknown game type entered!" << endl
			<< "lab2.exe [GameType] [player names ... ]" << endl
			<< "Run this program with a single GameType argument" << endl
			<< "Followed by a list of players, minimum two, to add to the game sep. by whitespace." << endl
			<< "ErrorCodes[0:Success, 1:BadInput,  2:FileNotOpen 3:TooFewPlayers, 4:TooFewCards," << endl
			<< "5:InstanceNotAvailable, 6:GameAlreadyStarted, 7:NoGameInProgress, 8:PlayerAlreadyPlaying, 9:UnknownGame]" << endl << endl;
		break;
	case AllPlayersLeft:
		std::cout << endl << "All players left the game." << endl
			<< "Player scores recorded and saved." << endl
			<< "Thanks for playing!" << endl;
		break;
	case InvalidName:
		std::cout << "ErrorCode[InvalidName] : Invalid name entered!" << endl
			<< "Please enter names consisting only of alpha-numeric characters." << endl;
		break;

	}
	return code;
}