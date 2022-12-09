#include "raylib.h"
#include <stdlib.h>

int main(void)
{
	int screenWidth = 1366;
	int screenHeight = 768;
	InitWindow(1366, 768, "FallRocks");

	bool Jump;
	bool canJump;
	int JumpTimer = 0;

	
	Vector2 RillPosition = {screenWidth/2, screenHeight/2};
	Vector2 RillSize = {60, 120};
	//Rectangle frameRec = { 0.0f, 0.0f, (float)Rill.width/5, (float)Rill.height};
	
	SetTargetFPS(60);
	
	Rectangle floor = {0, screenHeight-85, screenWidth, 80};	

	while (!WindowShouldClose())
	{
		bool onFloor = CheckCollisionPointRec ((Vector2){RillPosition.x+RillSize.x/2, RillPosition.y+RillSize.y}, floor);
		
		if (IsKeyDown(KEY_RIGHT)) RillPosition.x += 8.0f;
		if (IsKeyDown(KEY_LEFT)) RillPosition.x -= 8.0f;
		
		if (RillPosition.y < 0 || RillPosition.y > screenHeight) RillPosition.y = screenHeight/2;
		if (RillPosition.x < 0 || RillPosition.x > screenWidth) RillPosition.x =screenWidth/2;
		
		if (IsKeyDown(KEY_UP) && canJump) 
			Jump = true;
		else
			canJump = false;

		if (Jump && canJump){
			JumpTimer++; 
			if (JumpTimer < 30)
				RillPosition.y-=6;
			else
				canJump = 0;
		} else if (!onFloor)
			RillPosition.y+=6;

		if (onFloor){Jump = false; JumpTimer = 0; canJump = true;}

		BeginDrawing();
		ClearBackground(WHITE);
			DrawRectangleRec(floor, GREEN);
			DrawRectangleV(RillPosition, RillSize, BLACK);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
