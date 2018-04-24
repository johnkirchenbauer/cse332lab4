
#pragma once

#include "stdafx.h"
#include <stdio.h>
#include "SevenCardStud.h"
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
Card::Suit suitArrSCS[four] = {
	Card::clubs,
	Card::diamonds,
	Card::hearts,
	Card::spades
};

Card::Rank rankArrSCS[thirteen] = {
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

SevenCardStud::SevenCardStud() : dealerPosition(0), pot(0) {

	//create the 52 cards and add them to the deck
	for (int s = zero; s < four; s++) {
		for (int r = zero; r < thirteen; r++) {
			Card c = { suitArrSCS[s],rankArrSCS[r] };
			mainDeck.add_card(c);
		}
	}


}

/*This method receives input from the user telling it which card(s) to discard, and removes those cards from the player's hand*/
int SevenCardStud::before_turn(Player & player) {
	return Success;
}

/*This method deals cards to players who have less than 5 cards.*/
int SevenCardStud::turn(Player & p) {

	if (p.player_cards.size() == 6) {
		//if neither deck has enough cards, throw an exception, otherwise move the card from the deck to the player's hand
		if (mainDeck.size() <= zero) {

			if (discardDeck.size() <= zero) {
				throw TooFewCards;

			}
			else {
				discardDeck.shuffleDeck();
				add_card(p.facedown_cards, p.player_cards, discardDeck);
			}

		}
		else {
			add_card(p.facedown_cards, p.player_cards, mainDeck);
		}
	}
	else {
		if (mainDeck.size() <= zero) {

			if (discardDeck.size() <= zero) {
				throw TooFewCards;

			}
			else {
				discardDeck.shuffleDeck();
				add_card(p.faceup_cards, p.player_cards, discardDeck);
			}

		}
		else {
			add_card(p.faceup_cards, p.player_cards, mainDeck);
		}
	}
	return Success;
}

/*This method prints out each player's hand after the round*/
int SevenCardStud::after_turn(Player & p) {
	std::cout << p.playerName << ": " << p.player_cards << endl;
	return zero;
}

/*Calls the main deck member variable's shuffle method and then deals one card at a time from the main
**deck to each player, continuing to iterate through the players until each player has received five cards.
**/
int SevenCardStud::before_round() {

	std::cout << endl << "SCS Round starting!" << endl;

	size_t numPlayers = players.size();

	//check if there are a valid number of players
	if (numPlayers < two) {
		throw TooFewPlayers;
	}
	else if (numPlayers > ten) {
		throw TooFewCards;
	}

	std::cout << players[dealerPosition]->playerName << " is dealing!" << endl;

	mainDeck.shuffleDeck();

	size_t dealIndex;

	//deal each player 5 cards each from the deck
	for (size_t numCards = zero; numCards < three; numCards++) {
		for (size_t i = one; i <= numPlayers; ++i) {
			dealIndex = (dealerPosition + i) % numPlayers;
			if (numCards == 2) {
				add_card(players[dealIndex]->faceup_cards, players[dealIndex]->player_cards, mainDeck);
				//players[dealIndex]->faceup_cards << mainDeck;
			}
			else {
				//players[dealIndex]->facedown_cards << mainDeck;
				add_card(players[dealIndex]->facedown_cards, players[dealIndex]->player_cards, mainDeck);
			}
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

int SevenCardStud::betting_zero() {

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

//John's updaterooney --------------------------------------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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

int SevenCardStud::betting_one() {

	set<int> betterIndices;
	int callers = 0;
	for (int i = 0; i < players.size(); i++) {
		if (players[i]->still_betting && !players[i]->all_in) {
			betterIndices.insert(i);
		}
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
			cout << players[*it]->playerName << "(" << players[*it]->player_chips << ") ";
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

			//cout << currPlayer->playerName << " has " << currPlayer->player_chips << " chips." << endl;

			cout << players[i]->playerName << "'s Cards: " << players[i]->player_cards << endl;

			
			for (size_t j = 0; j < players.size(); ++j) {
				if (i != j && (players[j]->still_betting || players[j]->all_in)) {
					cout << players[j]->playerName << "'s Cards: ";

					for (size_t k = 0; k < players[j]->facedown_cards.size(); ++k) {
						cout << "* ";
					}

					cout << players[j]->faceup_cards << endl;
				}
			}


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
						cout << "It will cost " << currPlayer->playerName << " " << (maxBet - currPlayer->current_bet) << " chips to Call on the current bet." << endl;
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
							else { playerFinished = true; }
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
						if (resp == 2) {
							cout << "Starting bet is two chips!" << endl;
							maxBet += 2;
							currPlayer->player_chips -= (maxBet - currPlayer->current_bet);
							pot += (maxBet - currPlayer->current_bet);
							currPlayer->current_bet = maxBet;
							callers = 0;
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
							isBet = true;
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

			if (betterIndices.find(i) != betterIndices.end()) {
				callers++;
			}
		}

		i = (i + 1) % (players.size());
	}

	return Success;
}

//-------------------------------------------------------------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/*Iterates through the players and at each player calling their turn method and then their after_turn method*/
int SevenCardStud::round() {

	size_t turnIndex;
	size_t numPlayers = players.size();

	//call turn and after_turn methods for each player
	for (size_t betRound = 0; betRound < 4; ++betRound) {
		for (size_t i = one; i <= numPlayers; ++i) {
			turnIndex = (dealerPosition + i) % numPlayers;

			if (players[turnIndex]->still_betting || players[turnIndex]->all_in) {
				int turnResult = turn(*players[turnIndex]);

				if (turnResult != 0) {
					return Failure;
				}


			}

		}

		//reset current bet amount to 0
		for (int i = 0; i < players.size(); i++) {
			players[i]->current_bet = 0;
		}

		cout << endl;
		cout << endl;
		cout << "Next round of betting starting" << endl;
		betting_one();
	}

	//call after_turn on all players
	for (size_t i = zero; i < numPlayers; ++i) {
		int afterTurnResult = after_turn(*players[i]);

		if (afterTurnResult != 0) {
			return Failure;
		}
	}

	cout << "after_turn success" << endl;
	return Success;
}

/*get every possible combination of indices (to choose 5 cards from 7), e.g. [0,1,2,3,4], [0,1,2,3,5], etc */
set<vector<size_t>> get_all_combinations() {

	set<vector<size_t>> s;  //the set that will be returned

	vector<size_t> indices;  //the indices of the 5 selected cards

	//add the indices to the set, and clear the indices each time through
	for (size_t i = 0; i < 3; ++i) {
		for (size_t j = i+1; j < 4; ++j) {
			for (size_t k = j+1; k < 5; ++k) {
				for (size_t m = k+1; m < 6; ++m) {
					for (size_t n = m+1; n < 7; ++n) {
						indices.push_back(i);
						indices.push_back(j);
						indices.push_back(k);
						indices.push_back(m);
						indices.push_back(n);
						s.insert(indices);
						indices.clear();
					}
					
				}
			}
		}
	}

	return s;
	
}

/*Returns the best 5-card hand from the 7 cards in the hand*/
Hand get_best_hand(Hand & h) {
	if (h.size() != 7) {
		cout << "invalid hand length" << endl;
		vector<Card> dummy;
		Card c1 = { Card::spades, Card::two };
		Card c2 = { Card::spades, Card::three };
		Card c3 = { Card::spades, Card::four };
		Card c4 = { Card::spades, Card::five };
		Card c5 = { Card::hearts, Card::seven };
		dummy.push_back(c1);
		dummy.push_back(c2);
		dummy.push_back(c3);
		dummy.push_back(c4);
		dummy.push_back(c5);
		sort(dummy.begin(), dummy.end());

		return Hand(dummy);
	}

	int best_hand_score = -1;   //score of the best hand

	Hand bestHand;   //the hand that eventually gets returned.
	Hand temp;       //temporary variable to store intermediate hands

	set<vector<size_t>> combos = get_all_combinations();  //get all the combinations (7 choose 5)
	
	//add the cards at the appropriate indices to temp, compare temp to the current best hand, and replace if necessary
	for (set<vector<size_t>>::iterator it = combos.begin(); it != combos.end(); ++it) {
		for (size_t i = 0; i < (*it).size(); ++i) {
			size_t index = (*it)[i];
			cout << "Index: " << index << endl;
			cout << "Player cards: " << h.getCards() << endl;
			temp.getCards().push_back(h.getCards()[index]);
		}
		cout << "yay" << endl;

		int tempRank = getHandRank(temp.getCards());  //rank of the temp hand
		cout << "yay2" << endl;
		//if the temp hand is better, update bestHand
		if (tempRank > best_hand_score) {
			best_hand_score = tempRank;
			bestHand = temp;
		}

		//if both are equal, call poker rank to determine the better hand
		//if (tempRank == best_hand_score) {
		//	if (poker_rank(temp, bestHand)) {
		//		bestHand = temp;
		//	}
		//}
		cout << "yay3" << endl;

		temp.clear_hand();  //clear the temp hand
	}

	cout << "yay4" << endl;
	return bestHand;

}

/*Increments wins and losses, prints out the result of the round, moves all players' cards back to the deck, asks if any player
**wants to leave the game, and asks if any player wants to join the game.
*/
int SevenCardStud::after_round() {

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


	//choose the best hand for each player
	for (size_t i = 0; i < copyOfPlayers.size(); ++i) {
		Hand bestHand = get_best_hand(copyOfPlayers[i]->player_cards);
		copyOfPlayers[i]->player_cards.clear_hand();
		copyOfPlayers[i]->player_cards = bestHand;

	}

	cout << "yay5" << endl;


	//sort the hands based on their poker rank
	sort(copyOfPlayers.begin(), copyOfPlayers.end(), [](shared_ptr<Player> p1, shared_ptr<Player> p2) {
		return poker_rank(p1->player_cards, p2->player_cards);
	});
	std::cout << endl;
	cout << "sort poker hand succes" << endl;
	vector<string> stringsReceived;  //names of players that will be removed


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
		}

		else {
			copyOfPlayers[i]->hands_lost++;
		}

	}
	std::cout << endl;
	cout << "increment decrement winner loser success" << endl;

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
		for (int j = copyOfPlayers[i]->player_cards.size() -1 ; j >= zero; j--) {

			size_t k = j;

			//move cards back into the main deck
			mainDeck.add_card(copyOfPlayers[i]->player_cards[k]);

			//delete cards from player hands
			copyOfPlayers[i]->player_cards.remove_card(k);

		}
		copyOfPlayers[i]->facedown_cards.clear_hand();
		copyOfPlayers[i]->faceup_cards.clear_hand();

	}

	//move cards from the discard deck to the main deck
	for (size_t i = discardDeck.size(); i > zero; i--) {
		mainDeck.add_card(discardDeck.getCards()[i - one]);
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


	//check chip counts and reup or boot players
	for (int i = 0; i < players.size(); i++) {
		if (players[i]->player_chips <= 0) {
			cout << "Player: " << players[i]->playerName << " does not have enough chips to continue playing. Would they like to add more chips? (yes or no)" << endl;
			if (get_response()) {
				players[i]->player_chips = 20;
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




