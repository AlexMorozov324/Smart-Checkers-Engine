#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "utils.h"

int main()
{
	Player player;
	Board board;

	//setting up the starting board
	board[0][1]=board[0][3]=board[0][5]=board[0][7]=board[1][0]=board[1][2]=board[1][4]=board[1][6]=board[2][1]=board[2][3]=board[2][5]=board[2][7] = 'T';
	board[5][0]=board[5][2]=board[5][4]=board[5][6]=board[6][1]=board[6][3]=board[6][5]=board[6][7]=board[7][0]=board[7][2]=board[7][4]=board[7][6] = 'B';

	player = getRandomPlayer();
	
	playGame(board, player);
}

