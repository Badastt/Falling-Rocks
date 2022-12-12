#include "raylib.h"
#include <stdlib.h>

#define MAX_FPS 60

typedef struct {
	Vector2 position;
	Vector2 size;
	Texture2D texture;
	Rectangle frameRec;
} Player;

int main(void)
{
	int screenWidth = 1366;
	int screenHeight = 768;
	InitWindow(1366, 768, "FallRocks");
	
	Player Rill = {{screenWidth/2, screenHeight/2}, {32, 32}, LoadTexture("data/Rill3.png"), {0.0f, 0.0f, (float)Rill.texture.width/1, (float)Rill.texture.height/1}};

	Camera2D camera = {{Rill.position.x+32, Rill.position.y+32}, (Vector2){screenWidth/2, screenHeight/2}, 0, 3};

	bool Jump;
	bool canJump;
	int JumpTimer = 0;
	unsigned int frameCounter = 0;
	
	SetTargetFPS(MAX_FPS);
	
	Rectangle floor = {0, screenHeight-85, screenWidth, 600};	

	while (!WindowShouldClose())
	{
		frameCounter++;
		frameCounter %= MAX_FPS;
		camera.target = (Vector2){Rill.position.x+32, Rill.position.y+32};

		bool onFloor = CheckCollisionPointRec ((Vector2){Rill.position.x+Rill.size.x/2, Rill.position.y+Rill.size.y}, floor);
		
		if (IsKeyDown(KEY_RIGHT)) Rill.position.x += 4.0f;
		if (IsKeyDown(KEY_LEFT)) Rill.position.x -= 4.0f;
		
		if (Rill.position.y < 0 || Rill.position.y > screenHeight) Rill.position.y = screenHeight/2;
		if (Rill.position.x < 0 || Rill.position.x > screenWidth) Rill.position.x =screenWidth/2;
		
		if (IsKeyDown(KEY_UP) && canJump) 
			Jump = true;
		else
			canJump = false;

		if (Jump && canJump){
			JumpTimer++; 
			if (JumpTimer < 30)
				Rill.position.y-=6;
			else
				canJump = 0;
		} else if (!onFloor)
			Rill.position.y+=6;

		if (onFloor){Jump = false; JumpTimer = 0; canJump = true;}

		BeginDrawing();
		ClearBackground(RAYWHITE);
		BeginMode2D(camera);
			DrawRectangleRec(floor, GREEN);
			DrawTextureRec(Rill.texture, Rill.frameRec, Rill.position, WHITE);
		EndMode2D();
		EndDrawing();
	}
	
	UnloadTexture(Rill.texture);

	CloseWindow();

	return 0;
}
