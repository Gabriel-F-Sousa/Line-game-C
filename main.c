/*******************************************************************************************
*
*   
*
*   Copyright (c) 2023-2023 Gabriel S.
*
********************************************************************************************/

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define X_GRID_SIZE 10
#define Y_GRID_SIZE 10
#define GRID_LENGTH 10
#define GRID_DISTANCE 32

typedef struct box{
    //UP, RIGHT, DOWN, LEFT
    int used[4];
    int owner[4];
    int win;
} dot;

int createGrid(int, int, int, int);
int get_index_from_pos(int, int, int);
int * get_pos_from_index(int, int);
int * mouse_pos_to_grid(int, int);
int * grid_to_pos(int, int, int);
bool  is_mouse_in_grid(int, int, int);
int check_clicked_side(int, int, int, int, int);
void create_line(int, int, int, int, int);
int* get_used_sides(int, int, int, struct box array[]);
bool set_dot_side(int, int, int, struct box array[], int);
bool checkClosedBox(int, int, int, struct box array[], int, int);
void update_text(int, int, int);
void update_lines(struct box array[], int);
void restart_game(struct box array[], int, int*, int*, int*);
void end_screen();




//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
const int screenWidth = 800;//800
const int screenHeight = 450;//450


int currentPlayer = 1;
int player_1_score = 0;
int player_2_score = 0;


int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    //const int screenWidth = 800;
    //const int screenHeight = 450;
    
    //make the array grid
    int arraySize = (GRID_LENGTH) * (GRID_LENGTH);
    dot *array;
    
    array = malloc(sizeof(dot) * arraySize);
    
    for (int i = 0; i < arraySize; i++){
        array[i].used[0] = 0;//up
        array[i].used[1] = 0;//right
        array[i].used[2] = 0;//down
        array[i].used[3] = 0;//left
        
        array[i].owner[0] = 0;//up
        array[i].owner[1] = 0;//right
        array[i].owner[2] = 0;//down
        array[i].owner[3] = 0;//left
        array[i].win = 0;//left
    }
    
    Vector2 mousePoint = { 0.0f, 0.0f };
    
    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - basic shapes drawing");

    float rotation = 0.0f;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        rotation += 0.2f;
        mousePoint = GetMousePosition();
        Vector2 mouseOffsetPos;
        mouseOffsetPos.x = GetMousePosition().x - (screenWidth/5);
        mouseOffsetPos.y = GetMousePosition().y - (screenHeight/5);
        
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        
        BeginDrawing();
            ClearBackground(GRAY);

            //DrawText("some basic shapes available on raylib", 20, 20, 20, RED);

            // Circle shapes and lines
            //DrawCircle(screenWidth/5, 120, GRID_DISTANCE, DARKBLUE);
            
            createGrid(X_GRID_SIZE, Y_GRID_SIZE, screenHeight, screenWidth);
            //DrawText(TextFormat("true mouse pos:(%f, %f)", mouseOffsetPos.x, mouseOffsetPos.y), 10, 50, 25, BLACK);
            
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                int *pos;
                pos = mouse_pos_to_grid(mouseOffsetPos.x , mouseOffsetPos.y);
                
                DrawText(TextFormat("(%d, %d)", pos[0], pos[1]), mousePoint.x, mousePoint.y - 20, 5, BLACK);
                
                if (is_mouse_in_grid(mouseOffsetPos.x, mouseOffsetPos.y, GRID_LENGTH)){
                    int side = check_clicked_side(mouseOffsetPos.x, mouseOffsetPos.y, mousePoint.x, mousePoint.y, arraySize);
                    if (side != 4){
                        //make the changes to grid and all that
                        int *mouse_to_grid;
                        mouse_to_grid = mouse_pos_to_grid(mouseOffsetPos.x, mouseOffsetPos.y);
                        //printf("MOUSE_TO_GRID: %d %d \n", mouse_to_grid[0], mouse_to_grid[1]);
                        int *used_sides;
                        used_sides = get_used_sides(mouse_to_grid[0], mouse_to_grid[1], GRID_LENGTH, array);
                        
                        if (used_sides[side] == 0){
                           //(used_sides[side] == 0) ? printf("can send \n") : printf("can't iasdu gfjhadfsighu\n");
                            
                            if (set_dot_side(mouse_to_grid[0], mouse_to_grid[1], GRID_LENGTH, array, side)){
                                //printf("ADD LINE TO: %d %d, side %d\n", mouse_to_grid[0], mouse_to_grid[1], side);
                                
                                if (checkClosedBox(mouse_to_grid[0], mouse_to_grid[1], GRID_LENGTH, array, side, currentPlayer) == false ){
                                    (currentPlayer == 1) ? (currentPlayer = 2) : (currentPlayer = 1);
                                }
                                else{
                                    //printf("DON'T change dudes \n");
                                }
                            }
                        }
                        //create_line(mouseOffsetPos.x, mouseOffsetPos.y, side, GRID_LENGTH);
                    }
                }
            }
            
            if (GuiButton((Rectangle){ 600, 50, 125, 30 }, "#75# RESET")) {
                restart_game(array, arraySize, &player_1_score, &player_2_score, &currentPlayer);
            }
            
            if (IsKeyPressed(KEY_SPACE)){
                restart_game(array, arraySize, &player_1_score, &player_2_score, &currentPlayer);
                printf("SPACE: %d\n", 10123);
            }
            
            update_text(player_1_score, player_2_score, currentPlayer);
            update_lines(array, arraySize);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    free(array); 
    return 0;
}


int createGrid(int grid_size_x, int grid_size_y, int screenHeight, int screenWidth){
    for (int x = 0; x < GRID_LENGTH; x++){
        for (int y = 0; y < GRID_LENGTH; y++){
            DrawCircle(x*GRID_DISTANCE + (screenWidth/5), y*GRID_DISTANCE + (screenHeight / 5), 5, RED);
            
            //DrawCircle(x*GRID_DISTANCE, y*GRID_DISTANCE, 5, RED);
            // DEBUG text 
            int index = x + GRID_LENGTH*y;
            DrawText(TextFormat("(%d %d), \n%d", x ,y, index), x*GRID_DISTANCE + (screenWidth/5), y*GRID_DISTANCE + (screenHeight / 5), 5, BLACK);
        }
    }
    return 0;
}

int get_index_from_pos(int pos_x, int pos_y, int length){
    int index = pos_x + length * pos_y;
    return index;
}

int * get_pos_from_index(int index, int length){
    int x = index % length;
    int y = index / length;
    static int pos[2];
    pos[0] = x;
    pos[1] = y;
    return  pos;
}

int * mouse_pos_to_grid(int mouse_x, int mouse_y){
    int x = mouse_x / GRID_DISTANCE;
    int y = mouse_y / GRID_DISTANCE;
    static int pos[2];
    pos[0] = x;
    pos[1] = y;
    return pos;
}

int * grid_to_pos(int mouse_pos_x, int mouse_pos_y, int length){
    int x = (mouse_pos_x * GRID_DISTANCE) + (screenWidth/5);
    int y = (mouse_pos_y * GRID_DISTANCE) + (screenHeight/5);
    static int pos[2];
    pos[0] = x;
    pos[1] = y;
    return pos;
}

bool is_mouse_in_grid(int mouse_pos_x, int mouse_pos_y, int length){
    //mouse_pos_x as offset
    int *mouse_grid_pos;
    
    mouse_grid_pos = mouse_pos_to_grid(mouse_pos_x, mouse_pos_y);
    
    if (mouse_grid_pos[0] >= 0 && mouse_grid_pos[0] <= length-2 && mouse_grid_pos[1] >= 0 && mouse_grid_pos[1] <= length-2){
        DrawText(TextFormat("x is in(%d, %d)", mouse_grid_pos[0], mouse_grid_pos[1]), 10, 30, 5, BLACK);
        return true;
    }
    else{
        return false;
    }
    //DrawText(TextFormat("mouse grid pos(%d, %d)", mouse_grid_pos[0], mouse_grid_pos[1]), 10, 10, 24, RED);
}

int check_clicked_side(int mouse_pos_x, int mouse_pos_y, int global_mouse_x, int global_mouse_y, int length){
    //offset mouse
    //DrawText(TextFormat("iuahsdf(%d, %d)", abs(mouse_pos_x), mouse_pos_y), 10, 0, 24, RED);
    
    int *mouse_grid_pos;
    mouse_grid_pos = mouse_pos_to_grid(mouse_pos_x, mouse_pos_y);
    
    int index = get_index_from_pos(mouse_grid_pos[0], mouse_grid_pos[1], length);
    
    int *pos_from_index;
    pos_from_index = get_pos_from_index(index, length);
    
    int *grid_global_pos;
    grid_global_pos = grid_to_pos(pos_from_index[0], pos_from_index[1], length);
    
    /*stuff i used to debug
    DrawCircle(grid_global_pos[0], grid_global_pos[1], 5, BLUE);
    DrawText(TextFormat("index(%d, %d)", pos_from_index[0], pos_from_index[1]), 10, 0, 24, RED);
    DrawText(TextFormat("grid_global_pos(%d, %d)", grid_global_pos[0], grid_global_pos[1]), 10, 24, 24, RED);
    DrawText(TextFormat("abs(%d, %d)", abs(mouse_pos_x - grid_global_pos[0]), abs(mouse_pos_y - grid_global_pos[1])), 300, 24, 24, RED);
    DrawText(TextFormat("mouse_pos: (%d, %d)", mouse_pos_x, mouse_pos_y), 300, 0, 24, RED);
    */
    
    int corner_distance_x;
    int corner_distance_y;
    corner_distance_x = abs(grid_global_pos[0] - global_mouse_x);
    corner_distance_y = abs(grid_global_pos[1] - global_mouse_y);
    
    
    if (corner_distance_x < 10 && corner_distance_y > 5 && corner_distance_y < 25){
        DrawText("LEFT", 700, 250, 24, RED);
        return 3;
    }
    else if (corner_distance_x > 20 && corner_distance_y > 5 && corner_distance_y < 25){
        DrawText("RIGHT", 700, 250, 24, RED);
        return 1;
    }
    else if (corner_distance_y < 10 && corner_distance_x > 10 && corner_distance_x < 25){
        DrawText("UP", 700, 250, 24, RED);
        return 0;
    }
    else if (corner_distance_y > 20 && corner_distance_x > 10 && corner_distance_x < 25){
        DrawText("DOWN", 700, 250, 24, RED);
        return 2;
    }
    return 4;
}

void create_line(int mouse_pos_x, int mouse_pos_y, int side, int length, int player){
    /*
    int *mouse_grid_pos;
    mouse_grid_pos = mouse_pos_to_grid(mouse_pos_x, mouse_pos_y);
    */
    int mouse_grid_pos[2];
    mouse_grid_pos[0] = mouse_pos_x;
    mouse_grid_pos[1] = mouse_pos_y;
    
    int index = get_index_from_pos(mouse_grid_pos[0], mouse_grid_pos[1], length);
    
    int *pos_from_index;
    pos_from_index = get_pos_from_index(index, length);
    
    int *grid_global_pos;
    grid_global_pos = grid_to_pos(pos_from_index[0], pos_from_index[1], length);
    
    
    Vector2 startPos = {grid_global_pos[0], grid_global_pos[1]};
    Vector2 endPos = {0, 0};

    
    switch(side){
        case 0:
        endPos.x = grid_global_pos[0] + GRID_DISTANCE;
        endPos.y = grid_global_pos[1];
        break;
        
        case 1:
        startPos.x = grid_global_pos[0] + GRID_DISTANCE;
        startPos.y = grid_global_pos[1];
        
        endPos.x = grid_global_pos[0] + GRID_DISTANCE;
        endPos.y = grid_global_pos[1] + GRID_DISTANCE;
        break;
        
        case 2:
        startPos.x = grid_global_pos[0];
        startPos.y = grid_global_pos[1] + GRID_DISTANCE;
        
        endPos.x = grid_global_pos[0] + GRID_DISTANCE;
        endPos.y = grid_global_pos[1] + GRID_DISTANCE;
        break;
        
        case 3:
        endPos.x = grid_global_pos[0];
        endPos.y = grid_global_pos[1] + GRID_DISTANCE;
        break;
    }
    
    /*if 
    if (side == 0){
        endPos.x = grid_global_pos[0] + GRID_DISTANCE;
        endPos.y = grid_global_pos[1];
    }
    else if (side == 1){
        startPos.x = grid_global_pos[0] + GRID_DISTANCE;
        startPos.y = grid_global_pos[1];
        
        endPos.x = grid_global_pos[0] + GRID_DISTANCE;
        endPos.y = grid_global_pos[1] + GRID_DISTANCE;
    }
    else if (side == 2){
        startPos.x = grid_global_pos[0];
        startPos.y = grid_global_pos[1] + GRID_DISTANCE;
        
        endPos.x = grid_global_pos[0] + GRID_DISTANCE;
        endPos.y = grid_global_pos[1] + GRID_DISTANCE;
    }
    else if (side == 3){        
        endPos.x = grid_global_pos[0];
        endPos.y = grid_global_pos[1] + GRID_DISTANCE;
    }
    */
    if (player == 1){
        //DrawLineEx(startPos, endPos, 5, BLUE);
        DrawLineEx(startPos, endPos, 5, DARKBLUE);
    }
    else{
        //DrawLineEx(startPos, endPos, 5, GREEN);
        DrawLineEx(startPos, endPos, 5, LIME);
    }
}

int* get_used_sides(int position_x, int position_y, int length, struct box array[]){
    static int used_arr[4];
    int index = get_index_from_pos(position_x, position_y, length);
    for (int i = 0; i < 4; i++){
        used_arr[i] = array[index].used[i];
    }
    return used_arr;
}

bool set_dot_side(int position_x, int position_y, int length, struct box array[], int side){
    int index = get_index_from_pos(position_x, position_y, length);
    if (array[index].used[side] == 1){
        //printf("already used");
        return false;
    }
    
    int otherIndex = -1;
    int otherSide = -1;
    int hasOther = false;
    
    //printf("x+1: %d, %d\n", position_x +1, length);
    //change the other side
    int other_x = 0;
    int other_y = 0;
    
    switch(side){
        case 0:
        //up
        other_x = 0;
        other_y = -1;
        otherSide = 2;
        break;
        
        case 1:
        //right
        other_x = 1;
        other_y = 0;
        otherSide = 3;
        break;
        
        case 2:
        //down
        other_x = 0;
        other_y = 1;
        otherSide = 0;
        break;
        
        case 3:
        //left
        other_x = -1;
        other_y = 0;
        otherSide = 1;
        break;        
    }
    
    if (position_x + other_x < length-1 && position_x + other_x >= 0 && position_y + other_y < length-1 && position_y + other_y >= 0){
        otherIndex = get_index_from_pos(position_x + other_x, position_y + other_y, length);
        hasOther = true;
        //printf("can change  (%d, %d) -> (%d, %d)\n", position_x, position_y, position_x + other_x, position_y + other_y);
        //printf("side: %d, otherSide: %d \n", side, otherSide);
    }
    
    
    if (array[index].used[side] == 0 && hasOther == false){
        array[index].used[side] = 1;
        array[index].owner[side] = currentPlayer;
        //printf("change one side ONLY\n");
        return true;
    }
    
    else if (array[index].used[side] == 0 && hasOther == true && array[otherIndex].used[otherSide] == 0){
        array[index].used[side] = 1;
        array[index].owner[side] = currentPlayer;
        
        array[otherIndex].used[otherSide] = 1;
        array[otherIndex].owner[otherSide] = currentPlayer;
        //printf("change BOTH sides \n");
        return true;
    }
    
    
    else{
        return false;
    }
}

bool checkClosedBox(int position_x, int position_y, int length, struct box array[], int side, int player){
    int index = get_index_from_pos(position_x, position_y, length);
    
    int otherIndex = -1;
    
    int hasSquare_1 = true;
    int hasSquare_2 = true;
    
    //printf("x+1: %d, %d\n", position_x +1, length);
    //change the other side
    int other_x = 0;
    int other_y = 0;
    
    switch(side){
        case 0:
        //up
        other_x = 0;
        other_y = -1;
        break;
        
        case 1:
        //right
        other_x = 1;
        other_y = 0;
        break;
        
        case 2:
        //down
        other_x = 0;
        other_y = 1;
        break;
        
        case 3:
        //left
        other_x = -1;
        other_y = 0;
        break;        
    }
    
    int *used_sides;
    used_sides = get_used_sides(position_x, position_y, GRID_LENGTH, array);
    for (int i = 0; i <= 4; i++){
        if (used_sides[i] == 0){
            hasSquare_1 = false;
            break;
        }
    }
    
    if (position_x + other_x < length-1 && position_x + other_x >= 0 && position_y + other_y < length-1 && position_y + other_y >= 0){
        otherIndex = get_index_from_pos(position_x + other_x, position_y + other_y, length);
        int *other_used_sides;
        other_used_sides = get_used_sides(position_x + other_x, position_y + other_y, GRID_LENGTH, array);
        for (int i = 0; i <= 4; i++){
            if (other_used_sides[i] == 0){
                hasSquare_2 = false;
                break;
            }
        }
    }
    else{
        hasSquare_2 = false;
    }
    
    //(hasSquare_1 == true) ? create_square(position_x, position_y, length, currentPlayer) : (0);
    //(hasSquare_2 == true) ? create_square(position_x, position_y, length, currentPlayer) : (0);
    
    (hasSquare_1 == true) ? (array[index].win = player) : (0);
    (hasSquare_2 == true) ? (array[otherIndex].win = player) : (0);
    
    if (player == 1){
        player_1_score += hasSquare_1;
        player_1_score += hasSquare_2;
    }
    else{
        player_2_score += hasSquare_1;
        player_2_score += hasSquare_2;
    }
    
    if (hasSquare_1 == true || hasSquare_2 == true){
        return true;
    }
    else{
        return false;
    }
}

void create_square(int position_x, int position_y, int length, int player){
    int *GRID2POS;
    GRID2POS = grid_to_pos(position_x, position_y, length);
    (player == 1) ? DrawRectangle(GRID2POS[0] + 2, GRID2POS[1] + 3, GRID_DISTANCE - 5, GRID_DISTANCE - 5, BLUE) : DrawRectangle(GRID2POS[0] + 2, GRID2POS[1] + 3, GRID_DISTANCE - 5, GRID_DISTANCE - 5, GREEN);
}

void update_text(int player_1_s, int player_2_s, int current_player){
    if (current_player == 1){
        DrawText(TextFormat("Player 1: %d", player_1_s), 10, 10, 30, GREEN);
        DrawText(TextFormat("Player 2: %d", player_2_s), 10, 35, 30, BLACK);
    }
    else{
        DrawText(TextFormat("Player 1: %d", player_1_s), 10, 10, 30, BLACK);
        DrawText(TextFormat("Player 2: %d", player_2_s), 10, 35, 30, GREEN);
    }
}

void update_lines(struct box array[], int arraySize){
    for (int i = 0; i < arraySize; i++){
        int *pos_from_index;
        pos_from_index = get_pos_from_index(i, GRID_LENGTH);
        for (int u = 0; u < 4; u++){
            if (array[i].used[u] != 0){
                int player = array[i].owner[u];
                create_line(pos_from_index[0], pos_from_index[1], u, GRID_LENGTH, player);
            }
        }
        int hasSquare = true;
        int *used_sides;
        used_sides = get_used_sides(pos_from_index [0], pos_from_index[1], GRID_LENGTH, array);
        for (int h = 0; h <= 4; h++){
            if (used_sides[h] == 0){
                hasSquare = false;
                break;
            }
        }
        (hasSquare) ? (create_square(pos_from_index[0], pos_from_index[1], GRID_LENGTH, array[i].win)) : false;
    }
}

void restart_game(struct box array[], int arraySize, int *score_1, int *score_2, int *curPlayer){
    *score_1 = 0;
    *score_2 = 0;
    *curPlayer = 1;
    
    for (int i = 0; i < arraySize; i++){
        array[i].used[0] = 0;//up
        array[i].used[1] = 0;//right
        array[i].used[2] = 0;//down
        array[i].used[3] = 0;//left
        
        array[i].owner[0] = 0;//up
        array[i].owner[1] = 0;//right
        array[i].owner[2] = 0;//down
        array[i].owner[3] = 0;//left
        array[i].win = 0;//left
    }
}

void end_screen(){
    DrawRectangle(0, 0, 10000, 10000, BLACK);
    if (player_1_score > player_2_score)
        DrawText("player 1 WINS", screenHeight / 2, screenWidth / 4, 52, GREEN);
    else
        DrawText("player 2 WINS", screenHeight / 2, screenWidth / 4, 52, BLUE);
}






