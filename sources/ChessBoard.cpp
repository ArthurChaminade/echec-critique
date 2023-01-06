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
	for (int ligne = 0; ligne < 8; ligne++) {
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
	if (isPieceSelected) {
		for (int i = 0; i < nbMoves; i++) {
			if (legalMoves[i*4] == selectedTile[0] && legalMoves[i*4 + 1] == selectedTile[1]) {
				DrawCircle(boardDisplayPosition.x + (0.5f + legalMoves[i*4 +2]) * tileSize.x, boardDisplayPosition.y + (legalMoves[i*4+3] + 0.5f) * (tileSize.y), tileSize.x / 5.f, GRAY);
			}
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
				isPieceSelected = true;
				pieceSelected[0] = mouseOnTile[0];
				pieceSelected[1] = mouseOnTile[1];
			}
			else {
				isPieceSelected = false;
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
}

void ChessBoard::addAllMoves()
{ //TODO : optimiser (brute atm)
	nbMoves = 0;
	fill(begin(legalMoves), end(legalMoves), 0);
	int ptrLegalMoves = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			for (int i2 = 0; i2 < 8; i2++) {
				for (int j2 = 0; j2 < 8; j2++) {
					if (isMoveLegal(j, i, j2, i2)) {
						addMove(j, i, j2, i2, &ptrLegalMoves);
						nbMoves++;
					}
				}
			}
		}
	}
}

bool ChessBoard::isKingAttacked(bool color) {
	uint_fast8_t king;
	uint_fast8_t kingLigne;
	uint_fast8_t kingColonne;
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


	uint_fast8_t piece = chessBoard[ligneDepart][colonneDepart];
	uint_fast8_t pieceArrivee = chessBoard[ligneArrivee][colonneArrivee];

	bool b;
	bool classic;
	bool roque;
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
		return ((classic) && (pieceArrivee == 0 || pieceArrivee == 6));
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
			swapPieces(5, 7, 2, 7);
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
	bool legal = piece > 0 && piece <= 12;
	legal &= trait ^ (piece >= 7); //Le joueur joue une pièce à lui
	legal &= isMoveCorrect(colonneDepart, ligneDepart, colonneArrivee, ligneArrivee);
	swapPieces(colonneDepart, ligneDepart, colonneArrivee, ligneArrivee);
	legal &= !isKingAttacked(trait);
	swapPieces(colonneDepart, ligneDepart, colonneArrivee, ligneArrivee);
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
	for (int i = 0; i < nbMoves; i++) {
		if (legalMoves[i * 4] == colonneDepart && legalMoves[i * 4 + 1] == ligneDepart && legalMoves[i * 4 + 2] == colonneArrivee && legalMoves[i * 4 + 3] == ligneArrivee) {
			
			roquesCheck(colonneDepart, ligneDepart);

			if (colonneDepart == 4 && ligneDepart == 0) {
				if (colonneArrivee == 6 && ligneArrivee == 0 && chessBoard[ligneDepart][colonneDepart] == 6) {
					//Petit Roque
					swapPieces(5, 0, 7, 0);
				}
				if (colonneArrivee == 2 && ligneArrivee == 0 && chessBoard[ligneDepart][colonneDepart] == 6) {
					//Grand Roque
					swapPieces(2, 0, 0, 0);
				}
			}
			swapPieces(colonneDepart, ligneDepart, colonneArrivee, ligneArrivee);
			chessBoard[ligneDepart][colonneDepart] = 0;
			trait = !trait;
			addAllMoves();
			printAllMoves();
			return true;
		}
	}
	return false;
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
		cout << "roquesCheck ligne depart 0 : colonne depart = " << (int)colonneDepart << "\n";
		cout << "roques[0] : " << roques[0] << "\n";
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

void ChessBoard::start()
{
	
}
