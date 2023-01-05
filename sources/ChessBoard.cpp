#include "ChessBoard.h"
#include "ChessBoard.h"
//Classe représentant le plateau d'échec, les différentes coups légaux et des informations annexes (trait du joueur...)
/*
* Représentation du plateau en 287 bits -> long[9] de taille 288 bits
*
* bitset :[Pièces blances (56 bits), Pions blancs (80 bits), Pièces noires (56 bits), Pions noirs (80 bits)
*
*    Pièces blanches (8x7 = 56 bits):
* 1 bits : La pièce est-elle en jeu
* 6 bits : Position
* Dans l'ordre : Cavalier, Cavalier, Fou, Fou, Tour, Tour, Dame, Roi
*
*    Pions blancs (8x10 = 80) :
* 1 bits : La pièce est-elle en jeu
* 3 bits : Quelle pièce (000 = pion, 001 : Dame, 010 : Cavalier, 011 : Tour, 100 : Fou)
* 6 bits : Position
*
*    Pièces noires (8x7 = 56 bits):
* 1 bits : La pièce est-elle en jeu
* 6 bits : Position
* Dans l'ordre : Cavalier, Cavalier, Fou, Fou, Tour, Tour, Dame, Roi
*
*    Pions noirs (8x10 = 80) :
* 1 bits : La pièce est-elle en jeu
* 3 bits : Quelle pièce (000 = pion, 001 : Dame, 010 : Cavalier, 011 : Tour, 100 : Fou)
* 6 bits : Position
*
*    Informations supplémentaires (1 + 4 + 6 + 4 = 15 bits)
* 1 bits : Trait du joueur
* 4 bits : En passant (1 bit pour savoir si le coup est permis et 3 bits la valeur est celle de la colonne où le coup est permis)
* 6 bits : Nombre de coups de la règle des 50 coups
* 4 bits : roques disponibles (Petit roque blanc, grand roque blanc, petit roque noir, grand roque noir)
*/

ChessBoard::ChessBoard() {
	Texture2D piecesTexture[12] = { LoadTexture("resources/white_pawn.png"), LoadTexture("resources/white_knight.png"),LoadTexture("resources/white_bishop.png"), LoadTexture("resources/white_rook.png"), LoadTexture("resources/white_queen.png"),LoadTexture("resources/white_king.png"), LoadTexture("resources/black_pawn.png"), LoadTexture("resources/black_knight.png"),  LoadTexture("resources/black_bishop.png"), LoadTexture("resources/black_rook.png"), LoadTexture("resources/black_queen.png"),LoadTexture("resources/black_king.png") };

}
void ChessBoard::loadTexture(Texture2D pieces[12])
{
	for (int i = 0; i < 12; i++) {
		piecesTexture[i] = pieces[i];
	}
}
void ChessBoard::display()
{
	Vector2 rectanglePosition = boardDisplayPosition;
	Color tileColor;
	bool isBlack = false;
	
	//dessine le plateau
	for (int ligne = 0; ligne < 8; ligne++) {
		for (int colonne = 0; colonne < 8; colonne++) {
			if (isBlack) {
				tileColor = BROWN;
			}
			else {
				tileColor = LIGHTGRAY;
			}
			DrawRectangleV(rectanglePosition, tileSize, tileColor);
			uint_fast8_t piece = chessBoard[ligne][colonne];
			if (piece != 0) {
				DrawTextureEx(piecesTexture[piece - 1], rectanglePosition, 0, 1 / 12.f, RAYWHITE);
				
			}
			
			isBlack = !isBlack;
			
			rectanglePosition.x += tileSize.x;
		}
		rectanglePosition.y += tileSize.y;
		rectanglePosition.x = boardDisplayPosition.x;
		isBlack = !isBlack;
	}
}

void ChessBoard::movePiece(uint_fast8_t colonneDepart, uint_fast8_t ligneDepart, uint_fast8_t colonneArrivee, uint_fast8_t ligneArrivee) {
	uint_fast8_t swap = chessBoard[ligneArrivee][colonneArrivee];
	chessBoard[ligneArrivee][colonneArrivee] = chessBoard[ligneDepart][colonneDepart];
	chessBoard[ligneDepart][colonneDepart] = swap;
}

void ChessBoard::addMove(uint_fast8_t colonneDepart, uint_fast8_t ligneDepart, uint_fast8_t colonneDep, uint_fast8_t, int* pointerLegalMoves)
{



}



