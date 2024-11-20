#include <stdbool.h>
#ifndef _UTILS_H
#define _UTILS_H
#define _CRT_SECURE_NO_WARNINGS 
#define BOARD_SIZE 8

// all of the typedef's...
typedef struct checkersPos
{
	char row, col;
}checkersPos;

typedef unsigned char Board[BOARD_SIZE][BOARD_SIZE];

typedef unsigned char Player;

typedef struct SingleSourceMovesTreeNode
{
	Board board;
	checkersPos* pos;
	unsigned short total_captures_so_far;
	struct SingleSourceMovesTreeNode* next_move[2];
}SingleSourceMovesTreeNode;

typedef struct SingleSourceMovesTree
{
	SingleSourceMovesTreeNode* source;
}SingleSourceMovesTree;

typedef struct SingleSourceMoveListCell
{
	checkersPos* pos;
	unsigned short captures;
	struct SingleSourceMoveListCell* next;
}SingleSourceMoveListCell;

typedef struct SingleSourceMoveList
{
	struct SingleSourceMoveListCell* head;
	struct SingleSourceMoveListCell* tail;
}SingleSourceMoveList;

typedef struct multipleSourceMoveListCell
{
	SingleSourceMoveList* single_source_moves_list;
	struct multipleSourceMoveListCell* next;
}MultipleSourceMoveListCell;

typedef struct multipleSourceMoveList
{
	struct multipleSourceMoveListCell* head;
	struct multipleSourceMoveListCell* tail;
}MultipleSourceMoveList;


Player getRandomPlayer();

// Q1 functions
SingleSourceMovesTree* FindSingleSourceMoves(Board board, checkersPos* src);
int isValidPosition(checkersPos pos);
int hasPlayerPiece(Board board, Player player, checkersPos pos);
int canMoveToPosition(Board board, checkersPos src, checkersPos dest);
SingleSourceMovesTreeNode* createNode(Board board, checkersPos* pos, unsigned short total_captures);
SingleSourceMovesTreeNode* FindSingleSourceMovesNode(Board board, checkersPos* src, int captures);

// Q2 functions
SingleSourceMoveList* FindSingleSourceOptimalMove(SingleSourceMovesTree* moves_tree);
int maX(int a, int b);
void insertNode(SingleSourceMoveList* list, SingleSourceMovesTreeNode* treeNode);
void freeList(SingleSourceMoveList* list);
void getMaxCaptures(SingleSourceMovesTreeNode* node, int* maxCaptures, int* countForMaxCap);
void buildListForT(SingleSourceMovesTreeNode* node, SingleSourceMoveList* lst, int maxCaptures, int* dummy);
void buildListForB(SingleSourceMovesTreeNode* node, SingleSourceMoveList* lst, int maxCaptures, int* dummy);
void makeEmptyList(SingleSourceMoveList* lst);
SingleSourceMoveListCell* createNewListNode(SingleSourceMovesTreeNode* node, SingleSourceMoveListCell* next);
void insertNodeToStartList(SingleSourceMoveList* lst, SingleSourceMoveListCell* head);
bool isEmptyList1(SingleSourceMoveList* lst);

//Q3 functions
MultipleSourceMoveList* FindAllPossiblePlayerMoves(Board board, Player player);
void insertNodeToEndList(MultipleSourceMoveList* lst, SingleSourceMoveList* sList);
bool isEmptyList2(MultipleSourceMoveList* lst);

//Q4 functions
void Turn(Board board, Player player);
void sortMultipleSourceMoveList(MultipleSourceMoveList* list, Player player);
void swap(MultipleSourceMoveListCell* a, MultipleSourceMoveListCell* b);

//Q5 functions
void printBoard(Board board, int* T, int* cT, int* B, int* cB);
Player getNextPlayer(Player player);
bool isGameOver(Board board);
void playGame(Board board, Player starting_player);
Player getWinner(Board board);
#endif
