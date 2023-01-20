#include "ChessBoard.h"
#include<chrono>
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
	trait = true;
	fill(begin(roques), end(roques), true);
	addAllMoves();
}
void ChessBoard::loadTexture(Texture2D pieces[12])
{
	for (int i = 0; i < 12; i++) {
		piecesTexture[i] = pieces[i];
	}
}
void ChessBoard::display()
{
	Color tileColor;
	bool isBlack = false;
	Vector2 mousePosition = GetMousePosition();
	Rectangle rec = { boardDisplayPosition.x, boardDisplayPosition.y, tileSize.x, tileSize.y };
	Vector2 recPos = { rec.x, rec.y };
	mouseOnBoard = false;
	//dessine le plateau
	for (int ligne = 7; ligne >=  0; ligne--) {
		for (int colonne = 0; colonne < 8; colonne++) {
			if (isBlack) {
				tileColor = BROWN;
			}
			else {
				tileColor = LIGHTGRAY;
			}
			if (CheckCollisionPointRec(mousePosition, rec)) {
				mouseOnTile[0]= colonne;
				mouseOnTile[1] = ligne;
				mouseOnBoard = true;
			}
			




			DrawRectangleRec(rec, tileColor);
			if (isPieceSelected && pieceSelected[0] == colonne && pieceSelected[1] == ligne) {
				DrawRectangleRec(rec, Fade(ORANGE, 0.3f));
			}
			uint_fast8_t piece = chessBoard[ligne][colonne];
			if (piece != 0) {
				DrawTextureEx(piecesTexture[piece - 1], { rec.x +2, rec.y +2 }, 0, 1 / 13.f, RAYWHITE);
				
			}
			



			isBlack = !isBlack;
			
			rec.x += tileSize.x;
			recPos.x = rec.x;
		}
		rec.y += tileSize.y;
		rec.x = boardDisplayPosition.x;
		isBlack = !isBlack;
	}

	//Draw Moves
	if (isPieceSelected) {
		for (int i = 0; i < nbMoves; i++) {
			if (legalMoves[i*4] == selectedTile[0] && legalMoves[i*4 + 1] == selectedTile[1]) {
				DrawCircle(boardDisplayPosition.x + (0.5f + legalMoves[i*4 +2]) * tileSize.x, boardDisplayPosition.y + (8- (legalMoves[i*4+3] + 0.5f)) * (tileSize.y), tileSize.x / 5.f, GRAY);
			}
		}
	}

	if (gameEnd) {
		DrawRectangle(boardDisplayPosition.x + 1 * tileSize.x, boardDisplayPosition.y + 2.5 * tileSize.y, 6 * tileSize.x, 2 * tileSize.y, Fade(GRAY, 0.7));
		if (win) {

			if (whiteWin) {
				DrawText("White won", boardDisplayPosition.x + 1.5 * tileSize.x, boardDisplayPosition.y + 3 * tileSize.y, tileSize.x, BLACK);
			}
			else {
				DrawText("Black won", boardDisplayPosition.x + 1.5 * tileSize.x, boardDisplayPosition.y + 3 * tileSize.y, tileSize.x, BLACK);
			}
		}
		else {
			DrawText("It's a draw", boardDisplayPosition.x + 1.25 * tileSize.x, boardDisplayPosition.y + 3 * tileSize.y, tileSize.x, BLACK);
		}
	}


	//gère les évenements sur le plateau
	if (mouseOnBoard) {
		
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			selectedTile[0] = mouseOnTile[0];
			selectedTile[1] = mouseOnTile[1];
			uint_fast8_t tileValue = chessBoard[selectedTile[1]][selectedTile[0]];
			if (isPieceSelected) {
				if (playMove(pieceSelected[0], pieceSelected[1], selectedTile[0], selectedTile[1])) {
					isPieceSelected = false;
				};

			}
			if (tileValue > 0 && tileValue <= 12) {
				isPieceSelected = false;
				if (trait && tileValue < 7) {
					isPieceSelected = true;
					pieceSelected[0] = mouseOnTile[0];
					pieceSelected[1] = mouseOnTile[1];
				}
				if (!trait && tileValue > 6) {

					isPieceSelected = true;
					pieceSelected[0] = mouseOnTile[0];
					pieceSelected[1] = mouseOnTile[1];
				}
			}
		}
	}





}

void ChessBoard::movePiece(uint_fast8_t colonneDepart, uint_fast8_t ligneDepart, uint_fast8_t colonneArrivee, uint_fast8_t ligneArrivee) {
	uint_fast8_t swap = chessBoard[ligneArrivee][colonneArrivee];
	chessBoard[ligneArrivee][colonneArrivee] = chessBoard[ligneDepart][colonneDepart];
	chessBoard[ligneDepart][colonneDepart] = swap;
}

void ChessBoard::addMove(uint_fast8_t colonneDepart, uint_fast8_t ligneDepart, uint_fast8_t colonneArrivee, uint_fast8_t ligneArrivee, int* pointerLegalMoves)
{
	if (*pointerLegalMoves > maxMoves * 4) {
		cout << "Too much moves\n";
		return;
	}
	legalMoves[*pointerLegalMoves] = colonneDepart;
	legalMoves[*pointerLegalMoves +1] = ligneDepart;
	legalMoves[*pointerLegalMoves +2] = colonneArrivee;
	legalMoves[*pointerLegalMoves +3] = ligneArrivee;
	*pointerLegalMoves += 4;
	nbMoves++;
}

void ChessBoard::addPawnMoves(uint_fast8_t colonne, int_fast8_t ligne, int* iterator)
{
	if (trait) {
		if (isMoveLegal(colonne, ligne, colonne, ligne + 1)) {
			addMove(colonne, ligne, colonne, ligne + 1, iterator);
		}
		if (isMoveLegal(colonne, ligne, colonne +1, ligne + 1)) {
			addMove(colonne, ligne, colonne +1, ligne + 1, iterator);
		}
		if (isMoveLegal(colonne, ligne, colonne-1, ligne + 1)) {
			addMove(colonne, ligne, colonne -1, ligne + 1, iterator);
		}
		if (isMoveLegal(colonne, ligne, colonne, ligne + 2)) {
			addMove(colonne, ligne, colonne, ligne + 2, iterator);
		}
	}
	else {
		if (isMoveLegal(colonne, ligne, colonne, ligne - 1)) {
			addMove(colonne, ligne, colonne, ligne - 1, iterator);
		}
		if (isMoveLegal(colonne, ligne, colonne + 1, ligne - 1)) {
			addMove(colonne, ligne, colonne +1, ligne - 1, iterator);
		}
		if (isMoveLegal(colonne, ligne, colonne - 1, ligne - 1)) {
			addMove(colonne, ligne, colonne-1, ligne - 1, iterator);
		}
		if (isMoveLegal(colonne, ligne, colonne, ligne - 2)) {
			addMove(colonne, ligne, colonne, ligne - 2, iterator);
		}
	}
}

void ChessBoard::addKnightMoves(uint_fast8_t colonne , uint_fast8_t ligne, int* iterator)
{
	int knight[4] = { -2,-1,2,1 };
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i != j && i + 2 != j && j + 2 != i) {
				if (isMoveLegal(colonne, ligne, colonne + knight[i], ligne + knight[j])) addMove(colonne, ligne, colonne + knight[i], ligne + knight[j], iterator);
			}
		}
	}
}

void ChessBoard::addBishopMoves(uint_fast8_t colonne, uint_fast8_t ligne, int* iterator)
{
	for (int i = 1; i < 8; i++) {
		if (isMoveLegal(colonne, ligne, colonne + i, ligne + i)) addMove(colonne, ligne, colonne + i, ligne + i, iterator);
		if (isMoveLegal(colonne, ligne, colonne - i, ligne + i)) addMove(colonne, ligne, colonne - i, ligne + i, iterator);
		if (isMoveLegal(colonne, ligne, colonne + i, ligne - i)) addMove(colonne, ligne, colonne + i, ligne - i, iterator);
		if (isMoveLegal(colonne, ligne, colonne - i, ligne - i)) addMove(colonne, ligne, colonne - i, ligne - i, iterator);
	}
}

void ChessBoard::addRookMoves(uint_fast8_t colonne, uint_fast8_t ligne, int* iterator)
{
	for (int i = 1; i < 8; i++) {
		if (isMoveLegal(colonne, ligne, colonne + i, ligne)) addMove(colonne, ligne, colonne + i, ligne, iterator);
		if (isMoveLegal(colonne, ligne, colonne - i, ligne)) addMove(colonne, ligne, colonne - i, ligne, iterator);
		if (isMoveLegal(colonne, ligne, colonne, ligne + i)) addMove(colonne, ligne, colonne, ligne + i, iterator);
		if (isMoveLegal(colonne, ligne, colonne, ligne - i)) addMove(colonne, ligne, colonne, ligne - i, iterator);
	}
}

void ChessBoard::addQueenMoves(uint_fast8_t colonne, uint_fast8_t ligne, int* iterator)
{
	addBishopMoves(colonne, ligne, iterator);
	addRookMoves(colonne, ligne, iterator);
}

void ChessBoard::addKingMoves(uint_fast8_t colonne, uint_fast8_t ligne, int* iterator)
{
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			if (i != 0 || j != 0) {
				if (isMoveLegal(colonne, ligne, colonne + i, ligne + j)) addMove(colonne, ligne, colonne + i, ligne + j, iterator);
			}
		}
	}
}

void ChessBoard::addAllMoves()
{ 
	nbMoves = 0;
	fill(begin(legalMoves), end(legalMoves), 0);
	int ptrLegalMoves = 0;
	uint_fast8_t piece;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			piece = chessBoard[i][j];
			switch (piece) {
			case 0: break;
			case 1: case 7:
				addPawnMoves(j, i, &ptrLegalMoves);
				break;
			case 2: case 8:
				addKnightMoves(j, i, &ptrLegalMoves);
				break;
			case 3: case 9:
				addBishopMoves(j, i, &ptrLegalMoves);
				break;
			case 4: case 10:
				addRookMoves(j, i, &ptrLegalMoves);
				break;
			case 5: case 11:
				addQueenMoves(j, i, &ptrLegalMoves);
				break;
			case 6: case 12 :
				addKingMoves(j, i, &ptrLegalMoves);
				break;
			default:
				for (int i2 = 0; i2 < 8; i2++) {
					for (int j2 = 0; j2 < 8; j2++) {

						if (isMoveLegal(j, i, j2, i2)) {
							addMove(j, i, j2, i2, &ptrLegalMoves);

						}
					}
				}
				break;
			}
		}
	}
}

void ChessBoard::checkWin()
{ 
	if (nbMoves == 0) {
		if (isKingAttacked(trait)) {

			
			win = true;
			whiteWin = !trait;
			if (trait) {
				//cout << "BLACK WIN\n";
			}
			//else cout << "WHITE WIN\n";
		}
		else {
			//cout << "STALEMATE\n";
			draw = true;
		}
		gameEnd = true;
	}
	if (fiftyMove >= 100 || nbCoups >50) {
		//cout << "50 MOVES\n";
		draw = true;
		gameEnd = true;
	}
	if (nbPawns == 0 && nbQueens == 0 && nbRooks == 0 && nbBishops == 0 && nbKnights == 0) {
		cout << "NOT ENOUGH MATERIAL\n";
		draw = true;
		gameEnd = true;
	}
}

bool ChessBoard::isKingAttacked(bool color) {
	uint_fast8_t king;
	uint_fast8_t kingLigne = 8;
	uint_fast8_t kingColonne = 8;
	bool ret = false;
	if (color) {
		king = 6;
	}
	else {
		king = 12;
	}
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (chessBoard[i][j] == king) {
				kingLigne = i;
				kingColonne = j;
			}
		}
	}
	
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			uint_fast8_t piece = chessBoard[i][j];
			if (i == 1 && j == 4) {
			}
			if (piece != 0) {
				if (piece < 7 && !color) {
					ret |= isMoveCorrect(j, i, kingColonne, kingLigne);
				}
				if (piece >= 7 && color) {
					ret |= isMoveCorrect(j, i, kingColonne, kingLigne);
				}
			}
		}
	}
	return ret;




}


//Check if a move makes sense regarding the rules of the pieces involved and the state of the board. Do not take into account king safety nor trait 
bool ChessBoard::isMoveCorrect(uint_fast8_t colonneDepart, uint_fast8_t ligneDepart, uint_fast8_t colonneArrivee, uint_fast8_t ligneArrivee) {

	if (colonneDepart >= 8 || ligneDepart >= 8 || colonneArrivee >= 8 || ligneArrivee >= 8 || colonneDepart < 0 || ligneDepart < 0 || colonneArrivee < 0 || ligneArrivee < 0) {
		return false;
	}

	uint_fast8_t piece = chessBoard[ligneDepart][colonneDepart];
	uint_fast8_t pieceArrivee = chessBoard[ligneArrivee][colonneArrivee];

	bool b = true;
	bool classic = true;
	bool roque = true;
	switch (piece) {
	case 0:
		//pas de pièces
		return false;
	case 1:
		//pion
		if (colonneDepart == colonneArrivee && ligneDepart + 1 == ligneArrivee) {
			return (pieceArrivee == 0);
		}
		if (colonneDepart == colonneArrivee && ligneDepart == 1 && ligneArrivee == 3) {
			return (pieceArrivee == 0 && chessBoard[ligneDepart + 1][colonneDepart] == 0);
		}
		b = (colonneDepart == colonneArrivee + 1 || colonneDepart == colonneArrivee - 1);

		//TODO : Prise en passant
		if (b && ligneDepart + 1 == ligneArrivee) {
			if (enPassantOk && enPassant == colonneArrivee && ligneDepart == 4)//En passant {
				return true;
			return (pieceArrivee > 6);
		}
		return false;
		break;
	case 2:
		//cavalier
		b = ((colonneDepart == colonneArrivee + 1 || colonneDepart == colonneArrivee - 1) ^ (ligneDepart == ligneArrivee + 1 || ligneDepart == ligneArrivee - 1)) && ((colonneDepart == colonneArrivee + 2 || colonneDepart == colonneArrivee - 2) ^ (ligneDepart == ligneArrivee + 2 || ligneDepart == ligneArrivee - 2));
		if (b) {

			return (pieceArrivee == 0 || pieceArrivee > 6);
		}
		break;
	case 3:
		//fou
		for (int i = 1; i < 8; i++) { //On trouve quelle diagonale
			if (colonneDepart + i == colonneArrivee) {
				if (ligneDepart + i == ligneArrivee) {

					//Vérification de si la diagonale est disponible
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart + j][colonneDepart + j] != 0) return false;
					}
					return (pieceArrivee == 0 || pieceArrivee > 6);
				}
				if (ligneDepart - i == ligneArrivee) {
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart - j][colonneDepart + j] != 0) return false;
					}
					return (pieceArrivee == 0 || pieceArrivee > 6);
				}
			}
			if (colonneDepart - i == colonneArrivee) {
				if (ligneDepart + i == ligneArrivee) {
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart + j][colonneDepart - j] != 0) return false;
					}
					return (pieceArrivee == 0 || pieceArrivee > 6);
				}
				if (ligneDepart - i == ligneArrivee) {
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart - j][colonneDepart - j] != 0) return false;
					}
					return (pieceArrivee == 0 || pieceArrivee > 6);
				}
			}
		}
		return false;
		break;
	case 4:
		//tour
		if (colonneDepart == colonneArrivee) { //Déplacement vertical 
			for (int i = 1; i < 8; i++) {
				if (ligneDepart + i == ligneArrivee) { //Haut
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart + j][colonneDepart] != 0) return false;
					}
					return (pieceArrivee == 0 || pieceArrivee > 6);
				}

				if (ligneDepart - i == ligneArrivee) { //Bas
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart - j][colonneDepart] != 0) return false;
					}
					return (pieceArrivee == 0 || pieceArrivee > 6);
				}
			}
		}
		if (ligneDepart == ligneArrivee) { //Déplacement horizontal
			for (int i = 1; i < 8; i++) {
				if (colonneDepart + i == colonneArrivee) { //Droite
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart][colonneDepart + j] != 0) return false;
					}
					return (pieceArrivee == 0 || pieceArrivee > 6);
				}

				if (colonneDepart - i == colonneArrivee) { //Gauche
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart][colonneDepart - j] != 0) return false;
					}
					return (pieceArrivee == 0 || pieceArrivee > 6);
				}
			}
		}
		return false;
		break;
	case 5:
		//queen (optimisable en combinant les loops)

		//Deplacement en ligne
		if (colonneDepart == colonneArrivee) { //Déplacement vertical 
			for (int i = 1; i < 8; i++) {
				if (ligneDepart + i == ligneArrivee) { //Haut
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart + j][colonneDepart] != 0) return false;
					}
					return (pieceArrivee == 0 || pieceArrivee > 6);
				}

				if (ligneDepart - i == ligneArrivee) { //Bas
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart - j][colonneDepart] != 0) return false;
					}
					return (pieceArrivee == 0 || pieceArrivee > 6);
				}
			}
		}
		if (ligneDepart == ligneArrivee) { //Déplacement horizontal
			for (int i = 1; i < 8; i++) {
				if (colonneDepart + i == colonneArrivee) { //Droite
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart][colonneDepart + j] != 0) return false;
					}
					return (pieceArrivee == 0 || pieceArrivee > 6);
				}

				if (colonneDepart - i == colonneArrivee) { //Gauche
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart][colonneDepart - j] != 0) return false;
					}
					return (pieceArrivee == 0 || pieceArrivee > 6);
				}
			}
		}


		//Deplacement en diagonale
		for (int i = 1; i < 8; i++) { //On trouve quelle diagonale
			if (colonneDepart + i == colonneArrivee) {
				if (ligneDepart + i == ligneArrivee) {

					//Vérification de si la diagonale est disponible
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart + j][colonneDepart + j] != 0) return false;
					}
					return (pieceArrivee == 0 || pieceArrivee > 6);
				}
				if (ligneDepart - i == ligneArrivee) {
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart - j][colonneDepart + j] != 0) return false;
					}
					return (pieceArrivee == 0 || pieceArrivee > 6);
				}
			}
			if (colonneDepart - i == colonneArrivee) {
				if (ligneDepart + i == ligneArrivee) {
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart + j][colonneDepart - j] != 0) return false;
					}
					return (pieceArrivee == 0 || pieceArrivee > 6);
				}
				if (ligneDepart - i == ligneArrivee) {
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart - j][colonneDepart - j] != 0) return false;
					}
					return (pieceArrivee == 0 || pieceArrivee > 6);
				}
			}
		}


		return false;
		break;

	case 6:
		//king
		classic = (colonneDepart != colonneArrivee || ligneDepart != ligneArrivee);
		classic &= (colonneDepart == colonneArrivee + 1) || (colonneDepart == colonneArrivee) || (colonneDepart == colonneArrivee - 1);
		classic &= (ligneDepart == ligneArrivee + 1) || (ligneDepart == ligneArrivee) || (ligneDepart == ligneArrivee - 1);
		if (roques[0] && chessBoard[0][5] == 0 && chessBoard[0][6] == 0 && colonneDepart == 4 && ligneDepart == 0 && colonneArrivee == 6 && ligneArrivee == 0) {
			roque &= !isKingAttacked(true);
			swapPieces(4, 0, 5, 0);
			roque &= !isKingAttacked(true);
			swapPieces(5, 0, 6, 0);
			roque &= !isKingAttacked(true);
			swapPieces(6, 0, 4, 0);
			return true;
		}

		if (roques[1] && chessBoard[0][3] == 0 && chessBoard[0][2] == 0 && colonneDepart == 4 && ligneDepart == 0 && colonneArrivee == 2 && ligneArrivee == 0) {
			roque &= !isKingAttacked(true);
			swapPieces(4, 0, 3, 0);
			roque &= !isKingAttacked(true);
			swapPieces(5, 0, 2, 0);
			roque &= !isKingAttacked(true);
			swapPieces(2, 0, 4, 0);
			return true;
		}
		return ((classic) && (pieceArrivee == 0 || pieceArrivee > 6));
		break;



		//PIECES NOIRES
	case 7:
		//pion
		if (colonneDepart == colonneArrivee && ligneDepart - 1 == ligneArrivee) {
			return (pieceArrivee == 0);
		}
		if (colonneDepart == colonneArrivee && ligneDepart ==  6 && ligneArrivee == 4) {
			return (pieceArrivee == 0 && chessBoard[ligneDepart - 1][colonneDepart] == 0);
		}
		b = (colonneDepart == colonneArrivee + 1 || colonneDepart == colonneArrivee - 1);

		//TODO : Prise en passant
		if (b && ligneDepart - 1 == ligneArrivee) {

			if (enPassantOk && enPassant == colonneArrivee && ligneDepart == 3)//En passant {
				return true;
			return (pieceArrivee <= 6 && pieceArrivee != 0);
		}
		return false;
		break;
	case 8:
		//cavalier
		b = ((colonneDepart == colonneArrivee + 1 || colonneDepart == colonneArrivee - 1) ^ (ligneDepart == ligneArrivee + 1 || ligneDepart == ligneArrivee - 1)) && ((colonneDepart == colonneArrivee + 2 || colonneDepart == colonneArrivee - 2) ^ (ligneDepart == ligneArrivee + 2 || ligneDepart == ligneArrivee - 2));
		if (b) {

			return (pieceArrivee <= 6);
		}
		break;
	case 9:
		//fou
		for (int i = 1; i < 8; i++) { //On trouve quelle diagonale
			if (colonneDepart + i == colonneArrivee) {
				if (ligneDepart + i == ligneArrivee) {

					//Vérification de si la diagonale est disponible
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart + j][colonneDepart + j] != 0) return false;
					}
					return (pieceArrivee <= 6);
				}
				if (ligneDepart - i == ligneArrivee) {
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart - j][colonneDepart + j] != 0) return false;
					}
					return (pieceArrivee <= 6);
				}
			}
			if (colonneDepart - i == colonneArrivee) {
				if (ligneDepart + i == ligneArrivee) {
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart + j][colonneDepart - j] != 0) return false;
					}
					return (pieceArrivee <= 6);
				}
				if (ligneDepart - i == ligneArrivee) {
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart - j][colonneDepart - j] != 0) return false;
					}
					return (pieceArrivee <= 6);
				}
			}
		}
		return false;
		break;
	case 10:
		//tour
		if (colonneDepart == colonneArrivee) { //Déplacement vertical 
			for (int i = 1; i < 8; i++) {
				if (ligneDepart + i == ligneArrivee) { //Haut
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart + j][colonneDepart] != 0) return false;
					}
					return (pieceArrivee <= 6);
				}

				if (ligneDepart - i == ligneArrivee) { //Bas
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart - j][colonneDepart] != 0) return false;
					}
					return (pieceArrivee <= 6);
				}
			}
		}
		if (ligneDepart == ligneArrivee) { //Déplacement horizontal
			for (int i = 1; i < 8; i++) {
				if (colonneDepart + i == colonneArrivee) { //Droite
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart][colonneDepart + j] != 0) return false;
					}
					return (pieceArrivee <= 6);
				}

				if (colonneDepart - i == colonneArrivee) { //Gauche
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart][colonneDepart - j] != 0) return false;
					}
					return (pieceArrivee <= 6);
				}
			}
		}
		return false;
		break;
	case 11:
		//queen (optimisable en combinant les loops)

		//Deplacement en ligne
		if (colonneDepart == colonneArrivee) { //Déplacement vertical 
			for (int i = 1; i < 8; i++) {
				if (ligneDepart + i == ligneArrivee) { //Haut
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart + j][colonneDepart] != 0) return false;
					}
					return (pieceArrivee <= 6);
				}

				if (ligneDepart - i == ligneArrivee) { //Bas
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart - j][colonneDepart] != 0) return false;
					}
					return (pieceArrivee <= 6);
				}
			}
		}
		if (ligneDepart == ligneArrivee) { //Déplacement horizontal
			for (int i = 1; i < 8; i++) {
				if (colonneDepart + i == colonneArrivee) { //Droite
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart][colonneDepart + j] != 0) return false;
					}
					return (pieceArrivee <= 6);
				}

				if (colonneDepart - i == colonneArrivee) { //Gauche
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart][colonneDepart - j] != 0) return false;
					}
					return (pieceArrivee <= 6);
				}
			}
		}


		//Deplacement en diagonale
		for (int i = 1; i < 8; i++) { //On trouve quelle diagonale
			if (colonneDepart + i == colonneArrivee) {
				if (ligneDepart + i == ligneArrivee) {

					//Vérification de si la diagonale est disponible
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart + j][colonneDepart + j] != 0) return false;
					}
					return (pieceArrivee <= 6);
				}
				if (ligneDepart - i == ligneArrivee) {
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart - j][colonneDepart + j] != 0) return false;
					}
					return (pieceArrivee <= 6);
				}
			}
			if (colonneDepart - i == colonneArrivee) {
				if (ligneDepart + i == ligneArrivee) {
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart + j][colonneDepart - j] != 0) return false;
					}
					return (pieceArrivee <= 6);
				}
				if (ligneDepart - i == ligneArrivee) {
					for (int j = 1; j < i; j++) {
						if (chessBoard[ligneDepart - j][colonneDepart - j] != 0) return false;
					}
					return (pieceArrivee <= 6);
				}
			}
		}


		return false;
		break;

	case 12:
		//king
		classic = (colonneDepart != colonneArrivee || ligneDepart != ligneArrivee);
		classic &= (colonneDepart == colonneArrivee + 1) || (colonneDepart == colonneArrivee) || (colonneDepart == colonneArrivee - 1);
		classic &= (ligneDepart == ligneArrivee + 1) || (ligneDepart == ligneArrivee) || (ligneDepart == ligneArrivee - 1);
		if (roques[2] && chessBoard[7][5] == 0 && chessBoard[7][6] == 0 && colonneDepart == 4 && ligneDepart == 7 && colonneArrivee == 6 && ligneArrivee == 7) {
			roque &= !isKingAttacked(true);
			swapPieces(4, 7, 5, 7);
			roque &= !isKingAttacked(true);
			swapPieces(5, 7, 6, 7);
			roque &= !isKingAttacked(true);
			swapPieces(6, 7, 4, 7);
			return true;
		}
		if (roques[3] && chessBoard[7][3] == 0 && chessBoard[7][2] == 0 && colonneDepart == 4 && ligneDepart == 7 && colonneArrivee == 2 && ligneArrivee == 7) {
			roque &= !isKingAttacked(true);
			swapPieces(4, 7, 3, 7);
			roque &= !isKingAttacked(true);
			swapPieces(3, 7, 2, 7);
			roque &= !isKingAttacked(true);
			swapPieces(2, 7, 4, 7);
			return true;
		}
		return (classic && (pieceArrivee <= 6));
		break;


	default:
		cout << "isMoveCorrect : piece not recognised";
		return false;
		break;
	}

}


//Check if a move is legal in current position
bool ChessBoard::isMoveLegal(uint_fast8_t colonneDepart, uint_fast8_t ligneDepart, uint_fast8_t colonneArrivee, uint_fast8_t ligneArrivee)
{
	uint_fast8_t piece = chessBoard[ligneDepart][colonneDepart];
	uint_fast8_t pieceArrivee = chessBoard[ligneArrivee][colonneArrivee];
	bool legal = piece > 0 && piece <= 12;
	legal &= trait ^ (piece >= 7); //Le joueur joue une pièce à lui
	legal &= isMoveCorrect(colonneDepart, ligneDepart, colonneArrivee, ligneArrivee);
	swapPieces(colonneDepart, ligneDepart, colonneArrivee, ligneArrivee);
	chessBoard[ligneDepart][colonneDepart] = 0;
	legal &= !isKingAttacked(trait);
	swapPieces(colonneDepart, ligneDepart, colonneArrivee, ligneArrivee);
	chessBoard[ligneArrivee][colonneArrivee] = pieceArrivee;
	return legal;
}

void ChessBoard::swapPieces(uint_fast8_t colonneDepart, uint_fast8_t ligneDepart, uint_fast8_t colonneArrivee, uint_fast8_t ligneArrivee)
{
	uint_fast8_t swap = chessBoard[ligneDepart][colonneDepart];
	chessBoard[ligneDepart][colonneDepart] = chessBoard[ligneArrivee][colonneArrivee];
	chessBoard[ligneArrivee][colonneArrivee] = swap;
}

bool ChessBoard::playMove(uint_fast8_t colonneDepart, uint_fast8_t ligneDepart, uint_fast8_t colonneArrivee, uint_fast8_t ligneArrivee)
{
	nbCoups++; //Mesure de sécurité
	for (int i = 0; i < nbMoves; i++) {
		if (!gameEnd && (legalMoves[i * 4] == colonneDepart && legalMoves[i * 4 + 1] == ligneDepart && legalMoves[i * 4 + 2] == colonneArrivee && legalMoves[i * 4 + 3] == ligneArrivee)) {
			fiftyMove++;
			
			uint_fast8_t piece = chessBoard[ligneDepart][colonneDepart];
			uint_fast8_t pieceArrivee = chessBoard[ligneArrivee][colonneArrivee];
			roquesCheck(colonneDepart, ligneDepart);
			

			//Play Roque :
			if (colonneDepart == 4 && ligneDepart == 0) {
				if (colonneArrivee == 6 && ligneArrivee == 0 && piece == 6) {
					//Petit Roque
					swapPieces(5, 0, 7, 0);
				}
				if (colonneArrivee == 2 && ligneArrivee == 0 && piece == 6) {
					//Grand Roque
					swapPieces(2, 0, 0, 0);
				}
			}
			if (colonneDepart == 4 && ligneDepart == 7) {
				if (colonneArrivee == 6 && ligneArrivee == 7 && piece == 12) {
					//Petit Roque
					swapPieces(5, 7, 7, 7);
				}
				if (colonneArrivee == 2 && ligneArrivee == 7 && piece == 12) {
					//Grand Roque
					swapPieces(3, 7, 0, 7);
				}
			}


			enPassantOk = false;
			//Pawn move
			if (piece == 1 || piece == 7) {
				//Register EnPassant
				if (ligneDepart + 2 == ligneArrivee || ligneDepart - 2 == ligneArrivee) {
					enPassantOk = true;
					enPassant = colonneDepart;
				}
				//Eat pawn EnPassant
				if ((colonneDepart == colonneArrivee + 1 || colonneDepart == colonneArrivee - 1) && chessBoard[ligneArrivee][colonneArrivee] == 0) {
					chessBoard[ligneDepart][colonneArrivee] = 0;
				}
				//Promotion (Queen for now)
				if (ligneArrivee == 7) {
					chessBoard[ligneDepart][colonneDepart] = 5;
					nbQueens++;
				}
				if (ligneArrivee == 0) {
					chessBoard[ligneDepart][colonneDepart] = 11;
					nbQueens++;
				}
			}

			if (piece == 1 || pieceArrivee != 0) {
				fiftyMove = 0;
			}
			swapPieces(colonneDepart, ligneDepart, colonneArrivee, ligneArrivee);
			chessBoard[ligneDepart][colonneDepart] = 0;
			
			switch (pieceArrivee) {
			case 1: case 7: nbPawns--; break;
			case 2: case 8: nbKnights--; break;
			case 3: case 9: nbBishops--; break;
			case 4: case 10: nbRooks--; break;
			case 5: case 11: nbQueens--; break;

			}


			trait = !trait;
			addAllMoves();
			checkWin();
			


			return true;
		}
	}
	return false;
}

bool ChessBoard::playMove(int move)
{

	if (move >= nbMoves) {
		return false;
	}
	return playMove(legalMoves[move*4], legalMoves[move*4+1], legalMoves[move*4+2], legalMoves[move*4+3]);
}

void ChessBoard::printAllMoves()
{
	
	for (int i = 0; i < nbMoves; i++) {
		cout << moveToString(legalMoves[i * 4], legalMoves[i * 4 + 1], legalMoves[i * 4 + 2], legalMoves[i * 4 + 3]);

		if (i < nbMoves - 1) {
			cout << "; ";
		}
	}
	cout << '\n';
}

void ChessBoard::roquesCheck(uint_fast8_t colonneDepart, uint_fast8_t ligneDepart)
{
	if (ligneDepart == 0) {

		roques[0] &= (colonneDepart != 7 && colonneDepart != 4);
		roques[1] &= (colonneDepart != 0 && colonneDepart != 4);
	}
	if (ligneDepart == 7) {
		roques[2] &= (colonneDepart != 7 && colonneDepart != 4);
		roques[3] &= (colonneDepart != 0 && colonneDepart != 4);
	}
}

string ChessBoard::moveToString(uint_fast8_t colonneDepart, uint_fast8_t ligneDepart, uint_fast8_t colonneArrivee, uint_fast8_t ligneArrivee)
{
	string ret;
	uint_fast8_t piece = chessBoard[ligneDepart][colonneDepart];
	uint_fast8_t pieceArrivee = chessBoard[ligneArrivee][colonneArrivee];
	
	if (piece >= 7) {
		piece -= 6;
	}
	if (pieceArrivee >= 7) {
		pieceArrivee -= 6;
	}
	if (piece > 1) {
		ret += pieceStr[piece - 2];
	}
	if (pieceArrivee != 0) {

		//TODO: check if same piece can take
		if (piece == 1) ret += clm[colonneDepart];
		ret+= 'x';
	}
	ret+= clm[colonneArrivee] + std::to_string(ligneArrivee + 1);

	return ret;

}

void ChessBoard::restart()
{
	/*uint_fast8_t newBoard[8][8] = {{4, 2, 3, 5, 6, 3, 2, 4},
									{1, 1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{7, 7, 7, 7, 7, 7, 7, 7},
									{10, 8, 9, 11, 12, 9, 8, 10} };*/
	uint_fast8_t newBoard[8][8] = { {0, 0, 0, 5, 6, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0},
									{7, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 12, 0, 0,0} };
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			chessBoard[i][j] = newBoard[i][j];
		}
	}

	enPassantOk = false;
	trait = true;
	fill(begin(roques), end(roques), true);
	addAllMoves();
	gameEnd = false;
	win = false;
	draw = false;
	whiteWin = false;
	isPieceSelected = false;
	fiftyMove = 0;
	nbCoups = 0;
	nbQueens = 2;
	nbRooks = 4;
	nbBishops = 4;
	nbKnights = 4;
	nbPawns = 16;
}

bool ChessBoard::getTrait() {
	return trait;
}

void ChessBoard::randomPlayMove()
{
	if (!(gameEnd)) {
		int move = GetRandomValue(0, nbMoves - 1);
		cout << "test";
		playMove(move);
	}
}

vector<float> ChessBoard::chessBoardToVectorInput()
{
	vector<float> input(285, 0);
	
	fill(input.begin(), input.end(), 0.f);
	uint_fast8_t piece;
	int nbWhitePawn = 0;
	int nbWhiteKnight = 0;
	int nbWhiteBishop = 0;
	int nbWhiteRook = 0;
	int nbWhiteQueen = 0;
	int nbBlackPawn = 0;
	int nbBlackKnight = 0;
	int nbBlackBishop = 0;
	int nbBlackRook = 0;
	int nbBlackQueen = 0;
	int index = 0;
	for (int ligneCount = 0; ligneCount < 8; ligneCount++) {
		for (int colonneCount = 0; colonneCount < 8; colonneCount++) {
			int ligne = ligneCount;
			int colonne = colonneCount;
			piece = chessBoard[ligne][colonne];

			

			switch (piece) {
			case 0: break;
			case 1:
				index = 56 + 10 * nbWhitePawn;
				input[index] = 1;
				for (int k = 0; k < 3; k++) {
					input[index + 1 + k] = ((ligne >> k) & 1);
					input[index + 4 + k] = ((colonne >> k) & 1);
					//input[index + 7 + k] = 0;
				}

				nbWhitePawn++;
				break;
			case 2:
				
				if (nbWhiteKnight <= 1) {				
					index = nbWhiteKnight * 7;
					input[index] = 1;
					for (int k = 0; k < 3; k++) {
						input[index + 1 + k] = (ligne >> k) & 1;
						input[index + 4 + k] = (colonne >> k) & 1;
					}
					nbWhiteKnight++;
				}
				else {
					index = 56 + 10 * nbWhitePawn;
					input[index] = 1;
					for (int k = 0; k < 3; k++) {
						input[index + 1 + k] = (ligne >> k) & 1;
						input[index + 4 + k] = (colonne >> k) & 1;
					}
					input[index + 9] = 1;
					nbWhitePawn++;
				}
				break;
			case 3:
				if (nbWhiteBishop <= 1) {
					index = 14 + nbWhiteBishop * 7;
					input[index] = 1;
					for (int k = 0; k < 3; k++) {
						input[index + 1 + k] = (ligne >> k) & 1;
						input[index + 4 + k] = (colonne >> k) & 1;
					}
					nbWhiteBishop++;
				}
				else {
					index = 56 + 10 * nbWhitePawn;
					input[index] = 1;
					for (int k = 0; k < 3; k++) {
						input[index + 1 + k] = (ligne >> k) & 1;
						input[index + 4 + k] = (colonne >> k) & 1;
					}

					input[index + 8] = 1;
					nbWhitePawn++;
				}
				break;
			case 4:
				if (nbWhiteRook <= 1) {
					index = 28 + nbWhiteRook * 7;
					input[index] = 1;
					for (int k = 0; k < 3; k++) {
						input[index + 1 + k] = (ligne >> k) & 1;
						input[index + 4 + k] = (colonne >> k) & 1;
					}
					nbWhiteRook++;
				}
				else {
					index = 56 + 10 * nbWhitePawn;
					input[index] = 1;
					for (int k = 0; k < 3; k++) {
						input[index + 1 + k] = (ligne >> k) & 1;
						input[index + 4 + k] = (colonne >> k) & 1;
					}

					input[index + 8] = 1;
					input[index + 9] = 1;
					nbWhitePawn++;
				}
				break;
			case 5:
				if (nbWhiteQueen == 0) {
					index = 42;
					input[index] = 1;
					for (int k = 0; k < 3; k++) {
						input[index + 1 + k] = (ligne >> k) & 1;
						input[index + 4 + k] = (colonne >> k) & 1;
					}
					nbWhiteQueen++;
				}
				else {
					index = 56 + 10 * nbWhitePawn;
					input[index] = 1;
					for (int k = 0; k < 3; k++) {
						input[index + 1 + k] = (ligne >> k) & 1;
						input[index + 4 + k] = (colonne >> k) & 1;
					}
					input[index + 7] = 1;
					nbWhitePawn++;
				}
				break;
			case 6:

				index = 49;
				input[index] = 1;
				for (int k = 0; k < 3; k++) {
					input[index + 1 + k] = (ligne >> k) & 1;
					input[index + 4 + k] = (colonne >> k) & 1;
				}
				break;


				//BLACK PIECES
			case 7:

				index = 192 + 10 * nbBlackPawn;
				input[index] = 1;
				for (int k = 0; k < 3; k++) {
					input[index + 1 + k] = (ligne >> k) & 1;
					input[index + 4 + k] = (colonne >> k) & 1;
					//input[index + 7 + k] = 0;
				}

				nbBlackPawn++;
				break;
			case 8:
				if (nbBlackKnight <= 1) {
					index = 136 + nbBlackKnight * 7;
					input[index] = 1;
					for (int k = 0; k < 3; k++) {
						input[index + 1 + k] = ((ligne >> k) & 1);
						input[index + 4 + k] = ((colonne >> k) & 1);
					}
					nbBlackKnight++;
				}
				else {
					index = 192 + 10 * nbBlackPawn;
					input[index] = 1;
					for (int k = 0; k < 3; k++) {
						input[index + 1 + k] = (ligne >> k) & 1;
						input[index + 4 + k] = (colonne >> k) & 1;
					}
					input[index + 9] = 1;
					nbBlackPawn++;
				}
				break;
			case 9:
				if (nbBlackBishop <= 1) {
					index = 150 + nbBlackBishop * 7;
					input[index] = 1;
					for (int k = 0; k < 3; k++) {
						input[index + 1 + k] = (ligne >> k) & 1;
						input[index + 4 + k] = (colonne >> k) & 1;
					}
					nbBlackBishop++;
				}
				else {
					index = 192 + 10 * nbBlackPawn;
					input[index] = 1;
					for (int k = 0; k < 3; k++) {
						input[index + 1 + k] = (ligne >> k) & 1;
						input[index + 4 + k] = (colonne >> k) & 1;
					}

					input[index + 8] = 1;
					nbBlackPawn++;
				}
				break;
			case 10:
				if (nbBlackRook <= 1) {
					index = 164 + nbBlackRook * 7;
					input[index] = 1;
					for (int k = 0; k < 3; k++) {
						input[index + 1 + k] = (ligne >> k) & 1;
						input[index + 4 + k] = (colonne >> k) & 1;
					}
					nbBlackRook++;
				}
				else {
					index = 192 + 10 * nbBlackPawn;
					input[index] = 1;
					for (int k = 0; k < 3; k++) {
						input[index + 1 + k] = (ligne >> k) & 1;
						input[index + 4 + k] = (colonne >> k) & 1;
					}

					input[index + 8] = 1;
					input[index + 9] = 1;
					nbBlackPawn++;
				}
				break;
			case 11:
				if (nbBlackQueen == 0) {
					index = 171;
					input[index] = 1;
					for (int k = 0; k < 3; k++) {
						input[index + 1 + k] = (ligne >> k) & 1;
						input[index + 4 + k] = (colonne >> k) & 1;
					}
					nbBlackQueen++;
				}
				else {
					index = 192 + 10 * nbBlackPawn;
					input[index] = 1;
					for (int k = 0; k < 3; k++) {
						input[index + 1 + k] = (ligne >> k) & 1;
						input[index + 4 + k] = (colonne >> k) & 1;
					}
					input[index + 7] = 1;
					nbBlackPawn++;
				}
				break;
			case 12:

				index = 178;
				input[index] = 1;
				for (int k = 0; k < 3; k++) {
					input[index + 1 + k] = (ligne >> k) & 1;
					input[index + 4 + k] = (colonne >> k) & 1;
				}
				break;
			}
		}
	}

	
	//bool trait
	input[272] = trait;

	//bool roques
	input[273] = roques[0];
	input[274] = roques[1];
	input[275] = roques[2];
	input[276] = roques[3];

	//bools en passant
	input[275] = enPassantOk;
	for (int k = 0; k < 3; k++) {
		input[276 + k] = (enPassant >> k) & 1;
	}


	//règles des 50 coups
	for (int k = 0; k < 6; k++) {
		input[279 + k] = (fiftyMove >> k) & 1;
	}
	
	return input;
}

float ChessBoard::evaluateMove(Network network, int move)
{
	if (move > nbMoves) {
		cout << "PROBLEM : EVALUATION OF A NON EXISTANT MOVE";
		return 0.0f;
	}
	uint_fast8_t colonneDepart = legalMoves[move];
	uint_fast8_t ligneDepart = legalMoves[move + 1];
	uint_fast8_t colonneArrivee = legalMoves[move + 2];
	uint_fast8_t ligneArrivee = legalMoves[move + 3];
	uint_fast8_t piece = chessBoard[ligneDepart][colonneDepart];
	uint_fast8_t pieceArrivee = chessBoard[ligneArrivee][colonneArrivee];

	bool roquesState[4] = { roques[0], roques[1], roques[2], roques[3] };
	int fiftyMoveState = fiftyMove;
	bool enPassantOkState = enPassantOk;
	uint_fast8_t enPassantState = enPassant;

	float result = 0.f;

	fiftyMove++;
	roquesCheck(colonneDepart, ligneDepart);

	//Play Roque :
	if (colonneDepart == 4 && ligneDepart == 0) {
		if (colonneArrivee == 6 && ligneArrivee == 0 && piece == 6) {
			//Petit Roque
			swapPieces(5, 0, 7, 0);
		}
		if (colonneArrivee == 2 && ligneArrivee == 0 && piece == 6) {
			//Grand Roque
			swapPieces(2, 0, 0, 0);
		}
	}
	if (colonneDepart == 4 && ligneDepart == 7) {
		if (colonneArrivee == 6 && ligneArrivee == 7 && piece == 12) {
			//Petit Roque
			swapPieces(5, 7, 7, 7);
		}
		if (colonneArrivee == 2 && ligneArrivee == 7 && piece == 12) {
			//Grand Roque
			swapPieces(3, 7, 0, 7);
		}
	}


	enPassantOk = false;
	//Pawn move
	if (piece == 1 || piece == 7) {
		//Register EnPassant
		if (ligneDepart + 2 == ligneArrivee || ligneDepart - 2 == ligneArrivee) {
			enPassantOk = true;
			enPassant = colonneDepart;
		}
		//Eat pawn EnPassant
		if ((colonneDepart == colonneArrivee + 1 || colonneDepart == colonneArrivee - 1) && chessBoard[ligneArrivee][colonneArrivee] == 0) {
			chessBoard[ligneDepart][colonneArrivee] = 0;
		}
		//Promotion (Queen for now)
		if (ligneArrivee == 7) {
			chessBoard[ligneDepart][colonneDepart] = 5;
		}
		if (ligneArrivee == 0) {
			chessBoard[ligneDepart][colonneDepart] = 11;
		}
	}

	if (piece == 0 || chessBoard[ligneArrivee][colonneArrivee]) {
		fiftyMove = 0;
	}
	swapPieces(colonneDepart, ligneDepart, colonneArrivee, ligneArrivee);
	chessBoard[ligneDepart][colonneDepart] = 0;

	trait = !trait;

	network.input(chessBoardToVectorInput());

	result = network.calculateOutput();
	


	//Return to before move state
	trait = !trait;
	fiftyMove = fiftyMoveState;
	enPassant = enPassantState;
	enPassantOk = enPassantOkState;
	for (int i = 0; i < 4; i++) {
		roques[i] = roquesState[i];
	}
	chessBoard[ligneDepart][colonneDepart] = piece;
	chessBoard[ligneArrivee][colonneArrivee] = pieceArrivee;
	
	return result;
}

void ChessBoard::playBestMove(Network network)
{

	
	int bestMove = 0;
	float bestValue = 0.f;
	float evaluation;
	for (int i = 0; i < nbMoves; i++) {
		evaluation = evaluateMove(network, i);
		if (bestValue < evaluation) {
			bestMove = i;
			bestValue = evaluation;
		}
	}

	playMove(bestMove);

}

int ChessBoard::matchNetwork(Network *n1, Network *n2) {
	restart();
	int score = 0;
	//Match 1 : n1 plays white, n2 plays black
	while (!gameEnd) {
		if (trait) {
			playBestMove(*n1);
		}
		else {
			playBestMove(*n2);
		}
	}
	if (win) {
		if (whiteWin) score++;
		else score--;
	}
	restart();
	//Match 2 : n2 plays white, n1 plays black
	while (!gameEnd) {
		if (trait) playBestMove(*n2);
		else playBestMove(*n1);
	}
	
	if (win) {
		if (whiteWin) score--;
		else score++;
	}
	(* n1).fitness += score;
	(* n2).fitness -= score;
	restart();
	return score;
	
}