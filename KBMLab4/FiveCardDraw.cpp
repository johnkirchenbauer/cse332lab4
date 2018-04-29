/*
* Name: FiveCardDraw.cpp
* Authors: Alex Bakus, John Kirchenbauer, Zach Miller
* Description: This file contains the definitions for the FiveCardDraw class, which inherits from the abstract class Game, and is meant to
* represent the five-card draw poker variant, where each player is dealt 5 cards and has the choice to discard their cards.
*/

#pragma once

#include "stdafx.h"
#include <stdio.h>
#include "FiveCardDraw.h"
#include "kbmlab4.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "poker_rank_logic.h"
#include "Game.h"
#include <string>
#include <set>


using namespace std;

//arrays used in the constructor to create a standard 52-card deck
Card::Suit suitArr[four] = {
	Card::clubs,
	Card::diamonds,
	Card::hearts,
	Card::spades
};

Card::Rank rankArr[thirteen] = {
	Card::two ,
	Card::three,
	Card::four,
	Card::five,
	Card::six,
	Card::seven,
	Card::eight,
	Card::nine,
	Card::ten,
	Card::jack,
	Card::queen,
	Card::king,
	Card::ace
};

/*Compares size_t objects*/
bool comp(size_t i, size_t j) {
	return i > j;
}

/*Constructor for FiveCardDraw that constructs a 52-card deck and initializes the dealer position to 0*/
FiveCardDraw::FiveCardDraw() : dealerPosition(zero),pot(zero) {

	//create the 52 cards and add them to the deck
	for (int s = zero; s < four; s++) {
		for (int r =zero; r < thirteen; r++) {
			Card c = { suitArr[s],rankArr[r]};
			mainDeck.add_card(c);
		}
	}

}

/*This method receives input from the user telling it which card(s) to discard, and removes those cards from the player's hand*/
int FiveCardDraw::before_turn(Player & player) {

	if (player.still_betting || player.all_in) {
		std::cout << player.playerName << "'s hand: " << player.player_cards << endl;
	}



	string input;					//user input
	vector<char> charsReceived;     //the characters received from the user
	vector<size_t> validIndices;    //takes all the valid characters from charsReceived and converts them to size_t
	string indexBuf;
	char indexChar;

	bool invalidString = false;     //true if the input character has incorrect length
	bool validInput = false;        //true if the user input is of correct length

	enum hand_ranks { no_hand, one_pair, two_pair, three_of_a_kind, straight, flush_hand, full_house, four_of_a_kind, straight_flush };
	vector<Card> cards = player.player_cards.getCards();
	
	

	while ((player.still_betting || player.all_in) && !validInput) {
		std::cout << "Enter the indices of any cards you want to discard (1-5).  Press Enter to keep all cards." << endl;
		getline(cin, input);
	
		istringstream inputStream(input);  

		invalidString = false;

		while (inputStream >> indexBuf) {
			
			invalidString = false;

			//if the length of the word is one, add it to the charsReceived vector, otherwise set the invalidString boolean flag to true.
			if (indexBuf.length() == one) {
				indexChar = indexBuf.at(zero);
				charsReceived.push_back(indexChar);
			}
			else {
				invalidString = true;
			}
		}

		//if a valid number of indices are received, convert them to size_t and add it to the validIndices vector
		if (!invalidString) {
			if (charsReceived.size() <= five && charsReceived.size() >= zero) {

				validInput = true;

				for (size_t i = zero; i < charsReceived.size(); i++) {
					if ((charsReceived[i]) >= 0x31 && (charsReceived[i]) <= 0x35) {
						validIndices.push_back((size_t)((charsReceived[i]-one) - '0'));

					}
					else {
						charsReceived.clear();
						validIndices.clear();
						std::cout << "invalid index provided" << endl;
						validInput = false;
						break;
					}
				}
			}
			else {
				charsReceived.clear();
				validIndices.clear();
				std::cout << "Too many indices provided." << endl;
				validInput = false;
			}
		}
		else {
			charsReceived.clear();
			validIndices.clear();
			std::cout << "Invalid index provided." << endl;
			validInput = false;
		}

		//if the input is valid, remove the appropriate cards from the player's hand
		if (validInput) {
			sort(validIndices.begin(), validIndices.end(), comp);
			for (size_t i = zero; i < validIndices.size(); i++) {
				try {
					discardDeck.add_card(player.player_cards[validIndices[i]]);
					player.player_cards.remove_card(validIndices[i]);
				}
				catch (...) {
					throw;
				}

			}
			return Success;
		}

	}
	return Success;
}

/*This method deals cards to players who have less than 5 cards.*/
int FiveCardDraw::turn(Player & p) {

	while (p.player_cards.size() < five) {

		//if neither deck has enough cards, throw an exception, otherwise move the card from the deck to the player's hand
		if (mainDeck.size() <= zero) {

			if (discardDeck.size() <= zero) {
				throw TooFewCards;
				
			}
			else {
				discardDeck.shuffleDeck();
				p.player_cards << discardDeck;
			}

		}

		p.player_cards << mainDeck;
	}
	return Success;
}

/*This method prints out each player's hand after the round*/
int FiveCardDraw::after_turn(Player & p) {
	std::cout << p.playerName << ": " << p.player_cards << endl;
	return zero;	
}

/*Calls the main deck member variable's shuffle method and then deals one card at a time from the main 
**deck to each player, continuing to iterate through the players until each player has received five cards.
**/
int FiveCardDraw::before_round() {

	std::cout << endl << "Round starting!" << endl;

	size_t numPlayers = players.size();

	//check if there are a valid number of players
	if (numPlayers < two) {
		throw TooFewPlayers;
	}
	else if (numPlayers > ten) {
		throw TooFewCards;
	}

	std::cout << players[dealerPosition]->playerName << " is dealing!"<< endl;

	mainDeck.shuffleDeck();

	size_t dealIndex;
	
	//deal each player 5 cards each from the deck
	for (size_t numCards = zero; numCards < five; numCards++) {
		for (size_t i = one; i <= numPlayers; ++i) {
			dealIndex = (dealerPosition + i) % numPlayers;
			players[dealIndex]->player_cards << mainDeck;
		}
	}

	//betting round
	cout << "First Round of betting starts" << endl;
	betting_one();

	//call before_turn for each player
	for (size_t i = one; i <= numPlayers; ++i) {
		dealIndex = (dealerPosition + i) % numPlayers;
		before_turn(*players[dealIndex]);
	}
	std::cout << endl;

	return Success;
}

int FiveCardDraw::betting_zero() {

	for (int i = zero; i < players.size(); i++) {
		
		players[i]->player_chips--;
		
		if (players[i]->player_chips == zero) {
			cout << players[i]->playerName << " ante'd up and is all in!" << endl;
		}
	}
	
	pot += players.size();
	
	return Success;
}

enum BetterTypes {
	Raise2,
	Raise1,
	Call,
	Check,
	AllIn,
	Place2,
	Place1,
	Fold
};

bool get_response() {
	bool validResponse = false;
	string ans;
	bool resp;
	while (!validResponse) {
		getline(cin, ans);
		if (ans == "yes") {
			validResponse = true;
			resp = true;
		}else if (ans == "no") {
			validResponse = true;
			resp = false;
		}else {
			cout << "Please answer 'yes' or 'no'!" << endl;
		}
	}
	return resp;
}

int get_one_two() {
	bool validResponse = false;
	string ans;
	int resp;
	while (!validResponse) {
		getline(cin, ans);
		if (ans == "one") {
			validResponse = true;
			resp = one;
		}
		else if (ans == "two") {
			validResponse = true;
			resp = two;
		}
		else if (ans == "no") {
			validResponse = true;
			resp = zero;
		}
		else {
			cout << "Please answer 'one' , 'two' , or 'no'!" << endl;
		}
	}
	return resp;
}

int FiveCardDraw::betting_one() {

	set<int> betterIndices;
	int callers = zero;
	for (int i = zero; i < players.size(); i++) {
		if (players[i]->still_betting && !players[i]->all_in) {
			betterIndices.insert(i);
		}
	}
	
	int maxBet = zero;
	bool isBet = false;
	
	BetterTypes playerType;
	shared_ptr<Player> currPlayer;
	int i = zero;
	
	while (callers < betterIndices.size()) {

		
		if (betterIndices.find(i) != betterIndices.end()) {
			cout << endl << " ================================================================ " << endl << endl;

			cout << "Betters remaining: ";
			for (auto it = betterIndices.begin(); it != betterIndices.end(); it++) {
				cout << players[*it]->playerName << "(" << players[*it]->player_chips << ") ";
			}
			cout << endl;


			currPlayer = players[i];

			int betDiff = (maxBet - currPlayer->current_bet);
			int playerChips = (currPlayer->player_chips);

			if (isBet) {
				cout << "The current bet is " << maxBet << "." << endl;
				bool needAllChips = false;
				playerChips >= betDiff ? playerType = Call : needAllChips = true;

				if(needAllChips){
					cout << players[i]->playerName << " need's to put in all their chips to call. Would they like to call? (yes or no)" << endl;
					get_response() ? playerType = AllIn : playerType = Fold;
					needAllChips = false;
				}
			}
			else {
				cout << "No bet has been made." << endl;
				(playerChips > zero ? playerType = Check : playerType = AllIn);
			}

			cout << currPlayer->playerName << " has " << currPlayer->player_chips << " chips." << endl;
			cout << players[i]->playerName << "'s hand: " << players[i]->player_cards << endl;

			if (isBet) {
				bool playerFinished = false;
				while (!playerFinished) {
					
					switch (playerType) {
					case Fold:
						cout << currPlayer->playerName << " has folded for this round!" << endl;
						betterIndices.erase(i);
						currPlayer->still_betting = false;
						playerFinished = true;
						break;
					case AllIn:
						cout << currPlayer->playerName << " is All In for this round!" << endl;
						betterIndices.erase(i);
						currPlayer->player_chips = zero;
						pot += currPlayer->player_chips;
						currPlayer->all_in = true;
						playerFinished = true;
						break;
					case Call:
						cout << "It will cost " << currPlayer->playerName <<" "<<(maxBet - currPlayer->current_bet) << " chips to Call on the current bet." << endl;
						cout << "Would " << currPlayer->playerName << " like to Call? (yes or no)" << endl;
						if (get_response()) {
							currPlayer->player_chips -= (maxBet - currPlayer->current_bet);
							pot += (maxBet - currPlayer->current_bet);
							currPlayer->current_bet = maxBet;
							if (currPlayer->player_chips >= two) { playerType = Raise2; }
							else if (currPlayer->player_chips == one) { playerType = Raise1; }
							else { playerType = AllIn; }
						}
						else {
							playerType = Fold;
						}
						break;
					case Raise1:
						cout << currPlayer->playerName << " has enough chips to raise the current bet by one chip." << endl;
						cout << "Would " << currPlayer->playerName << " like to Raise the bet by one chip? (yes or no)" << endl;
						if (get_response()) {
							cout << "Bet raised by one chip!" << endl;
							maxBet += one;
							currPlayer->player_chips -= (maxBet - currPlayer->current_bet);
							pot += (maxBet - currPlayer->current_bet);
							currPlayer->current_bet = maxBet;
							callers = zero;
							playerType = AllIn;
						}
						else {
							playerFinished = true;
						}
						break;
					case Raise2:
						cout << currPlayer->playerName << " has enough chips to raise the current bet by one or two chips." << endl;
						cout << "Would " << currPlayer->playerName << " like to Raise the bet by one or two chips? (one, two or no)" << endl;
						int resp = get_one_two();
						if (resp == two) {
							cout << "Bet raised by two chips!" << endl;
							maxBet += two;
							currPlayer->player_chips -= (maxBet - currPlayer->current_bet);
							pot += (maxBet - currPlayer->current_bet);
							currPlayer->current_bet = maxBet;
							callers = zero;
							if (currPlayer->player_chips == zero) { playerType = AllIn; }
							else { playerFinished = true; }
						}
						else if (resp == one) {
							cout << "Bet raised by one chip!" << endl;
							maxBet += one;
							currPlayer->player_chips -= (maxBet - currPlayer->current_bet);
							pot += (maxBet - currPlayer->current_bet);
							currPlayer->current_bet = maxBet;
							callers = one;
							if (currPlayer->player_chips == one) { playerType = AllIn; }
							else { playerFinished = true; }
						}
						else {
							playerFinished = true;
						}
						break;
					}
				}
			}

			else {
				bool playerFinished = false;
				while (!playerFinished) {
					
					switch (playerType) {
					case Fold:
						cout << currPlayer->playerName << " has folded for this round!" << endl;
						betterIndices.erase(i);
						currPlayer->still_betting = false;
						playerFinished = true;
						break;
					case AllIn:
						cout << currPlayer->playerName << " is All In for this round!" << endl;
						betterIndices.erase(i);
						pot += currPlayer->player_chips;
						currPlayer->player_chips = zero;
						currPlayer->all_in = true;
						playerFinished = true;
						break;
					case Check:
						if (currPlayer->player_chips >= one) {
							cout << currPlayer->playerName << ", would you like to place the first Bet? (yes or no)" << endl;
							if (get_response()) {
								if (currPlayer->player_chips > one) { playerType = Place2; }
								else if (currPlayer->player_chips == one) { playerType = Place1; }
								break;
							}
						}
						cout << currPlayer->playerName << ", would you like to Check?  (yes or no)" << endl;
						if (get_response()) {
							if (currPlayer->player_chips == zero) { playerType = AllIn; }
							else{ playerFinished = true; }
						}
						else {
							playerType = Fold;
						}
						break;
					case Place1:
						cout << "Would " << currPlayer->playerName << " like to place the initial bet at one chip? (yes or no)" << endl;
						if (get_response()) {
							cout << "Starting bet is one chip!" << endl;
							maxBet += one;
							currPlayer->player_chips -= (maxBet - currPlayer->current_bet);
							pot += (maxBet - currPlayer->current_bet);
							currPlayer->current_bet = maxBet;
							callers = zero;
							isBet = true;
							playerType = AllIn;
						}
						else {
							playerFinished = true;
						}
						break;
					case Place2:
						cout << "Would " << currPlayer->playerName << " like to place the initial bet of one or two chips? (one, two, or no)" << endl;
						int resp = get_one_two();
						if (resp == two) {
							cout << "Starting bet is two chips!" << endl;
							maxBet += two;
							currPlayer->player_chips -= (maxBet - currPlayer->current_bet);
							pot += (maxBet - currPlayer->current_bet);
							currPlayer->current_bet = maxBet;
							callers = zero;
							isBet = true;
							if (currPlayer->player_chips == zero) { playerType = AllIn; }
							else { playerFinished = true; }
						}
						else if (resp == one) {
							cout << "Starting bet is one chip!" << endl;
							maxBet += one;
							currPlayer->player_chips -= (maxBet - currPlayer->current_bet);
							pot += (maxBet - currPlayer->current_bet);
							currPlayer->current_bet = maxBet;
							callers = zero;
							isBet = true;
							if (currPlayer->player_chips == zero) { playerType = AllIn; }
							else { playerFinished = true; }
						}
						else {
							playerFinished = true;
						}
						break;
					}
				}
			}
			
			if(betterIndices.find(i) != betterIndices.end()) {
				callers++;
			}
		}

	i = (i + one) % (players.size());
	}
	
	return Success;
}
 
/*Iterates through the players and at each player calling their turn method and then their after_turn method*/
int FiveCardDraw::round() {

	size_t turnIndex;
	size_t numPlayers = players.size();

	//call turn and after_turn methods for each player
	for (size_t i = one; i <= numPlayers; ++i) {
		turnIndex = (dealerPosition + i) % numPlayers;

		if (players[turnIndex]->still_betting || players[turnIndex]->all_in) {
			int turnResult = turn(*players[turnIndex]);

			if (turnResult != zero) {
				return Failure;
			}

			int afterTurnResult = after_turn(*players[turnIndex]);

			if (afterTurnResult != zero) {
				return Failure;
			}
		}

	}

	//second betting round
	for (int i = zero; i < players.size(); i++) {
		players[i]->current_bet = zero;
	}

	cout << "Second round of betiing starting" << endl;
	betting_one();

	return Success;
}

/*Increments wins and losses, prints out the result of the round, moves all players' cards back to the deck, asks if any player 
**wants to leave the game, and asks if any player wants to join the game.
*/
int FiveCardDraw::after_round() {

	//make copy so we don't mess with deal order
	vector<shared_ptr<Player>> copyOfPlayers = players;

	//sort the hands based on their poker rank
	sort(copyOfPlayers.begin(), copyOfPlayers.end(), [](shared_ptr<Player> p1, shared_ptr<Player> p2) {
		return poker_rank(p1->player_cards, p2->player_cards);
	});
	std::cout << endl;

	vector<string> stringsReceived;  //names of players that will be removed

	int j = zero;
	while (j < copyOfPlayers.size()) {
		if (copyOfPlayers[j]->still_betting == true || copyOfPlayers[j]->all_in == true) {
			break;
		}
		j++;
	}

	//increment score for winner, decrement score for losers
	for (size_t i = j; i < copyOfPlayers.size(); i++) {
		if (!poker_rank(copyOfPlayers[j]->player_cards, copyOfPlayers[i]->player_cards)) {
			//block not still betting
			if (copyOfPlayers[i]->still_betting == true || players[i]->all_in == true) {
				std::cout << "Winner: " << copyOfPlayers[i]->playerName << endl;
				copyOfPlayers[i]->hands_won++;
				copyOfPlayers[i]->player_chips += pot;
				cout << "Pot size: " << pot << endl;
				pot = zero;
			}
		}

		else {
			copyOfPlayers[i]->hands_lost++;
		}
	
	}
	std::cout << endl;

	//print out the player hands
	for (size_t i = zero; i < copyOfPlayers.size(); i++) {
		if ((copyOfPlayers[i]->still_betting == true) || (copyOfPlayers[i]->all_in == true)) {
			std::cout << *copyOfPlayers[i] << endl;
			std::cout << copyOfPlayers[i]->player_cards << endl;
		}
	}
	std::cout << endl;

	//transfer the cards from the player hands back to the deck
	for (size_t i = zero; i < copyOfPlayers.size(); i++) {
		for (int j = four; j >= zero; j--) {

			size_t k = j;

			//move cards back into the main deck
			mainDeck.add_card(copyOfPlayers[i]->player_cards[k]);

			//delete cards from player hands
			copyOfPlayers[i]->player_cards.remove_card(k);

		}
	}

	//move cards from the discard deck to the main deck
	for (size_t i = discardDeck.size(); i > zero; i--) {
		mainDeck.add_card(discardDeck.getCards()[i-one]);
		discardDeck.getCards().pop_back();
	}

	//update the dealer position
	dealerPosition = (dealerPosition + one) % players.size();

	std::cout << "Round finished!" << endl;

	//reset all betting fields
	for (int i = zero; i < players.size(); i++) {
		players[i]->still_betting = true;
		players[i]->all_in = false;
		players[i]->current_bet = zero;
	}
	
	string input;
	string response;				 //response from user
	bool removalFinished = false;    //true if removing players step is complete
	bool additionFinished = false;   //true if adding players step is complete
	
	//vector<string> stringsReceived;  //stores the user's response word-by-word

	//check chip counts and reup or boot players
	for (int i = zero; i < players.size(); i++) {
		if (players[i]->player_chips <= zero) {
			cout << "Player: " << players[i]->playerName << " does not have enough chips to continue playing. Would they like to add more chips? (yes or no)" << endl;
			if (get_response()) {
				players[i]->player_chips = twenty;
				cout << "Player: " << players[i]->playerName << "'s chips generously re-upped to 20!" << endl;
			}
			else {
				stringsReceived.push_back(players[i]->playerName);
			}
		}
	}

	while (!removalFinished) {

		std::cout << "Enter the names of players who would like to leave the game, or enter 'no'" << endl;
		getline(cin, input);

		istringstream inputStream(input);
	
		//store the words into the vector
		while (inputStream >> response) {
			stringsReceived.push_back(response);
		}

		//if the response is 'no', then move on.  Otherwise, parse the response and remove the appropriate players
		if (stringsReceived.size() == one && (response == "no" || response == "No")) {
				removalFinished = true;
		}
		else {
			for (size_t i = zero; i < stringsReceived.size(); i++) {

				shared_ptr<Player> currPlayer = Game::find_player(stringsReceived[i]);

				if (currPlayer != nullptr) {

					for (vector<shared_ptr<Player>>::iterator it = players.begin(); it != players.end(); ++it) {
						string name = (*it)->playerName;
                        string numOfWins = to_string((*it)->hands_won);
                        string numOfLosses = to_string((*it)->hands_lost);
						string chipBalance = to_string((*it)->player_chips);

						if (name.compare(stringsReceived[i]) == zero) {
							ofstream newFile(name + ".txt", ofstream::out);
							if (newFile.is_open()) {
								newFile << name << " " << numOfWins << " " << numOfLosses << " " << chipBalance;
							}
							else {
								std::cout << "Unable to Open File For This Player: " << stringsReceived[i] << endl;

							}
							newFile.close();
							players.erase(it);

							std::cout << "Player has left the game: " << name << endl;
							break;
						
							
						}
					}
				}
			}
		}
		stringsReceived.clear();

		if (players.size() == zero) {
			removalFinished = true;
		}
	}

	while (!additionFinished) {

		std::cout << "Enter the names of players who would like to join the game, or enter 'no'" << endl;

		getline(cin, input);

		istringstream inputStream(input);

		//store the user response into the vector
		while (inputStream >> response) {
			stringsReceived.push_back(response);
		}

		//if the response is 'no', move on.  Otherwise, parse the response and add the appropriate players to the game.
		if (stringsReceived.size() == one && (response == "no" || response == "No")) {
			additionFinished = true;
		}
		else {
			for (size_t i = zero; i < stringsReceived.size(); i++) {

				try {
					//shared_ptr<Player> currPlayer = make_shared<Player>((char *)(stringsReceived[i].c_str()));
					Game::add_player(stringsReceived[i]);
				}
				catch (ErrorCode e) {
					program_Usage(e);
				}
				
				
			}
		}
		stringsReceived.clear();
	}
	
	//check if there are a valid number of players
	if (additionFinished) {
		if (players.size() == zero) {
			throw AllPlayersLeft;
		}
		if (players.size() < two) {
			throw TooFewPlayers;
		}
	}

	return Success;
}


