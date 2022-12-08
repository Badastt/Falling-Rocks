#include "raylib.h"
#include <stdlib.h>

int main(void)
{
	int screenWidth = GetMonitorWidth(GetCurrentMonitor());
	int screenHeight = GetMonitorHeight(GetCurrentMonitor());
	InitWindow(screenWidth, screenHeight, "FallRocks");

	MaximizeWindow();

	int cor = 0;

	Vector2 rectanglePosition = { GetScreenWidth()/2.f, GetScreenHeight()/2.f };
	Vector2 size = {50, 100};
	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
	if (IsKeyDown(KEY_RIGHT)) rectanglePosition.x += 4.0f;
	if (IsKeyDown(KEY_LEFT)) rectanglePosition.x -= 4.0f;
	if (IsKeyDown(KEY_UP)) rectanglePosition.y -= 4.0f;
	if (IsKeyDown(KEY_DOWN)) rectanglePosition.y += 4.0f;

	BeginDrawing();
		ClearBackground(RAYWHITE);
			if (rand() > RAND_MAX/2)
				DrawRectangleV(rectanglePosition, size, MAGENTA);
			else 
				DrawRectangleV(rectanglePosition, size, RED);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
