#include "raylib.h"
#include "ChessBoard.h"
#include <chrono>
#include <thread>
using namespace std;


int main() {
    const int screenWidth = 750;
    const int screenHeight = 500;
    
    ChessBoard chessBoard;

    bool aiPlayWhite = false;
    bool aiPlayBlack = false;
    
    vector<int> networkDimensions = { 285,143, 1 };
    int nbOfNetworks = 512;
    //vector<Network> networks = vector<Network>(20,*new Network(networkDimensions));
    int count = nbOfNetworks;
    Network father = Network("NN/father.txt");
    Network mother = Network("NN/mother.txt");
    /*vector<vector<Network>> bracket;
    vector<vector<int>> bracketID;
    while (count >= 1) {
        
        bracket.push_back(* (new vector<Network>(count, *new Network(networkDimensions))));
        bracketID.push_back(*(new vector<int>(count)));
        count = count / 2;
    }
    for (int i = 0; i < nbOfNetworks; i++) {
        bracket[0][i] = father.reproduction(mother);
        bracketID[0][i] = i;
    }

    for (int gen = 0; gen < 100; gen++) {
        for (int i = 0; i < bracket.size() - 1; i++) {
            if (bracket[i].size() == 8) {
                //TOP 8 creation of new generation
                int countRNG = 0;
                for (int j = 0; j < nbOfNetworks; j++) {
                    if (countRNG < nbOfNetworks / 3) {
                        if (rand() % 3 == 0) {
                            countRNG++;
                            bracket[0][j].randomWeights();
                        }
                        else
                        bracket[0][j] = bracket[i][rand() % (int)(8)].reproduction(bracket[i][rand() % (int)(8)]);
                    }
                    else {
                        bracket[0][j] = bracket[i][rand() % (int)(8)].reproduction(bracket[i][rand() % (int)(8)]);
                    }
                }
            }
            if (bracket[i].size() == 2) {
                bracket[i][0].saveToFile("NN/father.txt");
                bracket[i][1].saveToFile("NN/mother.txt");
            }
            for (int j = 0; j < bracket[i].size(); j += 2) {
                cout << "gen : " << gen << "; match : " << i << "; " << bracketID[i][j] << " " << bracketID[i][j + 1] << '\n';
                int result = chessBoard.matchNetwork(&bracket[i][j], &bracket[i][j + 1]);
                
                if (result > 0) {
                    bracket[i + 1][j / 2] = bracket[i][j];
                    bracketID[i + 1][j / 2] = bracketID[i][j];
                }
                else {
                    if (result < 0) {
                        bracket[i + 1][j / 2] = bracket[i][j + 1];
                        bracketID[i + 1][j / 2] = bracketID[i][j + 1];
                    }
                    else {
                        int r = rand() % 2;
                        bracket[i + 1][j / 2] = bracket[i][j + r];
                        bracketID[i + 1][j / 2] = bracketID[i][j + r];
                    }
                }
            }
        }
    }
    /*
    int indiceFather = 0;
    int indiceMother = 0;
    int fitnessFirst= 0;
    int fitnessSecond = 0;
    
    for (int i = 0; i < 15; i++) {
        networks[i] = father.reproduction(mother);
    }
    for (int i = 15; i < 20; i++) {
        networks[i].randomWeights();
    }
    for (int gen = 0; gen < 30; gen++) {
        cout << "NewGeneration : "<< gen << "\n";
        cout << "startMatchs\n";

        for (int i = 0; i < 19; i++) {
            for (int j = i + 1; j < 20; j++) {
                chessBoard.matchNetwork(&networks[i], &networks[j]);
                cout << i << " vs " << j << "\n";
            }
            cout << "matches of " << i << " has been done. Fitness ="<< networks[i].fitness <<"\n";
        }
        cout << "endMatchs\n";

        cout << "FITNESS :\n";
        for (int i = 0; i < 20; i++) {
            int fit = networks[i].fitness;
            cout << i << " : " << networks[i].fitness << "\n";
            if (fit > fitnessFirst) {
                indiceMother = indiceFather;
                indiceFather = i;
                fitnessSecond = fitnessFirst;
                fitnessFirst = fit;
            }
            else {
                if (fit > fitnessSecond) {
                    indiceMother = i;
                    fitnessSecond = fit;
                }
            }
        }
        father = networks[indiceFather];
        mother = networks[indiceMother];
        father.saveToFile("NN/father.txt");
        mother.saveToFile("NN/mother.txt");
        cout << "Creation of new generation \n";
        for (int i = 0; i < 15; i++) {
            networks[i] = father.reproduction(mother);
        }
        for (int i = 15; i < 20; i++) {
            networks[i] = *new Network(networkDimensions);
            networks[i].randomWeights();
        }
    }
    
    
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; i++) {
        chessBoard.evaluateMove(networks[0],0);
    }
    auto end = chrono::high_resolution_clock::now();
    cout << "Time : " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "µs\n";
    */
    
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

        if (CheckCollisionPointRec(GetMousePosition(), restartRec)) { //RESTART BUTTON
            restartButtonColor = SKYBLUE;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                chessBoard.restart();
            }
        }
        else {
            restartButtonColor = DARKBLUE;
        }
       



        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_R)) {
            
            aiPlayWhite = !aiPlayWhite;
            cout << "AI plays white : " << aiPlayWhite << "\n";
        }
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_T)) {
            aiPlayBlack = !aiPlayBlack;
            cout << "AI plays black : " << aiPlayBlack << "\n";
        }
        //AI plays
        if ((aiPlayWhite && chessBoard.getTrait()) || (aiPlayBlack && !chessBoard.getTrait())) {
            
            if (chessBoard.getTrait()) {
                chessBoard.playBestMove(father);
            }
            else {
                chessBoard.randomPlayMove();
            }
        }
       




        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        if (chessBoard.getTrait()) {
            DrawRectangleRec(trait, WHITE);
        }
        else {
            DrawRectangleRec(trait, BLACK);
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