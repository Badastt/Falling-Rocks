#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

#define MAX_FPS 60
#define GRAVITY 1.02
#define PLAYER_JUMP_SPD 15.0f
#define PLAYER_HOR_SPD 3.0f
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
	int hitObstacleL, hitObstacleR;
	Vector2 respawnPos;
	int state;
} Player;


typedef struct{
    Rectangle rect;
    Color color;
	char type;
} EnvItem;

void UpdatePlayer(Player *player, Camera2D *camera, EnvItem *envItems, int envItemsLength);
void Die(Player *player, Camera2D *camera);

int main(void)
{
	//int screenWidth = GetScreenWidth();
	//int screenHeight = GetScreenHeight();
	InitWindow(1366, 768, "FallRocks");
	
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
	Rill.hitObstacleL = 0;
	Rill.hitObstacleR = 0;
	Rill.respawnPos = (Vector2){0, -32};
	//Rill.respawnPos = (Vector2){80*T, -44*T};
	Rill.state = 0;

	Camera2D camera = {0};
	camera.target = Rill.position;
	camera.offset = (Vector2){1366/2, 768/2};	
	camera.rotation = 0.0f;
	camera.zoom = 2.5f;

	//posX posY width height
	EnvItem envItems[] = {
        {{ -3000, -10000, 6000, 12000 }, GRAY, 'B' },
        {{ -80*T, -200*T, 80*T, 250*T }, BROWN, 'F' },
        {{ 102*T, -200*T, 80*T, 250*T }, BROWN, 'F' },
        {{ -20*T, 0, 40*T, 30*T }, BROWN, 'F' },
        {{ 20*T, 1*T, 80*T, 29*T }, BROWN, 'F' },
        {{ 40*T, -6*T, 8*T, 36*T }, BROWN, 'F' },
        {{ 55*T, -11*T, 8*T, 41*T }, BROWN, 'F' },
        {{ 70*T, -16*T, 8*T, 46*T }, BROWN, 'F' },
        {{ 78*T, -18*T, 24*T, 48*T }, BROWN, 'F' },
        {{ 0*T, -40*T, 42*T, 22*T }, BROWN, 'F' },
        {{ 42*T, -40*T, 20*T, 16*T }, BROWN, 'F' },
        {{ 62*T, -40*T, 22*T, 12*T }, BROWN, 'F' },
        {{ 84*T, -40*T, 18*T, 1*T }, BLACK, 'P' },
        {{ 80*T, -44*T, 1*T, 4*T }, BLACK, 'C' },
        {{ 80*T, -46*T, 3*T, 2*T }, RED, 'B' },
        {{ 78*T, -65*T, 24*T, 10*T }, BROWN, 'F' },
        {{ 84*T, -67*T, 5*T, 2*T }, BROWN, 'F' },
        {{ 24*T, -138*T, 39*T, 88*T }, BROWN, 'F' },
        {{ 90*T, -75*T, 8*T, 1*T }, BLACK, 'P' },
        {{ 66*T, -83*T, 14*T, 1*T }, BLACK, 'P' },
        {{ 68*T, -103*T, 34*T, 10*T }, BROWN, 'F' },
        {{ 50*T, -125*T, 48*T, 11*T }, BROWN, 'F' },
        {{ 0*T, -54*T, 4*T, 1*T }, BLACK, 'P' },
        {{ 10*T, -68*T, 4*T, 1*T }, BLACK, 'P' },
        {{ 20*T, -82*T, 4*T, 1*T }, BLACK, 'P' },
        {{ 10*T, -96*T, 4*T, 1*T }, BLACK, 'P' },
        {{ 0*T, -110*T, 4*T, 1*T }, BLACK, 'P' },
        {{ 10*T, -124*T, 4*T, 1*T }, BLACK, 'P' },
        {{ 20*T, -138*T, 4*T, 1*T }, BLACK, 'P' },
        {{ 48*T, 0, 7*T, 1*T }, RED, 'S' },
        {{ 77*T, -65*T, 1*T, 10*T }, RED, 'S' },
        {{ 77*T, -55*T, 25*T, 1*T }, RED, 'S' },
        {{ 77*T, -66*T, 7*T, 1*T }, RED, 'S' },
        {{ 89*T, -66*T, 13*T, 1*T }, RED, 'S' },
        {{ 63*T, -113*T, 1*T, 63*T }, RED, 'S' },
        {{ 68*T, -94*T, 34*T, 1*T }, RED, 'S' },
        {{ 63*T, -114*T, 35*T, 1*T }, RED, 'S' },
    };
	
	int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);
	int frame = 0;
	SetTargetFPS(MAX_FPS);
	
	while (!WindowShouldClose())
	{
		//float deltaTime = GetFrameTime();
		if (frame++ == 60) frame = 0;
		UpdatePlayer (&Rill, &camera, envItems, envItemsLength);
		
		BeginDrawing();
		ClearBackground(RAYWHITE);
		BeginMode2D(camera);
            for (int i = 0; i < envItemsLength; i++)
				DrawRectangleRec(envItems[i].rect, envItems[i].color);
			DrawTextureRec(Rill.texture, Rill.frameRec, Rill.position, WHITE);
			DrawText("Congrats! You made it to the final!", 65*T, -135*T, 18, GOLD);
		EndMode2D();
		EndDrawing();
	}
	
	UnloadTexture(Rill.texture);

	CloseWindow();

	return 0;
}

void UpdatePlayer(Player *player, Camera2D *camera, EnvItem *envItems, int envItemsLength)
{	
	player->speedX = 0;
    if (!player->state){
		if (IsKeyDown(KEY_LEFT) && !player->hitObstacleL) 
			player->speedX -= PLAYER_HOR_SPD;
		if (IsKeyDown(KEY_RIGHT) && !player->hitObstacleR) 
			player->speedX += PLAYER_HOR_SPD;
	}

	
    if (IsKeyDown(KEY_UP) && player->canJump){
        player->jumping = true;
        player->state = 0;
    } else
		player->canJump = false;
	
    if (player->jumping && player->canJump){
		if (!player->airTimer){
			player->airTimer = 1;
			player->speed = -PLAYER_JUMP_SPD;
		}
		//if (player->jumpTimer++ >= 24);
		//	player->canJump = false;
    } else {
		if (player->jumpTimer)
			player->speed = +1;
		player->jumping = false;
		player->canJump = false;
	}
	
	int hitA, hitD;
	if (IsKeyPressed(KEY_A))
		hitA = 1;
	else
		hitA = 0;
	
	if (IsKeyPressed(KEY_D))
		hitD = 1;
	else
		hitD = 0;
	
	player->hitObstacleL = 0;
	player->hitObstacleR = 0;

    int hitObstacle = 0;
    for (int i = 0; i < envItemsLength; i++)
    {
		EnvItem *ei = envItems + i;
		Vector2 *p = &(player->position);
		switch (ei->type){
			case 'P':
			if ((ei->rect.x <= p->x+8|| ei->rect.x <= p->x+20)  &&
				(ei->rect.x + ei->rect.width >= p->x+8 || ei->rect.x + ei->rect.width >= p->x+20) &&
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
			
			if ((ei->rect.y <= p->y+31 || ei->rect.y <= p->y+16 || ei->rect.y <= p->y) &&
				(ei->rect.y + ei->rect.height >= p->y+31 || ei->rect.y + ei->rect.height >= p->y) &&
				ei->rect.x + ei->rect.width >= p->x+8 &&
				ei->rect.x + ei->rect.width <= p->x+8 - player->speedX)
			{
				player->hitObstacleL = 1;
				p->x = ei->rect.x + ei->rect.width-8;
			}
			
			if ((ei->rect.y <= p->y+31 || ei->rect.y <= p->y+16 || ei->rect.y <= p->y) &&
				(ei->rect.y + ei->rect.height >= p->y+31 || ei->rect.y + ei->rect.height >= p->y) &&
				ei->rect.x <= p->x+26 &&
				ei->rect.x >= p->x+26 - player->speedX)
			{
				player->hitObstacleR = 1;
				p->x = ei->rect.x - 26;
			}
			
			if (hitA){
					if (ei->rect.y <= p->y+16 &&
						ei->rect.y + ei->rect.height >= p->y+16 &&
						ei->rect.x + ei->rect.width >= p->x-3 &&
						p->x+32 >= ei->rect.x + ei->rect.width)
					{
						player->state = 1;
					}
			}
		
			if (hitD){
				if (ei->rect.y <= p->y+16 &&
					ei->rect.y + ei->rect.height >= p->y+16 &&
					p->x+3 <= ei->rect.x &&
					p->x+32 >= ei->rect.x)
				{
					player->state = 1;
				}
			}
			break;
			
			case 'B':
			break;
			
			case 'S':
			if ((ei->rect.x <= p->x+8|| ei->rect.x <= p->x+20)  &&
				(ei->rect.x + ei->rect.width >= p->x+8 || ei->rect.x + ei->rect.width >= p->x+20) &&
				ei->rect.y >= p->y+32 &&
				ei->rect.y <= p->y+32 + player->speed*player->airTimer*GRAVITY)
			{	
				player->speed = 0;
				player->speedX = 0;
				Die(player, camera);
			}
			
			if ((ei->rect.x <= p->x+8|| ei->rect.x <= p->x+20)  &&
				(ei->rect.x + ei->rect.width >= p->x+8 || ei->rect.x + ei->rect.width >= p->x+20) &&
				ei->rect.y + ei->rect.height >= p->y &&
				ei->rect.y + ei->rect.height <= p->y - player->speed*player->airTimer*GRAVITY)
			{
				player->speed = 0;
				player->speedX = 0;
				Die(player, camera);
			}
			
			if ((ei->rect.y <= p->y+31 || ei->rect.y <= p->y+16 || ei->rect.y <= p->y) &&
				(ei->rect.y + ei->rect.height >= p->y+31 || ei->rect.y + ei->rect.height >= p->y) &&
				ei->rect.x + ei->rect.width >= p->x+8 &&
				ei->rect.x + ei->rect.width <= p->x+8 - player->speedX)
			{
				player->speed = 0;
				player->speedX = 0;
				Die(player, camera);
			}
			
			if ((ei->rect.y <= p->y+31 || ei->rect.y <= p->y+16 || ei->rect.y <= p->y) &&
				(ei->rect.y + ei->rect.height >= p->y+31 || ei->rect.y + ei->rect.height >= p->y) &&
				ei->rect.x <= p->x+26 &&
				ei->rect.x >= p->x+26 - player->speedX)
			{
				player->speed = 0;
				player->speedX = 0;
				Die(player, camera);
			}
				
			break;
			
			case 'C':
			if ((ei->rect.x <= p->x+8|| ei->rect.x <= p->x+20)  &&
				(ei->rect.x + ei->rect.width >= p->x+8 || ei->rect.x + ei->rect.width >= p->x+20) &&
				ei->rect.y >= p->y+32 &&
				ei->rect.y <= p->y+32 + player->speed*player->airTimer*GRAVITY)
			{	
				player->respawnPos.x = ei->rect.x; player->respawnPos.y = ei->rect.y;
			}
			
			if ((ei->rect.x <= p->x+8|| ei->rect.x <= p->x+20)  &&
				(ei->rect.x + ei->rect.width >= p->x+8 || ei->rect.x + ei->rect.width >= p->x+20) &&
				ei->rect.y + ei->rect.height >= p->y &&
				ei->rect.y + ei->rect.height <= p->y - player->speed*player->airTimer*GRAVITY)
			{
				player->respawnPos.x = ei->rect.x; player->respawnPos.y = ei->rect.y;
			}
			
			if (ei->rect.y <= p->y+31 &&
				ei->rect.y + ei->rect.height >= p->y+31 &&
				ei->rect.x + ei->rect.width >= p->x+8 &&
				ei->rect.x + ei->rect.width <= p->x+8 - player->speedX)
			{
				player->respawnPos.x = ei->rect.x; player->respawnPos.y = ei->rect.y;
			}
			
			if (ei->rect.y <= p->y+31 &&
				ei->rect.y + ei->rect.height >= p->y+31 &&
				ei->rect.x <= p->x+26 &&
				ei->rect.x >= p->x+26 - player->speedX)
			{
				player->respawnPos.x = ei->rect.x; player->respawnPos.y = ei->rect.y;
			}
				
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
		
    if (!hitObstacle && !player->state)
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

void Die(Player *player, Camera2D *camera){
	player->position = player->respawnPos;
	camera->target = player->respawnPos;
}
