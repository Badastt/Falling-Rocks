#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

#define MAX_FPS 60
#define GRAVITY 1.02
#define PLAYER_JUMP_SPD 15.0f
#define PLAYER_HOR_SPD 4.0f
#define T 8

typedef struct {
	Vector2 position;
	Vector2 size;
	double speed;
	double speedX;
	bool canJump;
	bool jumping;
	int jumpTimer;
	float airTimer;
	Texture2D texture;
	Rectangle frameRec;
} Player;


typedef struct{
    Rectangle rect;
    Color color;
	char type;
} EnvItem;

void UpdatePlayer(Player *player, Camera2D *camera, EnvItem *envItems, int envItemsLength);

int main(void)
{
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	InitWindow(screenWidth, screenHeight, "FallRocks");
	
	if (!IsWindowMaximized())
		MaximizeWindow();
	
	Player Rill = {0};
	Rill.position = (Vector2){0, -32};
	Rill.size = (Vector2){32, 32};
	Rill.speed = 0;
	Rill.canJump = false;
	Rill.airTimer = 1;
	Rill.texture = LoadTexture("data/Rill.png");
	Rill.frameRec = (Rectangle){0.0f, 0.0f, (double)Rill.texture.width/2, (double)Rill.texture.height/1};

	Camera2D camera = {0};
	camera.target = Rill.position;
	camera.offset = (Vector2){1920/2.0f, 1080/2.0f};	
	camera.rotation = 0.0f;
	camera.zoom = 2.5f;

	EnvItem envItems[] = {
        {{ -3000, -10000, 6000, 12000 }, LIGHTGRAY, 'P' },
        {{ -75*T, 0, 150*T, 37.5*T }, BROWN, 'F' },
        {{ 300, 200, 50*T, 3*T }, GRAY, 'F' },
        {{ 250, 300, 12.5*T, T }, GRAY, 'F' },
        {{ 650, 300, 12.5*T, T }, GRAY, 'F' }
    };
	
	int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);
	int frame = 0;
	SetTargetFPS(MAX_FPS);
	
	while (!WindowShouldClose())
	{
		float deltaTime = GetFrameTime();
		if (frame++ == 60) frame = 0;
		UpdatePlayer (&Rill, &camera, envItems, envItemsLength);
		
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

void UpdatePlayer(Player *player, Camera2D *camera, EnvItem *envItems, int envItemsLength)
{
    if (IsKeyDown(KEY_LEFT)) 
		player->speedX = -PLAYER_HOR_SPD;
    else if (IsKeyDown(KEY_RIGHT)) 
		player->speedX = PLAYER_HOR_SPD;
	else 
		player->speedX = 0;
	
    if (IsKeyDown(KEY_UP) && player->canJump)
        player->jumping = true;
	else {
		player->canJump = false;
	}
	
    if (player->jumping && player->canJump){
		if (!player->airTimer){
			player->airTimer = 1;
			player->speed = -PLAYER_JUMP_SPD;
		}
		else if (player->jumpTimer++ >= 24);
			player->canJump = false;
    } else {
		if (player->jumpTimer)
			player->speed = +1;
		player->jumping = false;
		player->canJump = false;
	}

    int hitObstacle = 0;
    for (int i = 0; i < envItemsLength; i++)
    {
		EnvItem *ei = envItems + i;
		Vector2 *p = &(player->position);
		switch (ei->type){
			case 'P':
			if (ei->rect.x <= p->x &&
				ei->rect.x + ei->rect.width >= p->x &&
				ei->rect.y >= p->y+32 &&
				ei->rect.y <= p->y+32 + player->speed*player->airTimer*GRAVITY)
			{
				hitObstacle = 1;
				player->speed = 0.0f;
				p->y = ei->rect.y-32;
			}
			break;
			
			case 'F':
			if ((ei->rect.x <= p->x+8|| ei->rect.x <= p->x+20)  &&
				(ei->rect.x + ei->rect.width >= p->x+8 || ei->rect.x + ei->rect.width >= p->x+20) &&
				ei->rect.y >= p->y+32 &&
				ei->rect.y <= p->y+32 + player->speed*player->airTimer*GRAVITY)
			{
				hitObstacle = 1;
				player->speed = 0.0f;
				p->y = ei->rect.y-32;
			}
			
			if ((ei->rect.x <= p->x+8|| ei->rect.x <= p->x+20)  &&
				(ei->rect.x + ei->rect.width >= p->x+8 || ei->rect.x + ei->rect.width >= p->x+20) &&
				ei->rect.y + ei->rect.height >= p->y &&
				ei->rect.y + ei->rect.height <= p->y - player->speed*player->airTimer*GRAVITY)
			{
				hitObstacle = 0;
				player->jumping = false;
				player->canJump = false;
				player->speed = 0.0f;
				p->y = ei->rect.y + ei->rect.height;
			}
			
			if (ei->rect.y <= p->y &&
				ei->rect.y + ei->rect.height >= p->y+32 &&
				ei->rect.x + ei->rect.width <= p->x+8 &&
				ei->rect.x + ei->rect.width >= p->x+8 - player->speedX)
			{
				hitObstacle = 0;
				player->speedX = 0.0f;
				p->x = ei->rect.x + ei->rect.width + 8;
			}
			break;
			
			case 'B':
			break;
			
			default:
			break;
		}
    }
	
	if (player->speedX != 0){
		player->position.x += player->speedX;
		camera->target.x += player->speedX;
		if (player->speedX > 0)
			player->frameRec.x = player->texture.width;
		else
			player->frameRec.x = player->texture.width/2;
	}
		
    if (!hitObstacle)
    {
		if (!player->airTimer) player->airTimer = 1;
        player->position.y += player->speed*player->airTimer;
		camera->target.y += player->speed*player->airTimer;
        player->speed += GRAVITY*player->airTimer;
		if (player->speed > 8) player->speed = 8;
		player->airTimer += 0.01;
		if (player->airTimer == 1.11) player->airTimer = 1.1;
    }
    else {player->canJump = true; player->jumping = false; player->jumpTimer = 0; player->airTimer = 0;}
	camera->target = player->position;
}