#include "raylib.h"

int main(void)
{
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	InitWindow(screenWidth, screenHeight, "FallRocks");

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
