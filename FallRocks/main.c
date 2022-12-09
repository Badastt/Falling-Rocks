#include "raylib.h"
#include <stdlib.h>

int main(void)
{
	int screenWidth = GetMonitorWidth(GetCurrentMonitor());
	int screenHeight = GetMonitorHeight(GetCurrentMonitor());
	InitWindow(1200, 1000, "FallRocks");
	
	bool Jump;
	int JumpTimer;
	
	Vector2 RillPosition = {350.0f, 250.0f};
	Vector2 RillSize = {60, 120};
	//Rectangle frameRec = { 0.0f, 0.0f, (float)Rill.width/5, (float)Rill.height};
	
	SetTargetFPS(60);
	
	Rectangle floor = {0, 900, 2000, 80};
	
	bool onFloor = CheckCollisionPointRec (RillPosition, floor);

	while (!WindowShouldClose())
	{
		
		if (IsKeyDown(KEY_RIGHT)) RillPosition.x += 8.0f;
		if (IsKeyDown(KEY_LEFT)) RillPosition.x -= 8.0f;
		
		
		if (!onFloor) RillPosition.y += 1.0f;
		else {Jump = false; JumpTimer = 0;}

		BeginDrawing();
		ClearBackground(WHITE);
			DrawRectangleRec(floor, BLACK);
			DrawRectangleV(RillPosition, RillSize, BLACK);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}