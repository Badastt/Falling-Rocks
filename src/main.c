#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define M_PI 3.14159265358979323846

#define MAX_FPS 60
#define GRAVITY 0.5
#define T 8.0f
#define MAXHEIGHT 80 //10*T
double PLAYER_JUMP_SPD = 8.94427191; //sqrt(2*MAXHEIGHT*GRAVITY);
double PLAYER_HOR_SPD =  (0.25f * T);
#define OFFSET 6

typedef struct {
	Vector2 position;
	Vector2 size;
	double speed;
	double speedY;
	double speedX;
	double acceleration;
	bool canJump;
	bool shouldJump;
	bool jumping;
	int jumpCooldown;
	bool hitA, hitD, hitW;
	Texture2D texture;
	Rectangle frameRec;
	int hitObstacleL, hitObstacleR, hitObstacleD;
	Vector2 respawnPos;
	int state;
	int mode;
	Vector2 hookPosition;
	double hookDistance;
	double hookAngle;
	double hookForce;
	int hookStrength;
	bool canSwing;
} Player;


typedef struct{
    Rectangle rect;
    Color color;
	char type;
} EnvItem;

void UpdatePlayer(Player *player, Camera2D *camera, EnvItem *envItems, int envItemsLength);
void Die(Player *player, Camera2D *camera);
void getInput(Player *player);
void checkCollisions(Player *player, Camera2D *camera, EnvItem *envItems, int envItemsLength);
int CheckCollisionUpDown(Player *p, EnvItem *ei, int offset);
int CheckCollisionDownUp(Player *p, EnvItem *ei, int offset);
int CheckCollisionLeftRight(Player *p, EnvItem *ei, int offset);
int CheckCollisionRightLeft(Player *p, EnvItem *ei, int offset);
int CheckCollisionLeftRight2(Player *p, EnvItem *ei, int offset);
int CheckCollisionRightLeft2(Player *p, EnvItem *ei, int offset);


int main(void)
{
	//int screenWidth = GetScreenWidth();
	//int screenHeight = GetScreenHeight();
	InitWindow(1366, 768, "FallRocks");
	
	if (!IsWindowMaximized())
		MaximizeWindow();
	
	Player Rill = {0};
	Rill.position = (Vector2){0, -32};
	//Rill.position = (Vector2){1000*T, -32};
	Rill.size = (Vector2){32, 32};
	Rill.speed = 0;
	Rill.speedY = 0;
	Rill.speedX = 0;
	Rill.canJump = false;
	Rill.shouldJump = false;
	Rill.jumpCooldown = 0;
	Rill.texture = LoadTexture("data/RillDebug.png");
	Rill.frameRec = (Rectangle){0.0f, 0.0f, (double)Rill.texture.width/1, (double)Rill.texture.height/1};
	Rill.hitObstacleL = 0;
	Rill.hitObstacleR = 0;
	Rill.respawnPos = (Vector2){0, -32};
	//Rill.respawnPos = (Vector2){1000*T, -32};
	Rill.state = 0;
	Rill.mode = 0;
	Rill.hookPosition = (Vector2){0, 0};
	Rill.hookDistance = 0;
	Rill.hookForce = 0;
	Rill.hookStrength = 0;
	Rill.canSwing = 1;

	Camera2D camera = {0};
	camera.target = Rill.position;
	camera.offset = (Vector2){1366/2, 768/2};	
	camera.rotation = 0.0f;
	camera.zoom = 4.0f;
	
	//Texture2D BlockTest = LoadTexture("data/Block.png");
	
	//posX posY width height
	EnvItem envItems[] = {
        {{ -3000, -20000, 6000, 24000 }, GRAY, 'B' },
        {{ -80*T, -200*T, 80*T, 250*T }, BROWN, 'F' },
        {{ 102*T, -200*T, 80*T, 250*T }, BROWN, 'F' },
        {{ -20*T, 0, 100*T, 30*T }, BROWN, 'F' },
        {{ 40*T, -10*T, 8*T, 36*T }, BROWN, 'F' },
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
		{{ 1000*T, 0, 100*T, 1*T}, BROWN, 'F' },
		{{ 1010*T, -5*T, 10*T, 1*T}, BROWN, 'F' },
		{{ 1020*T, -6*T, 10*T, 1*T}, BROWN, 'F' },
		{{ 1030*T, -7*T, 10*T, 1*T}, BROWN, 'F' },
		{{ 1040*T, -8*T, 10*T, 1*T}, BROWN, 'F' },
		{{ 1050*T, -9*T, 10*T, 1*T}, BROWN, 'F' },
		{{ 1060*T, -10*T, 10*T, 1*T}, BROWN, 'F' },
		{{ 1070*T, -11*T, 10*T, 1*T}, BROWN, 'F' },
		{{ 1080*T, -12*T, 10*T, 1*T}, BROWN, 'F' },
		
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

void UpdatePlayer(Player *player, Camera2D *camera, EnvItem *envItems, int envItemsLength){	
	getInput(player);
	checkCollisions(player, camera, envItems, envItemsLength);
	
	if (player->speedX != 0 ){
		player->position.x += player->speedX;
		camera->target.x += player->speedX;
		//if (player->speedX > 0)
		//	player->frameRec.x = player->texture.width;
		//else
		//	player->frameRec.x = player->texture.width/2;
	}
	
	if (player->shouldJump && player->canJump && !player->jumpCooldown){
		player->speedY = -PLAYER_JUMP_SPD;
		player->shouldJump = false;
		player->jumping = true;
		player->jumpCooldown = 4;
		player->mode = 0;
	} else if (player->jumping && player->canJump)
		player->speedY = player->speedY;
	else if (!player->canJump){
		if (player->speedY < 0)
			player->speedY += GRAVITY*2;
		else
			player->speedY = player->speedY;
		player->jumping = false;
		player->canJump = false;
	}
		
	if (!player->hitObstacleD && !player->mode){
		player->position.y += player->speedY;
		camera->target.y += player->speedY;
		player->speedY += GRAVITY;
		if (player->speedY > T) player->speedY = T;
	}
	
	if (!player->hitObstacleD && player->mode){
		player->position.y += player->speedY;
		camera->target.y += player->speedY;
		player->canJump = true;
		player->shouldJump = false;
		player->jumping = false;
		if (player->jumpCooldown)
			player->jumpCooldown--;
	}

	if (player->hitObstacleD){
		player->canJump = true;
		player->shouldJump = false;
		player->jumping = false;
		player->state = 0;
		if (player->jumpCooldown)
			player->jumpCooldown--;
	}
	
	camera->target = player->position;
}

void getInput(Player *player){
	if (!player->mode){
		player->speedX = 0;
		if (IsKeyDown(KEY_LEFT) && !player->hitObstacleL) 
			player->speedX -= PLAYER_HOR_SPD;
		if (IsKeyDown(KEY_RIGHT) && !player->hitObstacleR) 
			player->speedX += PLAYER_HOR_SPD;
		player->hookDistance = 0;
		player->hookAngle = M_PI/4;
	} else {
		if (player->hookDistance < 10)
			player->speedX = 0;
		if (IsKeyDown(KEY_DOWN) && !player->hitObstacleD && player->hookDistance <= 30){
			player->speedY = 0.5f;
			player->hookDistance += 0.5f;
		} else
			player->speedY = 0;
		player->canSwing = 1;
		
		player->hookForce = GRAVITY * sin(player->hookAngle * (M_PI / 180.0));
		player->acceleration = (-1 * player->hookForce) / player->hookDistance;
		player->speed += player->acceleration;
		player->hookAngle += player->speed;
		
		if (IsKeyDown(KEY_RIGHT) && player->hookDistance >= 10){
			if (player->hookStrength != 10 && player->canSwing){
				player->hookStrength++;
				player->speed *= 1.01;
			}
			player->position.x = player->hookPosition.x + player->hookDistance * sin(player->hookAngle);
			player->position.y = player->hookPosition.y + player->hookDistance * cos(player->hookAngle);
		}
		if (IsKeyDown(KEY_LEFT) && player->hookDistance >= 10){
			player->speedX += (cos(player->hookAngle * (M_PI / 180.0)));
			player->speedY -= (sin(player->hookAngle * (M_PI / 180.0)));
		}
	}
	
	if (IsKeyDown(KEY_UP) && player->canJump && !player->jumping)
		player->shouldJump = true;
	else if (IsKeyDown(KEY_UP) && player->canJump)
		player->jumping = true;
	else {
		player->jumping = false;
		player->canJump = false;
	}
	
	if (IsKeyPressed(KEY_A) && player->state > -2){
		player->hitA = 1;
	} else
		player->hitA = 0;
	
	if (IsKeyPressed(KEY_D) && player->state < 2){
		player->hitD = 1;
	} else
		player->hitD = 0;
	
	if (IsKeyPressed(KEY_W) && player->state != -2 && player->state != 2){
		player->hitW = 1;
	} else
		player->hitW = 0;
}

void checkCollisions(Player *player, Camera2D *camera, EnvItem *envItems, int envItemsLength){
	
	player->hitObstacleL = 0;
	player->hitObstacleR = 0;
	
	int (*CCUpDown) (Player*, EnvItem*, int) = CheckCollisionUpDown;
	int (*CCDownUp) (Player*, EnvItem*, int) = CheckCollisionDownUp;
	int (*CCLeftRight) (Player*, EnvItem*, int);
	int (*CCRightLeft) (Player*, EnvItem*, int);
	if (!player->hitObstacleD){
		CCLeftRight = CheckCollisionLeftRight;
		CCRightLeft = CheckCollisionRightLeft;
	} else {
		CCLeftRight = CheckCollisionLeftRight2;
		CCRightLeft = CheckCollisionRightLeft2;
	}
	player->hitObstacleD = 0;
	
	for (int i = 0; i < envItemsLength; i++){
		EnvItem *ei = envItems + i;
		Vector2 *p = &(player->position);
		switch (ei->type){
			case 'P':
			if (CCUpDown(player, ei, OFFSET)){
				player->hitObstacleD = 1;
				player->speedY = 0.0f;
				p->y = ei->rect.y-32;
			}
			break;
			
			case 'F':
			if (CCUpDown(player, ei, OFFSET+1)){
				player->hitObstacleD = 1;
				player->speedY = 0.0f;
				p->y = ei->rect.y-32;
			}
			
			if (CCDownUp(player, ei, OFFSET+1)){
				player->hitObstacleD = 0;
				player->jumping = false;
				player->canJump = false;
				player->speedY = 0.0f;
				p->y = ei->rect.y + ei->rect.height;
			}
			
			if (CCLeftRight(player, ei, OFFSET)){
				player->hitObstacleR = 1;
				p->x = ei->rect.x-(32-OFFSET);
				player->speedX = 0;
			}
			
			if (CCRightLeft(player, ei, OFFSET)){
				player->hitObstacleL = 1;
				p->x = ei->rect.x + ei->rect.width-OFFSET;
				player->speedX = 0;
			}
			
			if (player->hitA && !player->mode)
				if (((p->y+16 <= ei->rect.y && p->y+24 >= ei->rect.y+ei->rect.height-1) ||
					(p->y+16 >= ei->rect.y && p->y+16 <= ei->rect.y+ei->rect.height-1) ||
					(p->y+24 <= ei->rect.y+ei->rect.height-1 && p->y+24 >= ei->rect.y)) &&
					ei->rect.x + ei->rect.width >= p->x-OFFSET &&
					p->x+32 >= ei->rect.x + ei->rect.width){
						player->mode = 1;
						player->speedY = 0;
						if (player->state > -1)
							player->state = -1;
						else
							player->state = -2;
						player->hookPosition = (Vector2){p->x+16, ei->rect.y+ei->rect.height-1};
				}
			
			if (player->hitD && !player->mode)
				if (((p->y+16 <= ei->rect.y && p->y+24 >= ei->rect.y+ei->rect.height-1) ||
					(p->y+16 >= ei->rect.y && p->y+16 <= ei->rect.y+ei->rect.height-1) ||
					(p->y+24 <= ei->rect.y+ei->rect.height-1 && p->y+24 >= ei->rect.y)) &&
					ei->rect.x <= p->x+32+OFFSET &&
					p->x <= ei->rect.x){
						player->mode = 1;
						player->speedY = 0;
						if (player->state < 1)
							player->state = 1;
						else
							player->state = 2;
						player->hookPosition = (Vector2){p->x+16, ei->rect.y+ei->rect.height-1};
					}
			
			if (player->hitW && !player->mode)
				if (((p->x+OFFSET+1 <= ei->rect.x && p->x+(32-OFFSET+1) >= ei->rect.x + ei->rect.width) ||
					(p->x+OFFSET+1 >= ei->rect.x && p->x+OFFSET <= ei->rect.x+ei->rect.width) ||
					(p->x+(32-OFFSET+1) <= ei->rect.x+ei->rect.width && p->x+(32-OFFSET+1) >= ei->rect.x)) &&
					ei->rect.y + ei->rect.height-1 >= p->y-8 &&
					ei->rect.y + ei->rect.height-1 <= p->y){
						player->mode = 1;
						player->speedY = 0;
						if (player->state == -1)
							player->state = -2;
						else if (player->state == 0)
							player->state = 1;
						else if (player->state == 1)
							player->state = 2;
						player->hookPosition = (Vector2){p->x+16, ei->rect.y+ei->rect.height-1};
				}
			break;
			
			case 'B':
			break;
			
			case 'S':
			if (CCUpDown(player, ei, OFFSET+1)){
				player->speedY = 0;
				player->speedX = 0;
				Die(player, camera);
			}
			
			if (CCDownUp(player, ei, OFFSET+1)){
				player->speedY = 0;
				player->speedX = 0;
				Die(player, camera);
			}
			
			if (CCLeftRight(player, ei, OFFSET+1)){
				player->speedY = 0;
				player->speedX = 0;
				Die(player, camera);
			}
			
			if (CCRightLeft(player, ei, OFFSET+1)){
				player->speedY = 0;
				player->speedX = 0;
				Die(player, camera);
			}
				
			break;
			
			case 'C':
			if (CCUpDown(player, ei, OFFSET)){
				player->respawnPos.x = ei->rect.x; player->respawnPos.y = ei->rect.y;
			}
			
			if (CCDownUp(player, ei, OFFSET)){
				player->respawnPos.x = ei->rect.x; player->respawnPos.y = ei->rect.y;
			}
			
			if (CCLeftRight(player, ei, OFFSET)){
				player->respawnPos.x = ei->rect.x; player->respawnPos.y = ei->rect.y;
			}
			
			if (CCRightLeft(player, ei, OFFSET)){
				player->respawnPos.x = ei->rect.x; player->respawnPos.y = ei->rect.y;
			}
				
			break;
			
			default:
			break;
		}
    }
}

void Die(Player *player, Camera2D *camera){
	player->position = player->respawnPos;
	camera->target = player->respawnPos;
	player->jumpCooldown = 15;
}

int CheckCollisionUpDown(Player *p, EnvItem *ei, int offset){
	if (((p->position.x+offset <= ei->rect.x && p->position.x+(32-offset) >= ei->rect.x+ei->rect.width) ||
		(p->position.x+offset >= ei->rect.x && p->position.x+offset <= ei->rect.x+ei->rect.width) ||
		(p->position.x+(32-offset) <= ei->rect.x+ei->rect.width && p->position.x+(32-offset) >= ei->rect.x)) &&
		ei->rect.y >= p->position.y+32 &&
		ei->rect.y <= p->position.y+32 + (p->speedY+1))
		return 1;
	return 0;
}

int CheckCollisionDownUp(Player *p, EnvItem *ei, int offset){
	if (((p->position.x+offset <= ei->rect.x && p->position.x+(32-offset) >= ei->rect.x+ei->rect.width) ||
		(p->position.x+offset >= ei->rect.x && p->position.x+offset <= ei->rect.x+ei->rect.width) ||
		(p->position.x+(32-offset) <= ei->rect.x+ei->rect.width && p->position.x+(32-offset) >= ei->rect.x)) &&
		ei->rect.y + ei->rect.height-1 >= p->position.y &&
		ei->rect.y + ei->rect.height-1 <= p->position.y - (p->speedY-1))
		return 1;
	return 0;
}

int CheckCollisionLeftRight(Player *p, EnvItem *ei, int offset){
	if (((p->position.y <= ei->rect.y && p->position.y+32 >= ei->rect.y+ei->rect.height-1) ||
		(p->position.y >= ei->rect.y && p->position.y <= ei->rect.y+ei->rect.height-1) ||
		(p->position.y+32 <= ei->rect.y+ei->rect.height-1 && p->position.y+32 >= ei->rect.y)) &&
		ei->rect.x <= p->position.x+(32-offset) &&
		ei->rect.x >= p->position.x+(32-offset) - p->speedX)
		return 1;
	return 0;
}

int CheckCollisionRightLeft(Player *p, EnvItem *ei, int offset){
	if (((p->position.y <= ei->rect.y && p->position.y+32 >= ei->rect.y+ei->rect.height-1) ||
		(p->position.y >= ei->rect.y && p->position.y <= ei->rect.y+ei->rect.height-1) ||
		(p->position.y+32 <= ei->rect.y+ei->rect.height-1 && p->position.y+32 >= ei->rect.y)) &&
		ei->rect.x + ei->rect.width >= p->position.x+offset &&
		ei->rect.x + ei->rect.width <= p->position.x+offset - p->speedX)
		return 1;
	return 0;
}

int CheckCollisionLeftRight2(Player *p, EnvItem *ei, int offset){
	if (((p->position.y <= ei->rect.y && p->position.y+31 >= ei->rect.y+ei->rect.height-1) ||
		(p->position.y >= ei->rect.y && p->position.y <= ei->rect.y+ei->rect.height-1) ||
		(p->position.y+31 <= ei->rect.y+ei->rect.height-1 && p->position.y+31 >= ei->rect.y)) &&
		ei->rect.x <= p->position.x+(32-offset) &&
		ei->rect.x >= p->position.x+(32-offset) - p->speedX)
		return 1;
	return 0;
}

int CheckCollisionRightLeft2(Player *p, EnvItem *ei, int offset){
	if (((p->position.y <= ei->rect.y && p->position.y+31 >= ei->rect.y+ei->rect.height-1) ||
		(p->position.y >= ei->rect.y && p->position.y <= ei->rect.y+ei->rect.height-1) ||
		(p->position.y+31 <= ei->rect.y+ei->rect.height-1 && p->position.y+31 >= ei->rect.y)) &&
		ei->rect.x + ei->rect.width >= p->position.x+offset &&
		ei->rect.x + ei->rect.width <= p->position.x+offset - p->speedX)
		return 1;
	return 0;
}

