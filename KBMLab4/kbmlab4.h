/*
* Name: kbmlab4.h
* Authors: Alex Bakus, John Kirchenbauer, Zach Miller
* Description: This file contains the main function of the program
**/
#ifndef kbmlab4_H
#define kbmlab4_H

enum ErrorCode {
	Failure = -1,
	Success = 0,
	BadInput = 1,
	FileNotOpen = 2,
	TooFewPlayers = 3,
	TooFewCards = 4,
	InstanceNotAvailable = 5,
	GameAlreadyStarted = 6,
	NoGameInProgress = 7,
	PlayerAlreadyPlaying = 8,
	UnknownGame = 9,
	AllPlayersLeft = 10,
	InvalidName = 11
};

int program_Usage(ErrorCode code);

#endif