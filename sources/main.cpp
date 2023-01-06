#include "raylib.h"
#include "ChessBoard.h"
int main() {
    const int screenWidth = 500;
    const int screenHeight = 500;
    
    ChessBoard chessBoard;



    
    InitWindow(screenWidth, screenHeight, "Echec Critique");
    Texture2D piecesTexture[12] = {LoadTexture(ASSETS_PATH"white_pawn.png"), LoadTexture(ASSETS_PATH"white_knight.png"),LoadTexture(ASSETS_PATH"white_bishop.png"), LoadTexture(ASSETS_PATH"white_rook.png"), LoadTexture(ASSETS_PATH"white_queen.png"),LoadTexture(ASSETS_PATH"white_king.png"), LoadTexture(ASSETS_PATH"black_pawn.png"), LoadTexture(ASSETS_PATH"black_knight.png"),  LoadTexture(ASSETS_PATH"black_bishop.png"), LoadTexture(ASSETS_PATH"black_rook.png"), LoadTexture(ASSETS_PATH"black_queen.png"),LoadTexture(ASSETS_PATH"black_king.png")};

    chessBoard.loadTexture(piecesTexture);

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        chessBoard.display();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    return 0;
    
}