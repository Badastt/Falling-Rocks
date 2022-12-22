#include "raylib.h"
#include <stdlib.h>

#define MAX_FPS 60
#define G 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f

typedef struct {
	Vector2 position;
	Vector2 size;
	double speed;
	bool canJump;
	bool jumping;
	int jumpTimer;
	Texture2D texture;
	Rectangle frameRec;
} Player;


typedef struct{
    Rectangle rect;
    int blocking;
    Color color;
} EnvItem;

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta);

int main(void)
{
	int screenWidth = 1366;
	int screenHeight = 768;
	InitWindow(1366, 768, "FallRocks");
	
	Player Rill = {0};
	Rill.position = (Vector2){screenWidth/2 - 100, screenHeight/2 - 100};
	Rill.size = (Vector2){32, 32};
	Rill.speed = 0;
	Rill.canJump = false;
	Rill.texture = LoadTexture("data/Rill3.png");
	Rill.frameRec = (Rectangle){0.0f, 0.0f, (double)Rill.texture.width/1, (double)Rill.texture.height/1};

	Camera2D camera = {0};
	camera.target = Rill.position; //+32
	camera.offset = (Vector2){screenWidth/2.0f, screenHeight/2.0f};	
	camera.rotation = 0.0f;
	camera.zoom = 2.0f;

	EnvItem envItems[] = {
        {{ 0, 0, 1000, 400 }, 0, LIGHTGRAY },
        {{ 0, 400, 1000, 200 }, 1, GRAY },
        {{ 300, 200, 400, 10 }, 1, GRAY },
        {{ 250, 300, 100, 10 }, 1, GRAY },
        {{ 650, 300, 100, 10 }, 1, GRAY }
    };
	
	int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);

	SetTargetFPS(MAX_FPS);
	
	while (!WindowShouldClose())
	{
		float deltaTime = GetFrameTime();
		camera.target = (Vector2){Rill.position.x+32, Rill.position.y+32};

		UpdatePlayer (&Rill, envItems, envItemsLength, deltaTime); 

		/*if (IsKeyDown(KEY_UP) && canJump) 
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

		if (onFloor){Jump = false; JumpTimer = 0; canJump = true;}*/

		BeginDrawing();
		ClearBackground(RAYWHITE);
		BeginMode2D(camera);
            for (int i = 0; i < envItemsLength; i++)
				DrawRectangleRec(envItems[i].rect, envItems[i].color);
			DrawTextureRec(Rill.texture, Rill.frameRec, Rill.position, WHITE);
		EndMode2D();
		EndDrawing();
	}
	
	UnloadTexture(Rill.texture);

	CloseWindow();

	return 0;
}

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta)
{
    if (IsKeyDown(KEY_LEFT)) player->position.x -= PLAYER_HOR_SPD*delta;
    if (IsKeyDown(KEY_RIGHT)) player->position.x += PLAYER_HOR_SPD*delta;
    if (IsKeyDown(KEY_UP) && player->canJump)
    {
        player->speed = -PLAYER_JUMP_SPD;
        player->canJump = false;
    }
    
    if (player->jumping && player->canJump){
    	player->jumpTimer++;
    	if (player->jumpTimer < 3)
    		player->speed = -PLAYER_JUMP_SPD;
    	else
    		player->canJump = false;
    }

    int hitObstacle = 0;
    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem *ei = envItems + i;
        Vector2 *p = &(player->position);
        if (ei->blocking &&
            ei->rect.x <= p->x &&
            ei->rect.x + ei->rect.width >= p->x &&
            ei->rect.y >= p->y+32 &&
            ei->rect.y <= p->y+32 + player->speed*delta)
        {
            hitObstacle = 1;
            player->speed = 0.0f;
            p->y = ei->rect.y-32;
        }
    }

    if (!hitObstacle)
    {
        player->position.y += player->speed*delta;
        player->speed += G*delta;
        player->canJump = false;
    }
    else player->canJump = true;
}
