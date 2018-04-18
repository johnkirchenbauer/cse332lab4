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
FiveCardDraw::FiveCardDraw() : dealerPosition(0),pot(0) {

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

	if (player.playerName.back() != '*' && (player.still_betting || player.all_in)) {
		std::cout << player.playerName << "'s hand: " << player.player_cards << endl;
	}

	bool botPlayer = false;  //true if the player is a bot

	//check if the player is a bot
	if (player.playerName.back() == '*' && (player.still_betting || player.all_in)) {
		botPlayer = true;
		std::cout << player.playerName << " playing (BOT PLAYER)" << endl;
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
	
	int bot_hand = getHandRank(cards);

	if (botPlayer) {
		if (bot_hand == straight || bot_hand == flush_hand || bot_hand == straight_flush || bot_hand == full_house) {
			validIndices.clear();
		}
		else if (bot_hand == four_of_a_kind || bot_hand == two_pair) {
			validIndices = fourOfAKind_or_twoPair_DiscardIndices(cards);
		}
		else if (bot_hand == three_of_a_kind) {
			validIndices = threeOfAKindDiscardIndices(cards);
		}
		else if (bot_hand == one_pair) {
			validIndices = onePairDiscardIndices(cards);
		}
		else if (bot_hand == no_hand) {
			validIndices = noHandDiscardIndices(cards);
		}

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

		//std::cout << player.playerName << " discarded indices: "; 
		//for (int i = 0; i < validIndices.size(); ++i) {
		//	std::cout << validIndices[i] << " ";
		//}
		//std::cout << endl;
	}



	

	while ((player.still_betting || player.all_in) && !validInput && !botPlayer) {
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

	//cout << "Pot: " << pot << endl;
	for (int i = 0; i < players.size(); i++) {
		//cout << players[i]->playerName << " ante'd up 1 chip." << endl;
		//cout << "Chips before: " << players[i]->player_chips << endl;
		players[i]->player_chips--;
		//cout << "Chips after: " << players[i]->player_chips << endl;
		if (players[i]->player_chips == 0) {
			cout << players[i]->playerName << " ante'd up and is all in!" << endl;
		}
	}
	//cout << "Pot before: " << pot << endl;
	pot += players.size();
	//cout << "Pot after: " << pot << endl;
	
	return Success;
}

//Old Garbo
/*
int FiveCardDraw::betting_one() {

	bool isBet = false;
	int maxBet = 0;
	int numBetters = players.size();
	int callers = 0;
	
	int i = 0;
	int j = 99999;
	while(callers < numBetters) {


		cout << "callers before: " << callers << " betters before: " << numBetters << endl;

		cout << endl << "This player, " << players[i]->playerName << "'s chip count: " << players[i]->player_chips << endl;

		bool validResponse = false;

		if (players[i]->still_betting == true) {

			cout << "Top i: " << i << " j: " << j << endl;
			if (((i == j) && (players[j]->current_bet == maxBet)) || (j < 0)) {
				numBetters--;
				cout << "numBetters now: " << numBetters << endl;
			}

			cout << players[i]->playerName << "'s hand: " << players[i]->player_cards << endl; 

			if (isBet) {

				cout << "The current bet is: " << maxBet << endl;

				if ((maxBet - players[i]->current_bet) < players[i]->player_chips) {
					cout << "You need to add " << (maxBet - players[i]->current_bet) << " to call." << endl;
				}
				else if (players[i]->player_chips != 0) {
					cout << "You need to bet all your chips to call." << endl;
					cout << "You have " << players[i]->player_chips << " chips" << endl;

					string ans;
					bool allinResp = false;
					while (!allinResp) {
						
						cout << "Would you like to go all in?" << endl;
						getline(cin, ans);
						if (ans == "yes") {
							allinResp = true;
							validResponse = true;
							players[i]->all_in = true;
							players[i]->still_betting = false;
							numBetters--;
							pot += players[i]->player_chips;
							players[i]->player_chips = 0;
						}
						else if (ans == "no") {
							allinResp = true;
							validResponse = true;
							numBetters--;
							players[i]->still_betting = false;
						}
						else {
							cout << "Invalid response." << endl;
						}
					}
				}
				else {
					players[i]->all_in = true;
					players[i]->still_betting = false;
					numBetters--;
					cout << "You're all in for this round, sit'n it pig" << endl;
					validResponse = true;
				}

				string ans;

				while (!validResponse) {

					//if stmt st the correct options displayed
					cout << "Would you like to call, fold?" << endl;

					getline(cin, ans);

					if (ans == "call") {

						players[i]->current_bet = maxBet;
						players[i]->player_chips -= (maxBet - players[i]->current_bet);
						pot += (maxBet - players[i]->current_bet);
						callers++;

						if (players[i]->player_chips >= 2) {

							string ans;
							bool validRaise = false;
							while (!validRaise) {
								
								cout << "Would you like to raise by one or two chips? (one or two or no)" << endl;
								getline(cin, ans);
								if (ans == "one") {
									j = -1;
									maxBet += 1;
									callers = 1;
									pot += 1;
									players[i]->player_chips -= 1;
									players[i]->current_bet += 1;
									validRaise = true;
									validResponse = true;
									
								}
								else if (ans == "two") {
									j = -1;
									if (players[i]->player_chips == 2) {
										cout << "You're all in!" << endl;
										players[i]->all_in = true;
										players[i]->still_betting = false;
										//numBetters--;
										j = i;
									}
									maxBet += 2;
									callers = 1;
									pot += 2;
									players[i]->player_chips -= 2;
									players[i]->current_bet += 2;
									validRaise = true;
									validResponse = true;
								}
								else if (ans == "no") {
									validRaise = true;
									validResponse = true;
								}
								else {
									cout << "Invalid response" << endl;
								}
							}
						}
						if (players[i]->player_chips == 1) {
							//only enough to raise 1;

							string ans;
							bool validRaise = false;
							while (!validRaise) {
								
								cout << "Would you like to raise by one chip? (one or no)" << endl;
								getline(cin, ans);
								if (ans == "one") {
									j = -1;
									if (players[i]->player_chips == 1) {
										cout << "You're all in!" << endl;
										players[i]->all_in = true;
										players[i]->still_betting = false;
										//numBetters--;
										j = i;
									}
									maxBet += 1;
									callers = 1;
									pot += 1;
									players[i]->player_chips -= 1;
									players[i]->current_bet += 1;
									validRaise = true;
									validResponse = true;
								}
								else if (ans == "no") {
									validRaise = true;
									validResponse = true;
								}
								else {
									cout << "Invalid response" << endl;
								}
							}

						}

					}
					else if (ans == "fold") {
						numBetters--;
						players[i]->still_betting = false;
						validResponse = true;

					}
					else {
						cout << "Please answer 'call' or 'fold'!" << endl;

					}
				}

			}
			else {
				if (players[i]->player_chips != 0) {
					cout << "Would you like to place the first bet or check?" << endl;

					string ans;
					bool validResp = false;
					while (!validResp) {
						getline(cin, ans);

						if (ans == "check") {
							callers++;
							validResp = true;
						}
						else if (ans == "bet") {

							//cout << "Bet for : " << players[i]->playerName << " Chips: " << players[i]->player_chips << endl;
							if (players[i]->player_chips >= 2) {

								string ans;
								bool validBet = false;
								while (!validBet) {

									cout << "Would you like to place an initial bet of one or two chips? (one or two or no)" << endl;
									getline(cin, ans);
									if (ans == "one") {
										maxBet += 1;
										callers = 1;
										pot += 1;
										players[i]->player_chips -= 1;
										players[i]->current_bet += 1;
										validResp = true;
										validBet = true;
										isBet = true;
									}
									else if (ans == "two") {
										if (players[i]->player_chips == 2) {
											cout << "You're all in!" << endl;
											players[i]->all_in = true;
											players[i]->still_betting = false;
											//numBetters--;
											j = i;
										}
										maxBet += 2;
										callers = 1;
										pot += 2;
										players[i]->player_chips -= 2;
										players[i]->current_bet += 2;
										validResp = true;
										validBet = true;
										isBet = true;
									}
									else if (ans == "no") {
										validResp = true;
										validBet = true;

									}
									else {
										cout << "Invalid response" << endl;
									}
								}
							}
							else if(players[i]->player_chips == 1) {
								//only enough to raise 1;

								string ans;
								bool validBet = false;
								while (!validBet) {
									cout << "Would you like place an initial bet of one chip? (one or no)" << endl;
									getline(cin, ans);

									if (ans == "one") {

										if (players[i]->player_chips == 1) {
											cout << "You're all in!" << endl;
											players[i]->all_in = true;
											players[i]->still_betting = false;
											//numBetters--;
											j = i;
										}
										maxBet += 1;
										callers = 1;
										pot += 1;
										players[i]->player_chips -= 1;
										players[i]->current_bet += 1;
										validResp = true;
										validBet = true;
										isBet = true;

									}
									else if (ans == "no") {
										validResp = true;
										validBet = true;

									}
									else {
										cout << "Invalid response" << endl;
									}
								}

							}
							//else {
							//	players[i]->all_in = true;
							//	players[i]->still_betting = false;
							//	callers = 1;
							//	j = i;
							//	//numBetters--;
							//	cout << "You're all in for this round, sit'n it pig" << endl;
							//	validResp = true;
							//}
						}
						else {
							cout << "Invalid response." << endl;
						}
					}
				}
				else {
					cout << "You have zero chips, automatic check and all in!" << endl;
					players[i]->all_in = true;
					players[i]->still_betting = false;
					numBetters--;
				}
			}
		}

		
		i = (i+1) % (players.size());
		//cout << " i: " << i << endl;
		//cout << "Next player: " << players[i]->playerName << endl;
		cout << "callers after:" << callers << ", betters after:" << numBetters << endl;

		cout << "Bottom i: " << i << " j: " << j << endl;
	

	}
	return Success;
}
*/

//John's updaterooney --------------------------------------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/*
BetterTypes better_type(shared_ptr<Player> player, int maxBet, bool isBet) {
int betDiff = (maxBet - player->current_bet);
int playerChips = (player->player_chips);
BetterTypes playerType;

if (isBet) {
if ((betDiff + 2) <= playerChips) {
playerType = Raise2;
}
else if ((betDiff + 1) <= playerChips) {
playerType = Raise1;
}
else if (betDiff <= playerChips) {
playerType = Call;
}
else if (betDiff > playerChips) {
playerType = AllIn;
}
}
else {
if (playerChips >= 2) {
playerType = Place2;
}
else if (playerChips >= 1) {
playerType = Place1;
}
else if (playerChips == 0) {
playerType = AllIn;
}
}
return playerType;
}
*/

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
			resp = 1;
		}
		else if (ans == "two") {
			validResponse = true;
			resp = 2;
		}
		else if (ans == "no") {
			validResponse = true;
			resp = 0;
		}
		else {
			cout << "Please answer 'one' , 'two' , or 'no'!" << endl;
		}
	}
	return resp;
}

int FiveCardDraw::betting_one() {

	set<int> betterIndices;
	int callers = 0;
	for (int i = 0; i < players.size(); i++) {
		betterIndices.insert(i);
	}
	
	int maxBet = 0;
	bool isBet = false;
	
	BetterTypes playerType;
	shared_ptr<Player> currPlayer;
	int i = 0;
	
	while (callers < betterIndices.size()) {

		cout << endl << " ================================================================ " << endl << endl;

		cout << "Betters remaining: ";
		for (auto it = betterIndices.begin(); it != betterIndices.end(); it++) {
			cout << players[*it]->playerName << "(" << players[*it]->player_chips <<") ";
		}
		cout << endl;

		if (betterIndices.find(i) != betterIndices.end()) {

			currPlayer = players[i];

			int betDiff = (maxBet - currPlayer->current_bet);
			int playerChips = (currPlayer->player_chips);

			if (isBet) {
				cout << "The current bet is " << maxBet << "." << endl;
				(playerChips >= betDiff ? playerType = Call : playerType = AllIn);
			}
			else {
				cout << "No bet has been made." << endl;
				(playerChips > 0 ? playerType = Check : playerType = AllIn);
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
						currPlayer->player_chips = 0;
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
							if (currPlayer->player_chips >= 2) { playerType = Raise2; }
							else if (currPlayer->player_chips == 1) { playerType = Raise1; }
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
							maxBet += 1;
							currPlayer->player_chips -= (maxBet - currPlayer->current_bet);
							pot += (maxBet - currPlayer->current_bet);
							currPlayer->current_bet = maxBet;
							callers = 0;
							cout << "Callers reset to 0." << endl;
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
						if (resp == 2) {
							cout << "Bet raised by two chips!" << endl;
							maxBet += 2;
							currPlayer->player_chips -= (maxBet - currPlayer->current_bet);
							pot += (maxBet - currPlayer->current_bet);
							currPlayer->current_bet = maxBet;
							callers = 0;
							cout << "Callers reset to 0." << endl;
							if (currPlayer->player_chips == 0) { playerType = AllIn; }
							else { playerFinished = true; }
						}
						else if (resp == 1) {
							cout << "Bet raised by one chip!" << endl;
							maxBet += 1;
							currPlayer->player_chips -= (maxBet - currPlayer->current_bet);
							pot += (maxBet - currPlayer->current_bet);
							currPlayer->current_bet = maxBet;
							callers = 0;
							cout << "Callers reset to 0." << endl;
							if (currPlayer->player_chips == 0) { playerType = AllIn; }
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
						currPlayer->player_chips = 0;
						currPlayer->all_in = true;
						playerFinished = true;
						break;
					case Check:
						if (currPlayer->player_chips >= 1) {
							cout << currPlayer->playerName << ", would you like to place the first Bet? (yes or no)" << endl;
							if (get_response()) {
								if (currPlayer->player_chips > 1) { playerType = Place2; }
								else if (currPlayer->player_chips == 1) { playerType = Place1; }
								break;
							}
						}
						cout << currPlayer->playerName << ", would you like to Check?  (yes or no)" << endl;
						if (get_response()) {
							if (currPlayer->player_chips == 0) { playerType = AllIn; }
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
							maxBet += 1;
							currPlayer->player_chips -= (maxBet - currPlayer->current_bet);
							pot += (maxBet - currPlayer->current_bet);
							currPlayer->current_bet = maxBet;
							callers = 0;
							cout << "Callers reset to 0." << endl;
							isBet = true;
							playerType = AllIn;
						}
						else {
							playerType = Check;
						}
						break;
					case Place2:
						cout << "Would " << currPlayer->playerName << " like to place the initial bet of one or two chips? (one, two, or no)" << endl;
						int resp = get_one_two();
						if (resp == 2) {
							cout << "Starting bet is two chips!" << endl;
							maxBet += 2;
							currPlayer->player_chips -= (maxBet - currPlayer->current_bet);
							pot += (maxBet - currPlayer->current_bet);
							currPlayer->current_bet = maxBet;
							callers = 0;
							cout << "Callers reset to 0." << endl;
							isBet = true;
							if (currPlayer->player_chips == 0) { playerType = AllIn; }
							else { playerFinished = true; }
						}
						else if (resp == 1) {
							cout << "Starting bet is one chip!" << endl;
							maxBet += 1;
							currPlayer->player_chips -= (maxBet - currPlayer->current_bet);
							pot += (maxBet - currPlayer->current_bet);
							currPlayer->current_bet = maxBet;
							callers = 0;
							cout << "Callers reset to 0." << endl;
							isBet = true;
							if (currPlayer->player_chips == 0) { playerType = AllIn; }
							else { playerFinished = true; }
						}
						else {
							playerType = Check;
						}
						break;
					}
				}
			}
			
			if(betterIndices.find(i) != betterIndices.end()) {
				callers++;
			}
		}

	i = (i + 1) % (players.size());
	}
	
	return Success;
}
//-------------------------------------------------------------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/*Iterates through the players and at each player calling their turn method and then their after_turn method*/
int FiveCardDraw::round() {

	size_t turnIndex;
	size_t numPlayers = players.size();

	//call turn and after_turn methods for each player
	for (size_t i = one; i <= numPlayers; ++i) {
		turnIndex = (dealerPosition + i) % numPlayers;

		if (players[turnIndex]->still_betting || players[turnIndex]->all_in) {
			int turnResult = turn(*players[turnIndex]);

			if (turnResult != 0) {
				return Failure;
			}

			int afterTurnResult = after_turn(*players[turnIndex]);

			if (afterTurnResult != 0) {
				return Failure;
			}
		}

	}

	return Success;
}

/*Increments wins and losses, prints out the result of the round, moves all players' cards back to the deck, asks if any player 
**wants to leave the game, and asks if any player wants to join the game.
*/
int FiveCardDraw::after_round() {

	/*for (int k = 0; k < players.size(); k++) {
		cout << "players[k]," <<players[k]->playerName << " still betting = " << players[k]->still_betting << endl;
		cout << "players[k]," << players[k]->playerName << " all in = " << players[k]->all_in << endl;
	}*/

	//make copy so we don't mess with deal order
	vector<shared_ptr<Player>> copyOfPlayers = players;

	/*for (int k = 0; k < copyOfPlayers.size(); k++) {
		cout << "Copy of players[k]," << copyOfPlayers[k]->playerName << " still betting = " << copyOfPlayers[k]->still_betting << endl;
		cout << "Copy of players[k]," << copyOfPlayers[k]->playerName << " all in = " << copyOfPlayers[k]->all_in << endl;
	}*/

	//sort the hands based on their poker rank
	sort(copyOfPlayers.begin(), copyOfPlayers.end(), [](shared_ptr<Player> p1, shared_ptr<Player> p2) {
		return poker_rank(p1->player_cards, p2->player_cards);
	});
	std::cout << endl;

	vector<string> stringsReceived;  //names of players that will be removed

	unsigned int prob_leave_if_win = 10;
	unsigned int prob_leave_if_bottom = 90;
	unsigned int prob_leave_if_middle = 50;
	vector<string> leavingBots;

	int j = 0;
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
				cout << "BootyLoot: " << pot << endl;
				pot = 0;
			}
			//10% probability of bot leaving if it won the hand
			if (copyOfPlayers[i]->playerName.back() == '*') {
				unsigned int randNum = rand() % 100;

				if (randNum < prob_leave_if_win) {
					leavingBots.push_back(copyOfPlayers[i]->playerName);
				}
			}
		}

		else {
			copyOfPlayers[i]->hands_lost++;

			//90% probability of bot leaving if it had the worst hand, and 50% if they have neither the best nor the worst hand
			if (copyOfPlayers[i]->playerName.back() == '*') {
				if (i == copyOfPlayers.size() - 1) {
					unsigned int randNum = rand() % 100;

					if (randNum < prob_leave_if_bottom) {
						leavingBots.push_back(copyOfPlayers[i]->playerName);
					}
				}
				else {
					unsigned int randNum = rand() % 100;

					if (randNum < prob_leave_if_middle) {
						leavingBots.push_back(copyOfPlayers[i]->playerName);
					}
				}
			}

		}
	
	}
	std::cout << endl;

	//print out the player hands
	for (size_t i = zero; i < copyOfPlayers.size(); i++) {
		/*cout << "Copy of players[i]," << copyOfPlayers[i]->playerName << " still betting = " << copyOfPlayers[i]->still_betting << endl;
		cout << "Copy of players[i]," << copyOfPlayers[i]->playerName << " all in = " << copyOfPlayers[i]->all_in << endl;*/
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
	for (int i = 0; i < players.size(); i++) {
		players[i]->still_betting = true;
		players[i]->all_in = false;
		players[i]->current_bet = 0;
	}
	
	string input;
	string response;				 //response from user
	bool removalFinished = false;    //true if removing players step is complete
	bool additionFinished = false;   //true if adding players step is complete
	
	//vector<string> stringsReceived;  //stores the user's response word-by-word


	for (size_t i = zero; i < leavingBots.size(); i++) {

		shared_ptr<Player> currPlayer = Game::find_player(leavingBots[i]);

		if (currPlayer != nullptr) {

			for (vector<shared_ptr<Player>>::iterator it = players.begin(); it != players.end(); ++it) {
				string name = (*it)->playerName;
				
				string numOfWins = to_string((*it)->hands_won);
				string numOfLosses = to_string((*it)->hands_lost);
				string numOfChips = to_string((*it)->player_chips);

				if (name.compare(leavingBots[i]) == zero) {

						name.erase(name.size() - 1);
						ofstream newFile(name + ".txt", ofstream::out);
						if (newFile.is_open()) {
							newFile << name << " " << numOfWins << " " << numOfLosses << " " <<numOfChips ;
						}
						else {
							std::cout << "Unable to Open File For This Player: " << leavingBots[i] << endl;

						}
						newFile.close();
						players.erase(it);

						std::cout << "Bot has left the game: " << name << endl;		
						break;

				}
			}
		}
	}
		leavingBots.clear();



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
							if (name.back() == '*') {
								name.erase(name.size() - 1);
							}

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
	
	for (int i = 0; i < players.size(); i++) {
		if (players[i]->player_chips <= 0) {
			players[i]->player_chips = 20;
			cout << "Player: " << players[i]->playerName << "'s chips generously re-upped to 20!" << endl;
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


