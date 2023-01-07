#include "raylib.h"
#include "ChessBoard.h"
int main() {
    const int screenWidth = 750;
    const int screenHeight = 500;
    
    ChessBoard chessBoard;



    
    InitWindow(screenWidth, screenHeight, "Echec Critique");

    SetWindowIcon(LoadImage(ASSETS_PATH"black_king.png"));
    Texture2D piecesTexture[12] = {LoadTexture(ASSETS_PATH"white_pawn.png"), LoadTexture(ASSETS_PATH"white_knight.png"),LoadTexture(ASSETS_PATH"white_bishop.png"), LoadTexture(ASSETS_PATH"white_rook.png"), LoadTexture(ASSETS_PATH"white_queen.png"),LoadTexture(ASSETS_PATH"white_king.png"), LoadTexture(ASSETS_PATH"black_pawn.png"), LoadTexture(ASSETS_PATH"black_knight.png"),  LoadTexture(ASSETS_PATH"black_bishop.png"), LoadTexture(ASSETS_PATH"black_rook.png"), LoadTexture(ASSETS_PATH"black_queen.png"),LoadTexture(ASSETS_PATH"black_king.png")};

    chessBoard.loadTexture(piecesTexture);
    Rectangle restartRec = { 500, 350, 200, 75 };
    Rectangle trait = { 500, 50, 50, 50 };
    Color restartButtonColor = DARKBLUE;

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

        if (chessBoard.getTrait()) {
            DrawRectangleRec(trait, WHITE);
        }
        else {
            DrawRectangleRec(trait, BLACK);
        }

        if (CheckCollisionPointRec(GetMousePosition(), restartRec)) {
            restartButtonColor = SKYBLUE;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                chessBoard.restart();
            }
        }
        else {
            restartButtonColor = DARKBLUE;
        }
        DrawRectangleRec(restartRec, restartButtonColor);
        DrawText("Restart", restartRec.x + 20, restartRec.y + 20, 40, WHITE);

        ClearBackground(BLUE);

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