#pragma once


#include<iostream>
#include<string>
#include<cstdint>
#include<raylib.h>
#include<vector>
#include"Network.h"
using namespace std;

/*Pièces
* 
* Rien = 0
* 
* Blancs :
*	Pion = 1
*	Cavalier = 2
*	Fou = 3
*	Tour = 4
*	Dame = 5
*	Roi = 6
* 
* 
* Noirs
*	Pion = 7
*	Cavalier = 8
*	Fou = 9
*	Tour = 10
*	Dame = 11
*	Roi = 12
* 
*/
const int maxMoves = 100;
class ChessBoard {
private:
	//Représentation du plateau
	uint_fast8_t chessBoard[8][8] = { {0, 0, 0, 5, 6, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{1, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 12, 0, 0,0}};

	//Coups légaux (2 positions par coups)
	uint_fast8_t legalMoves[maxMoves * 4];
	uint_fast8_t nbQueens = 2;
	uint_fast8_t nbRooks = 4;
	uint_fast8_t nbBishops = 4;
	uint_fast8_t nbKnights = 4;
	uint_fast8_t nbPawns = 16;

	
	
	int nbCoups = 0;

	Vector2 boardDisplayPosition {50,50};
	Vector2 tileSize = {50,50};
	Texture2D piecesTexture[12];


	//Game State
	bool gameEnd = false;
	bool whiteWin = false;
	bool win = false;
	bool draw = false;


	//En passant
	bool enPassantOk = false;
	uint_fast8_t enPassant = 8;


	//50 moves rule
	int fiftyMove = 0;

	//Droit de roque des joueurs (blancs petit, grand, noir petit grand).
	bool roques[4] = { true, true, true, true };
	
	
	//Trait du joueur, true : blanc, false : noir
	bool trait = true;


	//String to print moves
	string clm = "abcdefgh";
	string pieceStr = "NBRQK";


	//Boolean for display and gui
	bool isPieceSelected = false;
	uint_fast8_t pieceSelected[2] = {10,10};
	bool mouseOnBoard = false;
	uint_fast8_t mouseOnTile[2] = { 8,8 };
	uint_fast8_t selectedTile[2] = { 8,8 };
public:
	uint_fast8_t nbMoves;
	ChessBoard();

	void setBoardDisplayPosition(Vector2);

	void setTileSize(Vector2);

	void loadTexture(Texture2D[12]);

	void displayEndGame();

	void display();

	void movePiece(uint_fast8_t,uint_fast8_t, uint_fast8_t, uint_fast8_t);

	void addMove(uint_fast8_t, uint_fast8_t, uint_fast8_t, uint_fast8_t, int*);

	void addPawnMoves(uint_fast8_t, int_fast8_t, int*);

	void addKnightMoves(uint_fast8_t, uint_fast8_t, int*);

	void addBishopMoves(uint_fast8_t, uint_fast8_t, int*);

	void addRookMoves(uint_fast8_t, uint_fast8_t, int*);
	
	void addQueenMoves(uint_fast8_t, uint_fast8_t, int*);

	void addKingMoves(uint_fast8_t, uint_fast8_t, int*);
	
	void addAllMoves();

	void checkWin();

	bool isKingAttacked(bool);
	
	bool isMoveCorrect(uint_fast8_t, uint_fast8_t, uint_fast8_t, uint_fast8_t);

	bool isMoveLegal(uint_fast8_t, uint_fast8_t, uint_fast8_t, uint_fast8_t);

	void swapPieces(uint_fast8_t, uint_fast8_t, uint_fast8_t, uint_fast8_t);

	bool playMove(uint_fast8_t, uint_fast8_t, uint_fast8_t, uint_fast8_t);

	bool playMove(int move);

	void printAllMoves();

	void roquesCheck(uint_fast8_t, uint_fast8_t);
	
	string moveToString(uint_fast8_t, uint_fast8_t, uint_fast8_t, uint_fast8_t);

	void restart();

	bool getTrait();


	//AI FUNCTIONS

	void randomPlayMove();

	vector<float> chessBoardToVectorInput();

	float evaluateMove(Network, int);

	void playBestMove(Network);

	int matchNetwork(Network*, Network*);
};