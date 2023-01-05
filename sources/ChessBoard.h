#pragma once


#include<iostream>
#include<string>
#include<cstdint>
#include<cstdint>
#include <raylib.h>
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
	uint_fast8_t chessBoard[8][8] = { {4, 2, 3, 5, 6, 3, 2, 4},
									{1, 1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{7, 7, 7, 7, 7, 7, 7, 7},
									{10, 8, 9, 11, 12, 9, 8, 10}};

	//Coups légaux (2 positions par coups)
	uint_fast8_t legalMoves[maxMoves * 4];

	string chessBoardToFen();
	

	Vector2 boardDisplayPosition {50,50};
	Vector2 tileSize = {50,50};
	Texture2D piecesTexture[12];

public:
	
	ChessBoard();

	void setBoardDisplayPosition(Vector2);

	void setTileSize(Vector2);

	void loadTexture(Texture2D[12]);

	void display();

	void movePiece(uint_fast8_t,uint_fast8_t, uint_fast8_t, uint_fast8_t);

	void addMove(uint_fast8_t, uint_fast8_t, uint_fast8_t, uint_fast8_t, int*);

	void addKnightMoves(uint_fast8_t, uint_fast8_t, int*);

	void addBishopMoves(uint_fast8_t, uint_fast8_t, int*);

	void addRookMoves(uint_fast8_t, uint_fast8_t, int*);
	
	void addQueenMoves(uint_fast8_t, uint_fast8_t, int*);

	void addKingMoves(uint_fast8_t, uint_fast8_t, int*);

	void addLegalMoves();
};