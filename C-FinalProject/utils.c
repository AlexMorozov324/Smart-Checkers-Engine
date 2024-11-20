#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"


Player getRandomPlayer()
{
	// Seed the random number generator
	srand(time(NULL));

	// Generate a random number between 0 and 1
	int randomNum = rand() % 2;

	// Return 'T' if randomNum is 0, otherwise return 'B'
	return randomNum == 0 ? 'T' : 'B';
}

//Q1 functions
int isValidPosition(checkersPos pos)
{
	return (pos.row >= 0 && pos.row < BOARD_SIZE&& pos.col >= 0 && pos.col < BOARD_SIZE);
}

int hasPlayerPiece(Board board, Player player, checkersPos pos)
{
	if(player == 'T')
		return (isValidPosition(pos) && board[pos.row][pos.col] == 'B');
	else
		return (isValidPosition(pos) && board[pos.row][pos.col] == 'T');
	
}

int canMoveToPosition(Board board, checkersPos src, checkersPos dest)
{
	return (isValidPosition(src) && isValidPosition(dest) && board[dest.row][dest.col] != 'B' && board[dest.row][dest.col] != 'T');
}

SingleSourceMovesTreeNode* createNode(Board board, checkersPos* pos, unsigned short total_captures)//Creates a new node for the single source moves tree with the given parameters. The function allocates memory for the nodeand its members, initializes them, and copies the provided board state.
{
	SingleSourceMovesTreeNode* node = (SingleSourceMovesTreeNode*)malloc(sizeof(SingleSourceMovesTreeNode));
	if (node == NULL)
		exit(0);
	if (node != NULL)
	{
		int i;
		for (i = 0; i < 2; i++)
		{
			node->next_move[i] = NULL;
		}

		node->pos = malloc(sizeof(checkersPos));
		if (node->pos == NULL)
			exit(0);
		node->pos->row = pos->row;
		node->pos->col = pos->col;
		node->total_captures_so_far = total_captures;

		// Copy the board
		memcpy(node->board, board, sizeof(Board));
	}

	return node;
}

SingleSourceMovesTreeNode* FindSingleSourceMovesNode(Board board, checkersPos* src, int captures)
{
	SingleSourceMovesTreeNode* treeNode = (SingleSourceMovesTreeNode*)malloc(sizeof(SingleSourceMovesTreeNode));
	if (treeNode == NULL)
		return NULL;

	// Check if source position is valid and contains a player's piece
	if (!isValidPosition(*src) || board[src->row][src->col] == ' ')
	{
		free(treeNode);
		return NULL;
	}

	// Create the root node
	treeNode = createNode(board, src, captures);

	// Checkers player
	Player player = board[src->row][src->col];

	// Determine the directions based on the player
	int forward, capture_direction;
	if (player == 'B')
	{
		forward = -1;
		capture_direction = -1;
	}
	else if (player == 'T')
	{
		forward = 1;
		capture_direction = 1;
	}
	else
	{
		free(treeNode);
		return NULL;
	}

	// Check for possible captures
	checkersPos dest;
	checkersPos destIfCap;

	// Left capture
	dest.row = src->row + forward;
	dest.col = src->col - 1;
	destIfCap.row = dest.row + forward;
	destIfCap.col = dest.col - 1;
	if (hasPlayerPiece(board, player, dest) && canMoveToPosition(board, dest, destIfCap) && board[dest.row][dest.col] != player)
	{
		captures++;
		board[dest.row][dest.col] = ' ';
		board[src->row + 2 * forward][src->col - 2] = player;
		treeNode->next_move[0] = FindSingleSourceMovesNode(board, &destIfCap, captures);
		if (player == 'T')
			board[dest.row][dest.col] = 'B';
		else
			board[dest.row][dest.col] = 'T';
		board[src->row][src->col] = player;
		board[src->row + 2 * forward][src->col - 2] = ' ';
	}

	// Right capture
	dest.row = src->row + forward;
	dest.col = src->col + 1;
	destIfCap.row = dest.row + forward;
	destIfCap.col = dest.col + 1;
	if (hasPlayerPiece(board, player, dest) && canMoveToPosition(board, dest, destIfCap) && board[dest.row][dest.col] != player)
	{
		captures++;
		board[dest.row][dest.col] = ' ';
		board[src->row + 2 * forward][src->col + 2] = player;
		treeNode->next_move[1] = FindSingleSourceMovesNode(board, &destIfCap, captures);
		if (player == 'T')
			board[dest.row][dest.col] = 'B';
		else
			board[dest.row][dest.col] = 'T';
		board[src->row][src->col] = player;
		board[src->row + 2 * forward][src->col + 2] = ' ';
	}

	// No captures available, check for regular moves
	if (captures == 0)
	{
		// Forward left move
		dest.row = src->row + forward;
		dest.col = src->col - 1;
		if (canMoveToPosition(board, *src, dest))
		{
			checkersPos* nextMove = malloc(sizeof(checkersPos));
			if (nextMove == NULL)
				exit(0);
			nextMove->row = dest.row;
			nextMove->col = dest.col;
			treeNode->next_move[0] = createNode(board, nextMove, captures);
		}
		else
			treeNode->next_move[0] = NULL;

		// Forward right move
		dest.row = src->row + forward;
		dest.col = src->col + 1;
		if (canMoveToPosition(board, *src, dest))
		{
			checkersPos* nextMove = malloc(sizeof(checkersPos));
			if (nextMove == NULL)
				exit(0);
			nextMove->row = dest.row;
			nextMove->col = dest.col;
			treeNode->next_move[1] = createNode(board, nextMove, captures);
		}
		else
			treeNode->next_move[1] = NULL;
	}
	return treeNode;
}

SingleSourceMovesTree* FindSingleSourceMoves(Board board, checkersPos* src)
{
	SingleSourceMovesTree* tr = malloc(sizeof(SingleSourceMovesTree));
	if (tr == NULL)
		exit(0);
	int captures = 0;
	tr->source = FindSingleSourceMovesNode(board, src, captures);
	return tr;
}

//Q2 functions
SingleSourceMoveList* FindSingleSourceOptimalMove(SingleSourceMovesTree* moves_tree)//Finds the optimal move for a single source in the given moves tree. The function determines the maximum number of capturesand the number of moves that result in the maximum captures.It then constructsand returns a single source move list representing the optimal moves.
{
	int maxCaptures = 0;
	int numOfmaxCaptures = 0;
	Player player = moves_tree->source->board[moves_tree->source->pos->row][moves_tree->source->pos->col];
	SingleSourceMoveList* lst = malloc(sizeof(SingleSourceMoveList));
	if (lst == NULL)
		exit(0);
	getMaxCaptures(moves_tree->source, &maxCaptures, &numOfmaxCaptures);
	if (maxCaptures == 0 || numOfmaxCaptures >= 1)
	{
		
		makeEmptyList(lst);
		if (player == 'T')
		{
			int dummy = 0;
			buildListForT(moves_tree->source, lst, maxCaptures, &dummy, moves_tree->source->pos->row, moves_tree->source->pos->col);
		}
		else
		{
			int dummy = 0;
			buildListForB(moves_tree->source, lst, maxCaptures, &dummy);
		}
		
	}
	else
	{
		makeEmptyList(lst);
		if (player == 'T')
		{
			int dummy = 0;
			buildListForT(moves_tree->source, lst, maxCaptures, &dummy, moves_tree->source->pos->row, moves_tree->source->pos->col);
		}
		else
		{
			int dummy = 0;
			buildListForB(moves_tree->source, lst, maxCaptures, &dummy, moves_tree->source->pos->row, moves_tree->source->pos->col);
		}

	}

	return lst;
}

int maX(int a, int b)
{
	return a > b ? a : b;
}

void insertNode(SingleSourceMoveList* list, SingleSourceMovesTreeNode* treeNode)
{
	SingleSourceMoveListCell* newNode = (SingleSourceMoveListCell*)malloc(sizeof(SingleSourceMoveListCell));
	if (newNode == NULL)
		exit(0);
	newNode->captures = treeNode->total_captures_so_far;
	newNode->pos = malloc(sizeof(checkersPos));
	if (newNode->pos == NULL)
		exit(0);
	newNode->pos->row = treeNode->pos->row;
	newNode->pos->col = treeNode->pos->col;
	newNode->next = NULL;

	if (list->head == NULL) 
	{
		list->head = newNode;
		list->tail = newNode;
	}
	else 
	{
		list->tail->next = newNode;
		list->tail = newNode;
	}
}

void freeList(SingleSourceMoveList* list)
{
	SingleSourceMoveListCell* current = list->head;
	while (current != NULL) 
	{
		SingleSourceMoveListCell* temp = current;
		current = current->next;
		free(temp);
	}
}

void getMaxCaptures(SingleSourceMovesTreeNode* node, int* maxCaptures, int* countForMaxCap)
{
	if (node == NULL)
		return;
	else if (node->next_move[0] == NULL && node->next_move[1] == NULL)//if the node is a "leaf" we will check if the current number of captures is bigger then the max number
	{
		if (node->total_captures_so_far >= *maxCaptures)
		{
			*maxCaptures = node->total_captures_so_far;
			*countForMaxCap++;
		}

	}
	else
	{
		getMaxCaptures(node->next_move[0], maxCaptures, countForMaxCap);
		getMaxCaptures(node->next_move[1], maxCaptures, countForMaxCap);
	}
}

void buildListForT(SingleSourceMovesTreeNode* node, SingleSourceMoveList* lst, int maxCaptures, int* dummy)
{
	if (node == NULL)
		return;
	else if (node->total_captures_so_far == maxCaptures && node->next_move[0] == NULL && node->next_move[1] == NULL)//checking if we got to the "leaf" at the end of the best wat to play
	{
		SingleSourceMoveListCell* lNode = createNewListNode(node, NULL);// setting up the list node
		insertNodeToStartList(lst, lNode);

		*dummy = 1;//dummy varible that tells us if we got to the right "leaf" (if it equals 1 => yes)
	}
	else
	{
		buildListForT(node->next_move[1], lst, maxCaptures, dummy);

		if (*dummy == 1)
		{
			SingleSourceMoveListCell* lNode = createNewListNode(node, NULL);
			insertNodeToStartList(lst, lNode);
			return;
		}

		buildListForT(node->next_move[0], lst, maxCaptures, dummy);

		if (*dummy == 1)
		{
			SingleSourceMoveListCell* lNode = createNewListNode(node, NULL);
			insertNodeToStartList(lst, lNode);
			return;
		}
		

		// Free memory for nodes that didn't lead to the optimal move
		free(node->next_move[0]);
		node->next_move[0] = NULL;
		free(node->next_move[1]);
		node->next_move[1] = NULL;
	}
}

void buildListForB(SingleSourceMovesTreeNode* node, SingleSourceMoveList* lst, int maxCaptures, int* dummy)
{
	if (node == NULL)
		return;
	else if (node->total_captures_so_far == maxCaptures && node->next_move[0] == NULL && node->next_move[1] == NULL)//checking if we got to the "leaf" at the end of the best wat to play
	{
		SingleSourceMoveListCell* lNode = createNewListNode(node, NULL);// setting up the list node
		insertNodeToStartList(lst, lNode);

		*dummy = 1;//dummy varible that tells us if we got to the right "leaf" (if it equals 1 => yes)
	}
	else
	{
		buildListForB(node->next_move[0], lst, maxCaptures, dummy);

		if (*dummy == 1)
		{
			SingleSourceMoveListCell* lNode = createNewListNode(node, NULL);
			insertNodeToStartList(lst, lNode);
			return;
		}

		buildListForB(node->next_move[1], lst, maxCaptures, dummy);

		if (*dummy == 1)
		{
			SingleSourceMoveListCell* lNode = createNewListNode(node, NULL);
			insertNodeToStartList(lst, lNode);
			return;
		}

		// Free memory for nodes that didn't lead to the optimal move
		free(node->next_move[0]);
		node->next_move[0] = NULL;
		free(node->next_move[1]);
		node->next_move[1] = NULL;
	}
}

void makeEmptyList(SingleSourceMoveList* lst)
{
	lst->head = NULL;
	lst->tail = NULL;
}

SingleSourceMoveListCell* createNewListNode(SingleSourceMovesTreeNode* node, SingleSourceMoveListCell* next)
{
	SingleSourceMoveListCell* res;
	res = (SingleSourceMoveListCell*)malloc(sizeof(SingleSourceMoveListCell));
	if (res == NULL)
		return NULL;
	res->pos = node->pos;
	res->captures = node->total_captures_so_far;
	res->next = next;

	return res;
}

void insertNodeToStartList(SingleSourceMoveList* lst, SingleSourceMoveListCell* head)
{
	if (isEmptyList1(lst) == true)
	{
		head->next = NULL;
		lst->head = lst->tail = head;
	}
	else
	{
		head->next = lst->head;
		lst->head = head;
	}
}

bool isEmptyList1(SingleSourceMoveList* lst)
{
	if (lst->head == NULL)
		return true;
	else
		return false;
}

//Q3 functions
MultipleSourceMoveList* FindAllPossiblePlayerMoves(Board board, Player player)
{
	/*Finds all possible moves for a given player on the provided board.
	The function iterates over the board and checks each position for the player's pieces.
	For each piece found, it creates a single source moves tree and finds the optimal move using the FindSingleSourceOptimalMove function.
	The resulting single source move list is then inserted into the multiple source move list.
	The function returns the multiple source move list containing all possible moves.*/
	int i, j;
	MultipleSourceMoveList* lst = NULL;
	MultipleSourceMoveListCell* tail = NULL;
	SingleSourceMovesTree* movesTree;
	SingleSourceMoveList* sList;

	lst = malloc(sizeof(MultipleSourceMoveList));//allocating memory for the list
	if (lst == NULL)
		return NULL;

	tail = malloc(sizeof(MultipleSourceMoveListCell));//allocating memory for the list tail
	if (tail == NULL)
	{
		free(lst);
		return NULL;
	}
	lst->head = NULL;
	lst->tail = tail;

	for (i = 0; i <= 7; i++)
		for (j = 0; j <= 7; j++)
		{
			if (board[i][j] == player)
			{
				checkersPos* src = malloc(sizeof(checkersPos));
				if (src == NULL)
					exit(0);
				src->row = i;
				src->col = j;
				movesTree = FindSingleSourceMoves(board, src);
				if (movesTree->source->next_move[0] != NULL || movesTree->source->next_move[1] != NULL)
				{
					sList = FindSingleSourceOptimalMove(movesTree);
					insertNodeToEndList(lst, sList);
				}
				free(src);
			}
		}
	return lst;
}

void insertNodeToEndList(MultipleSourceMoveList* lst, SingleSourceMoveList* sList)
{
	MultipleSourceMoveListCell* cell = malloc(sizeof(MultipleSourceMoveListCell));
	if (cell == NULL)
		return;

	cell->single_source_moves_list = sList;
	cell->next = NULL;

	if (isEmptyList2(lst))
		lst->head = cell;
	else
		lst->tail->next = cell;
	lst->tail = cell;
}

bool isEmptyList2(MultipleSourceMoveList* lst)
{
	return lst->head == NULL;
}

//Q4 functions
void Turn(Board board, Player player) 
{
	MultipleSourceMoveList* lst;
	SingleSourceMoveList* chosenPath;
	SingleSourceMoveListCell* node;

	// Find all possible moves for the player on the given board
	lst = FindAllPossiblePlayerMoves(board, player);

	// Sort the list of moves in ascending order accroding to the captures made and location on the board of the player piece
	sortMultipleSourceMoveList(lst,player);
	// Get the chosen path (sequence of moves) with the highest number of captures or the best location according to the roles
	chosenPath = lst->head->single_source_moves_list;

	// Convert row indices to corresponding characters for printing
	char Srow = 'A' + chosenPath->head->pos->row;
	char Erow = 'A' + chosenPath->tail->pos->row;

	// Print the chosen path and turn information
	printf("\n%c's turn:\n", player);
	printf("%c%d->%c%d\n", Srow, chosenPath->head->pos->col + 1, Erow, chosenPath->tail->pos->col + 1);
	
	// update the moves and captures on the board
	if (chosenPath->tail->captures > 0) 
	{
		node = chosenPath->head;
		while (node->next != NULL) 
		{
			if (player == 'B')
			{
				if (node->pos->row - 1 > node->next->pos->row)
				{
					if(node->pos->col > node->next->pos->col)
						board[node->pos->row - 1][node->pos->col - 1] = ' ';
					else
						board[node->pos->row - 1][node->pos->col + 1] = ' ';
				}
					board[node->pos->row][node->pos->col] = ' '; 
				node = node->next;
			}
			else
			{
				if (node->pos->row + 1 < node->next->pos->row)
				{
					if (node->pos->col > node->next->pos->col)
						board[node->pos->row + 1][node->pos->col - 1] = ' ';
					else
						board[node->pos->row + 1][node->pos->col + 1] = ' ';
				}
				board[node->pos->row][node->pos->col] = ' '; 
				node = node->next;
			}
			
		}
	}
	else if (chosenPath->head->captures == 0)
	{
		board[chosenPath->head->pos->row][chosenPath->head->pos->col] = ' ';
	}
	// Place the player's piece on the final position of the chosen path
	board[chosenPath->tail->pos->row][chosenPath->tail->pos->col] = player;
}

void sortMultipleSourceMoveList(MultipleSourceMoveList* list, Player player) //  Sorts the multiple source move list in descending order based on captures made. If captures are the same, the sorting is based on the position of the head node.
{
	int swapped;
	MultipleSourceMoveListCell* ptr1;
	MultipleSourceMoveListCell* lptr = NULL;

	if (list->head == NULL)
		return;

	// Perform bubble sort on the list...
	do {
		swapped = 0;
		ptr1 = list->head;

		while (ptr1->next != lptr) 
		{
			if (ptr1->single_source_moves_list->tail->captures < ptr1->next->single_source_moves_list->tail->captures) 
			{
				swap(ptr1, ptr1->next);
				swapped = 1;
			}
			else if (ptr1->single_source_moves_list->tail->captures == ptr1->next->single_source_moves_list->tail->captures)
			{
				if (player == 'B')
				{
					if (ptr1->single_source_moves_list->head->pos->row > ptr1->next->single_source_moves_list->head->pos->row)
					{
						swap(ptr1, ptr1->next);
						swapped = 1;
					}
					else if (ptr1->single_source_moves_list->head->pos->row == ptr1->next->single_source_moves_list->head->pos->row) 
					{
						if (ptr1->single_source_moves_list->head->pos->col > ptr1->next->single_source_moves_list->head->pos->col)
						{
							swap(ptr1, ptr1->next);
							swapped = 1;
						}
					}
				}
				else if (player == 'T') 
				{
					if (ptr1->single_source_moves_list->head->pos->row < ptr1->next->single_source_moves_list->head->pos->row) 
					{
						swap(ptr1, ptr1->next);
						swapped = 1;
					}
					else if (ptr1->single_source_moves_list->head->pos->row == ptr1->next->single_source_moves_list->head->pos->row) 
					{
						if (ptr1->single_source_moves_list->head->pos->col < ptr1->next->single_source_moves_list->head->pos->col) 
						{
							swap(ptr1, ptr1->next);
							swapped = 1;
						}
					}
				}
			}
			ptr1 = ptr1->next;
		}
		lptr = ptr1;
	} while (swapped);
}

void swap(MultipleSourceMoveListCell* a, MultipleSourceMoveListCell* b) 
{
	SingleSourceMoveList* temp = a->single_source_moves_list;
	a->single_source_moves_list = b->single_source_moves_list;
	b->single_source_moves_list = temp;
}

//Q5 functions
void printBoard(Board board,int* T, int* cT, int* B, int* cB)// Prints the current state of the game board. Also updates the countand remaining counts of 'T' and 'B' pieces for the statistics of the game.
{
	int i, j;
	int countT = 0;
	int countB = 0;
	char row = 'A';
	printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	printf("+ | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |");
	for (i = 0; i < 8; i++)
	{
		printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n"); 
		printf("|%c|", row);
		row++;
		for (j = 0; j < 8; j++)
		{
			if (board[i][j] == 'T')
			{
				countT++;
				printf(" T |");
			}
			else if (board[i][j] == 'B')
			{
				countB++;
				printf(" B |");
			}
			else
			{
				printf("   |");
			}
		}
	}
	printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");

	if (*B > countB)
	{
		*cT = *B - countB;
		*B = countB;
	}
	if (*T > countT)
	{
		*cB = *T - countT;
		*T = countT;
	}
}

Player getNextPlayer(Player player)//getting the next player
{
	if (player == 'T')
		return 'B';
	else
		return 'T';
}

bool isGameOver(Board board)//Checks if the game is over by examining the current state of the game board.
{
	int i,j;
	int count1 = 0;
	int count2 = 0;
	bool isGameOver = false;
	for (i = 0; i < 8; i++)//Checks if the game has ended according to the second or third condition
	{
		if (board[7][i] == 'T')
			isGameOver = true;
		if (board[0][i] == 'B')
			isGameOver = true;
	}

	for(i = 0 ; i < 8 ; i++)//Checks if the game has ended according to the first condition
		for (j = 0; j < 8; j++)
		{
			if (board[i][j] == 'T')
				count1++;
			else if(board[i][j] == 'B')
				count2++;
		}
	if(count1 == 0 || count2 == 0)
		isGameOver = true;

	return isGameOver;

}

Player getWinner(Board board)//getting the winner by examining the current state of the game board.
{
	Player winner = NULL;
	int i, j;
	int count1 = 0;
	int count2 = 0;
	for (i = 0; i < 8; i++)//Checks there is a winner according to the second or third condition and setting it
	{
		if (board[7][i] == 'T')
			winner = 'T';
		if (board[0][i] == 'B')
			winner = 'B';
	}
	if (winner == NULL)
	{
		for (i = 0; i < 8; i++)//Checks there is a winner according to the first condition and setting it
			for (j = 0; j < 8; j++)
			{
				if (board[i][j] == 'T')
					count1++;
				else if (board[i][j] == 'B')
					count2++;
			}
		if (count1 == 0)
			winner = 'B';
		else
			winner = 'T';
	}
	
	return winner;

}

void playGame(Board board, Player starting_player) 
{
   /* the function simulates and plays a game of checkers on the given game board starting with the specified player.
	  The function alternates between playersand executes their turns until the game is over.
	  After the game is finished, it prints the winner, the number of moves made by the winner,
	  and the player who performed the highest number of captures in a single move. */
	
	Player maxCpturesPlayer;
	int* T = malloc(sizeof(int));
	int* cT = malloc(sizeof(int));
	int* B = malloc(sizeof(int));
	int* cB = malloc(sizeof(int));
	*cB = *cT = 0;
	*T = *B = 12;
	int maxCptures = 0;
	Player current_player = starting_player;
	printBoard(board, T, cT, B, cB);
	int numOfMovesForT = 0;
	int numOfMovesForB = 0; 

	while (!isGameOver(board))
	{
		Turn(board, current_player);
		printBoard(board, T, cT, B, cB);
		if (*cB != 0 || *cT != 0)
		{
			if (*cB > maxCptures && *cT == 0)
			{
				maxCptures = *cB;
				maxCpturesPlayer = 'B';
			}
			else if (*cT > maxCptures && *cB == 0)
			{
				maxCpturesPlayer = 'T';
				maxCptures = *cT;
			}
		}
		*cB = *cT = 0;

		if (current_player == 'T')
			numOfMovesForT++;
		else
			numOfMovesForB++;

		// Switch to the next player
		current_player = getNextPlayer(current_player);
	}

	  // Game over, print the winner and statistics
	Player winner = getWinner(board);
	printf("\n\n%c wins!\n", winner);
	if (numOfMovesForT > numOfMovesForB)
	{
		printf("%c performed %d moves.\n", winner, numOfMovesForT);
	}
	else
	{
		printf("%c performed %d moves.\n", winner, numOfMovesForB);
	}
	printf("%c performed the highest number of captures in a single move - %d\n", maxCpturesPlayer, maxCptures);
	
}

