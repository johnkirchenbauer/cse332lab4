
#pragma once

#include "stdafx.h"
#include <stdio.h>
#include "TexasHoldEm.h"
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




TexasHoldEm::TexasHoldEm() : dealerPosition(0), pot(0) {
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
	//create the 52 cards and add them to the deck
	for (int s = zero; s < four; s++) {
		for (int r = zero; r < thirteen; r++) {
			Card c = { suitArrSCS[s],rankArrSCS[r] };
			mainDeck.add_card(c);
		}
	}
}

int TexasHoldEm::before_turn(Player & player) {
	return Success;
}

/*This method deals cards to players who have less than 5 cards.*/
int TexasHoldEm::turn(Player & p) {
	return Success;
}

/*This method prints out each player's hand after the round*/
int TexasHoldEm::after_turn(Player & p) {
	std::cout << p.playerName << ": " << p.player_cards << endl;
	return zero;
}

/*Calls the main deck member variable's shuffle method and then deals one card at a time from the main
**deck to each player, continuing to iterate through the players until each player has received five cards.
**/
int TexasHoldEm::before_round() {

	std::cout << endl << "Texas Hold 'Em Round starting!" << endl;

	size_t numPlayers = players.size();

	//check if there are a valid number of players
	if (numPlayers < two) {
		throw TooFewPlayers;
	}
	else if (numPlayers > twentythree) {
		throw TooFewCards;
	}

	std::cout << players[dealerPosition]->playerName << " is dealing!" << endl;

	mainDeck.shuffleDeck();

	size_t dealIndex;

	//deal each player 5 cards each from the deck
	for (size_t numCards = zero; numCards < two; numCards++) {
		for (size_t i = one; i <= players.size(); ++i) {
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

	}
	std::cout << endl;

	return Success;
}

int TexasHoldEm::betting_zero() {

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

int TexasHoldEm::betting_one() {

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

				if (needAllChips) {
					cout << players[i]->playerName << " need's to put in all their chips to call. Would they like to call? (yes or no)" << endl;
					get_response() ? playerType = AllIn : playerType = Fold;
					needAllChips = false;
				}
			}
			else {
				cout << "No bet has been made." << endl;
				(playerChips > 0 ? playerType = Check : playerType = AllIn);
			}

			//cout << currPlayer->playerName << " has " << currPlayer->player_chips << " chips." << endl;

			cout << players[i]->playerName << "'s Cards: " << players[i]->player_cards << endl;

			if (public_cards.size() == 0) {
				cout << "Public Cards: none" << endl;
			}
			else {
				cout << "Public Cards: " << public_cards << endl;
			}

			for (size_t j = 0; j < players.size(); ++j) {
				if (i != j && (players[j]->still_betting || players[j]->all_in)) {

					cout << players[j]->playerName << "'s Cards: ";
					for (size_t k = 0; k < players[j]->player_cards.size(); ++k) {
						cout << "* ";
					}
					cout << endl;
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
int TexasHoldEm::round() {

	size_t turnIndex;
	size_t numPlayers = players.size();

	//add the first 3 public cards
	while (public_cards.size() < three) {
		if (mainDeck.size() <= zero) {
			throw TooFewCards;
		}
		else {
			public_cards << mainDeck;

		}
	}

	betting_one();
	for (int i = 0; i < players.size(); i++) {
		players[i]->current_bet = 0;
	}

	//add the 4th public card
	if (mainDeck.size() <= zero) {
		throw TooFewCards;
	}
	else {
		public_cards << mainDeck;

	}

	betting_one();
	for (int i = 0; i < players.size(); i++) {
		players[i]->current_bet = 0;
	}

	//add the 5th public card
	if (mainDeck.size() <= zero) {
		throw TooFewCards;
	}
	else {
		public_cards << mainDeck;
	}

	betting_one();
	for (int i = 0; i < players.size(); i++) {
		players[i]->current_bet = 0;
	}

	return Success;
}


/*get every possible combination of indices (to choose 5 cards from 7), e.g. [0,1,2,3,4], [0,1,2,3,5], etc */
set<vector<size_t>> get_combinations() {

	set<vector<size_t>> s;  //the set that will be returned

	vector<size_t> indices;  //the indices of the 5 selected cards

							 //add the indices to the set, and clear the indices each time through
	for (size_t i = 0; i < 3; ++i) {
		for (size_t j = i + 1; j < 4; ++j) {
			for (size_t k = j + 1; k < 5; ++k) {
				for (size_t m = k + 1; m < 6; ++m) {
					for (size_t n = m + 1; n < 7; ++n) {
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
Hand getBestHand(Hand & h) {


	int best_hand_score = -1;   //score of the best hand

	Hand bestHand;   //the hand that eventually gets returned.
	Hand temp;       //temporary variable to store intermediate hands

	set<vector<size_t>> combos = get_combinations();  //get all the combinations (7 choose 5)

														  //add the cards at the appropriate indices to temp, compare temp to the current best hand, and replace if necessary
	for (set<vector<size_t>>::iterator it = combos.begin(); it != combos.end(); ++it) {
		for (size_t i = 0; i < (*it).size(); ++i) {
			size_t index = (*it)[i];
			//cout << "Index: " << index << endl;
			//cout << "Player cards: " << h.getCards() << endl;
			temp.getCards().push_back(h.getCards()[index]);
		}

		int tempRank = getHandRank(temp.getCards());  //rank of the temp hand

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

		temp.clear_hand();  //clear the temp hand
	}

	return bestHand;

}

/*Increments wins and losses, prints out the result of the round, moves all players' cards back to the deck, asks if any player
**wants to leave the game, and asks if any player wants to join the game.
*/
int TexasHoldEm::after_round() {

	//make copy so we don't mess with deal order
	vector<shared_ptr<Player>> copyOfPlayers = players;




	//choose the best hand for each player
	for (size_t i = 0; i < copyOfPlayers.size(); ++i) {
		for (size_t j = 0; j < public_cards.size(); ++j) {
			copyOfPlayers[i]->player_cards.add_card(public_cards[j]);
		}

		if (copyOfPlayers[i]->player_cards.size() == 7) {
			Hand bestHand = getBestHand(copyOfPlayers[i]->player_cards);
			copyOfPlayers[i]->player_cards.clear_hand();
			copyOfPlayers[i]->player_cards = bestHand;
		}



	}

	//sort the hands based on their poker rank
	sort(copyOfPlayers.begin(), copyOfPlayers.end(), [](shared_ptr<Player> p1, shared_ptr<Player> p2) {
		bool p1Fold = !p1->all_in && !p1->still_betting;
		bool p2Fold = !p2->all_in && !p2->still_betting;

		bool ans = false;

		if (!p1Fold && !p2Fold) {
			ans = poker_rank(p1->player_cards, p2->player_cards);
		}
		else {

			if (p2Fold) {
				ans = true;
			}

			if (p1Fold) {
				ans = false;
			}
		}

		return ans;

	});
	std::cout << endl;
	vector<string> stringsReceived;  //names of players that will be removed


									 //increment score for winner, decrement score for losers
	for (size_t i = 0; i < copyOfPlayers.size(); i++) {
		if (copyOfPlayers[i]->all_in || copyOfPlayers[i]->still_betting) {
			if (!poker_rank(copyOfPlayers[0]->player_cards, copyOfPlayers[i]->player_cards)) {
				//block not still betting
				//if (copyOfPlayers[i]->still_betting == true || players[i]->all_in == true) {
				std::cout << "Winner: " << copyOfPlayers[i]->playerName << endl;
				copyOfPlayers[i]->hands_won++;
				copyOfPlayers[i]->player_chips += pot;
				cout << "Pot size: " << pot << endl;
				pot = 0;
				//}
			}

			else {
				copyOfPlayers[i]->hands_lost++;
			}
		}
		else {
			copyOfPlayers[i]->hands_lost++;
		}

	}
	std::cout << endl;

	//print out the player hands
	for (size_t i = zero; i < copyOfPlayers.size(); i++) {

		std::cout << *copyOfPlayers[i] << endl;
		std::cout << copyOfPlayers[i]->player_cards;
		if ((!copyOfPlayers[i]->still_betting) && !(copyOfPlayers[i]->all_in)) {
			cout << " (FOLDED)";
		}

		cout << endl;



	}
	std::cout << endl;

	//transfer the cards from the player hands back to the deck
	for (size_t i = zero; i < copyOfPlayers.size(); i++) {
		for (int j = copyOfPlayers[i]->player_cards.size() - 1; j >= zero; j--) {

			size_t k = j;

			//move cards back into the main deck
			mainDeck.add_card(copyOfPlayers[i]->player_cards[k]);

			//delete cards from player hands
			copyOfPlayers[i]->player_cards.remove_card(k);

		}
		

	}

	for (size_t i = zero; i < public_cards.size(); ++i) {
		mainDeck.add_card(public_cards[i]);
	}

	public_cards.clear_hand();

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




