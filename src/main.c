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
	int hitObstacleL, hitObstacleR, hitObstacleD, hitObstacleU;
	Vector2 respawnPos;
	int state;
	int mode;
	Vector2 hookPosition;
	double hookDistance;
	double hookAngle;
	double maxHookAngle;
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
	Rill.position = (Vector2){0, -32-10*T};
	//Rill.position = (Vector2){1072, -152-33};
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
	Rill.respawnPos = (Vector2){0-1, -32-1-10*T};
	//Rill.respawnPos = (Vector2){1000*T, -32};
	Rill.state = 0;
	Rill.mode = 0;
	Rill.hookPosition = (Vector2){0, 0};
	Rill.hookDistance = 0;
	Rill.hookAngle = 0;
	Rill.maxHookAngle = 0;
	Rill.hookForce = 0;
	Rill.hookStrength = 0;
	Rill.canSwing = 1;

	Camera2D camera = {0};
	camera.target = Rill.position;
	camera.offset = (Vector2){1366/2, 768/2};	
	camera.rotation = 0.0f;
	camera.zoom = 4.0f;
	
	Texture2D map = LoadTexture("data/Teste.png");
	
	//posX posY width height
	EnvItem envItems[] = {
		{{0, -6000, 2000, 6000}, GRAY, 'B'},
		{{-200, -6200, 200, 6400}, BROWN, 'F'},
		{{2000, -6200, 200, 6400}, BROWN, 'F'},
		{{0, 0, 2000, 200}, BROWN, 'F'},
		{{0, -80, 120, 80}, BROWN, 'F'},
		{{240, -32, 48, 32}, BROWN, 'F'},
		{{288, -64, 48, 64}, BROWN, 'F'},
		{{336, -96, 48, 96}, BROWN, 'F'},
		{{384, -72, 24, 8}, BLACK, 'P'},//A PARTIR DESSE
		{{480, -96, 48, 96}, BROWN, 'F'},
		{{528, -80, 48, 80}, BROWN, 'F'},
		{{656, -80, 200, 80}, BROWN, 'F'},
		{{736, -500, 40, 340}, BROWN, 'F'},
		{{856, -280, 40, 280}, BROWN, 'F'},
		{{896, -120, 208, 120}, BROWN, 'F'},
		{{1104, -80, 32, 8}, BLACK, 'P'},
		{{936, -500, 32, 300}, BROWN, 'F'},
		{{1016, -320, 40, 320}, BROWN, 'F'},
		{{1080, -500, 224, 300}, BROWN, 'F'},
		{{1272, -80, 32, 8}, BLACK, 'P'},
		{{1304, -80, 320, 80}, BROWN, 'F'},
		
		
		{{0, -500, 736, 300}, BROWN, 'F'},
		{{776, -500, 800, 120}, BROWN, 'F'},
		
		
		
		{{1072, -152, 8, 32}, BLACK, 'C'},
        {{1072, -168, 24, 16}, RED, 'B'},
		{{496, -128, 8, 32}, BLACK, 'C'},
		{{496, -144, 24, 16}, RED, 'B'},
		
		
		{{576, -16, 80, 16}, BROWN, 'F'},//INUTIL
		{{1104, -16, 200, 16}, BROWN, 'F'},//INUTIL
		{{1104, -24, 200, 8}, RED, 'S'},
		{{576, -24, 80, 8}, RED, 'S'}
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
			DrawTextureRec(map, (Rectangle){0, 0, 2400, 6400}, (Vector2){-200, -6200}, WHITE);
			for (int i = 0; i < envItemsLength; i++)
				DrawRectangleRec(envItems[i].rect, envItems[i].color);
			DrawTextureRec(Rill.texture, Rill.frameRec, Rill.position, WHITE);
			DrawText("Congrats! You made it to the final!", 65*T, -135*T, 18, GOLD);
			if (Rill.mode)
				DrawLineV(Rill.hookPosition, (Vector2){Rill.position.x+OFFSET+10, Rill.position.y+16}, BLACK);
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
	
	if (player->hitObstacleU){
		player->hitObstacleD = 0;
		player->maxHookAngle = player->hookAngle;
		player->position.y = (envItems+player->hitObstacleU-1)->rect.y+(envItems+player->hitObstacleU-1)->rect.height;
		player->speedY = 0.0f;
		if (!player->mode){
			player->jumping = false;
			player->canJump = false;
			player->speedY = -player->speedY;
			player->speedX = 0;
		}
		
	}
	if (player->hitObstacleD){
		player->speedY = 0.0f;
		player->position.y = (envItems+player->hitObstacleD-1)->rect.y-32;
	}
	if (player->hitObstacleR && player->speedX >= 0){
		player->speedX = 0.0f;
		player->position.x = (envItems+player->hitObstacleR-1)->rect.x-(32-OFFSET);
	}
	if (player->hitObstacleL && player->speedX <= 0){
		player->speedX = 0.0f;
		player->position.x = (envItems+player->hitObstacleL-1)->rect.x + (envItems+player->hitObstacleL-1)->rect.width-OFFSET;
	}
	
	//if (player->speedX != 0 ){
		//player->position.x += player->speedX;
		//if (player->speedX > 0)
		//	player->frameRec.x = player->texture.width;
		//else
		//	player->frameRec.x = player->texture.width/2;
	//}
	
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
		player->speedY += GRAVITY;
		if (player->speedY > T) player->speedY = T;
	}
	
	if (!player->hitObstacleD && player->mode){
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
		player->mode = 0;
		if (player->jumpCooldown)
			player->jumpCooldown--;
	}
	
	player->position.y += player->speedY;
	player->position.x += player->speedX;
	camera->target = player->position;
}

void getInput(Player *player){
	if (!player->mode){
		player->speedX = 0;
		if (IsKeyDown(KEY_LEFT)) 
			player->speedX -= PLAYER_HOR_SPD;
		if (IsKeyDown(KEY_RIGHT)) 
			player->speedX += PLAYER_HOR_SPD;
		player->hookDistance = 0;
		player->hookForce = 0;
		player->acceleration = 0;
		player->speed = 0;
		player->hookAngle = 0;
		player->hookStrength = 0;
		player->canSwing = 0;
		player->maxHookAngle = 0;
	} else {
		player->speedX = 0;
		player->speedY = 0;
		player->canSwing = 1;
		
		if (player->hookDistance >= 10){
			if (player->hookAngle == 0)
				player->hookAngle = atan((player->hookPosition.x - (player->position.x+OFFSET+10)) / (player->hookPosition.y - (player->position.y+16)));
			player->hookForce = GRAVITY*0.2 * sin(player->hookAngle);
			player->acceleration = (-1 * player->hookForce) / player->hookDistance;
			player->speed += player->acceleration;
			player->hookAngle += player->speed;
			if (player->hookAngle >= player->maxHookAngle && player->maxHookAngle>0){
				player->hookAngle = player->maxHookAngle;
				player->speed *= 0.9;
			} else if (player->hookAngle <= player->maxHookAngle && player->maxHookAngle<0){
				player->hookAngle = player->maxHookAngle;
				player->speed *= 0.9;
			}

			player->speed *= 0.99;
			player->speedX = -(player->position.x+OFFSET+10) + (player->hookPosition.x - player->hookDistance * sin(player->hookAngle));
			player->speedY = -player->position.y + (player->hookPosition.y + player->hookDistance * cos(player->hookAngle));
		}
		
		if (IsKeyDown(KEY_DOWN) && !player->hitObstacleD && player->hookDistance <= 40){
			player->speedY += 0.5f;
			player->hookDistance += 0.5f;
			player->maxHookAngle = 0;
		}
		
		if (IsKeyDown(KEY_RIGHT) && player->hookDistance >= 10){ // ARRUMAR
			if (player->hookStrength != 10 && player->canSwing){
				player->hookStrength++;
				player->hookAngle -= 0.03;
			}
		}
		if (IsKeyDown(KEY_LEFT) && player->hookDistance >= 10){ // ARRUMAR ISSO E COLISOES DO PENDULO
			if (player->hookStrength != -10 && player->canSwing){
				player->hookStrength--;
				player->hookAngle += 0.03;
			}
		}
	}
	//player->hookDistance = sqrt(pow(fabs(player->position.x) - fabs(player->hookPosition.x), 2) + pow(fabs(player->position.y) - fabs(player->hookPosition.y) + 16, 2));
	//atan(player->hookPosition.x - player->position.x / player->hookPosition.y - player->position.y);
	
	if (IsKeyDown(KEY_UP) && player->canJump && !player->jumping)
		player->shouldJump = true;
	else if (IsKeyDown(KEY_UP) && player->canJump)
		player->jumping = true;
	else {
		player->jumping = false;
		player->canJump = false;
	}
	
	if (IsKeyPressed(KEY_A) && player->state > -2000){
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
	
	player->hitObstacleU = 0;
	player->hitObstacleD = 0;
	player->hitObstacleR = 0;
	player->hitObstacleL = 0;

	for (int i = 0; i < envItemsLength; i++){
		EnvItem *ei = envItems + i;
		Vector2 *p = &(player->position);
		switch (ei->type){
			case 'P':
			if (CCUpDown(player, ei, OFFSET))
				player->hitObstacleD = i+1;
			break;
			
			case 'F':
			if (CCUpDown(player, ei, OFFSET+1))
				player->hitObstacleD = i+1;
			
			if (CCDownUp(player, ei, OFFSET+1))
				player->hitObstacleU = i+1;
			
			if (CCLeftRight(player, ei, OFFSET))
				player->hitObstacleR = i+1;
			
			if (CCRightLeft(player, ei, OFFSET))
				player->hitObstacleL = i+1;
			
			if (player->hitA && !player->mode)
				if (((p->y+16 <= ei->rect.y && p->y+24 >= ei->rect.y+ei->rect.height-1) ||
					(p->y+16 >= ei->rect.y && p->y+16 <= ei->rect.y+ei->rect.height-1) ||
					(p->y+24 <= ei->rect.y+ei->rect.height-1 && p->y+24 >= ei->rect.y)) &&
					ei->rect.x + ei->rect.width >= p->x-OFFSET &&
					p->x+32 >= ei->rect.x + ei->rect.width){
						player->mode = 1;
						
						if (player->state > -1)
							player->state = -1;
						else
							player->state = -2;
						
						if (player->speedY > 0)
							player->hookPosition = (Vector2){ei->rect.x+ei->rect.width, p->y-player->speedY+16};
						else if (player->speedY < 0)
							player->hookPosition = (Vector2){ei->rect.x+ei->rect.width, p->y+player->speedY+16};
						else
							player->hookPosition = (Vector2){ei->rect.x+ei->rect.width, p->y+16};
						
						//player->hookDistance = sqrt(pow(fabs(player->position.x)+OFFSET+10 - fabs(player->hookPosition.x), 2) + pow(fabs(player->position.y) - fabs(player->hookPosition.y)+16, 2));
						player->hookDistance = fabs(player->position.x) - fabs(player->hookPosition.x);
				}
			
			if (player->hitD && !player->mode)
				if (((p->y+16 <= ei->rect.y && p->y+24 >= ei->rect.y+ei->rect.height-1) ||
					(p->y+16 >= ei->rect.y && p->y+16 <= ei->rect.y+ei->rect.height-1) ||
					(p->y+24 <= ei->rect.y+ei->rect.height-1 && p->y+24 >= ei->rect.y)) &&
					ei->rect.x <= p->x+32+OFFSET &&
					p->x <= ei->rect.x){
						player->mode = 1;
						
						if (player->state < 1)
							player->state = 1;
						else
							player->state = 2;

						if (player->speedY > 0)
							player->hookPosition = (Vector2){ei->rect.x, p->y-player->speedY+16};
						else if (player->speedY < 0)
							player->hookPosition = (Vector2){ei->rect.x, p->y+player->speedY+16};
						else
							player->hookPosition = (Vector2){ei->rect.x, p->y+16};
						player->hookDistance = sqrt(pow(fabs(player->position.x)+OFFSET+10 - fabs(player->hookPosition.x), 2) + pow(fabs(player->position.y) - fabs(player->hookPosition.y)+16, 2));
						//player->hookDistance = fabs(player->position.x) - fabs(player->hookPosition.x);
					}
			
			if (player->hitW && !player->mode){
				if (((p->x+OFFSET+1 <= ei->rect.x && p->x+(32-OFFSET+1) >= ei->rect.x + ei->rect.width) ||
				(p->x+OFFSET+1 >= ei->rect.x && p->x+OFFSET <= ei->rect.x+ei->rect.width) ||
				(p->x+(32-OFFSET+1) <= ei->rect.x+ei->rect.width && p->x+(32-OFFSET+1) >= ei->rect.x)) &&
				ei->rect.y + ei->rect.height-1 >= p->y-16 &&
				ei->rect.y + ei->rect.height-1 <= p->y){
					player->mode = 1;
					
					if (player->state == -1)
						player->state = -2;
					else if (player->state == 0)
						player->state = 1;
					else if (player->state == 1)
						player->state = 2;
					
					if (player->speedX < 0)
						player->hookPosition = (Vector2){p->x-PLAYER_HOR_SPD+OFFSET+10, ei->rect.y+ei->rect.height};
					else if (player->speedX > 0)
						player->hookPosition = (Vector2){p->x+PLAYER_HOR_SPD+OFFSET+10, ei->rect.y+ei->rect.height};
					else
						player->hookPosition = (Vector2){p->x+OFFSET+10, ei->rect.y+ei->rect.height};
					
					player->hookDistance = fabs(player->hookPosition.y) - fabs(player->position.y);
				}
			}
			break;
			
			case 'B':
			break;
			
			case 'S':
			if (CCUpDown(player, ei, OFFSET+3)){
				Die(player, camera);
				i = envItemsLength;
			}
			
			if (CCDownUp(player, ei, OFFSET+3)){
				Die(player, camera);
				i = envItemsLength;
			}
			
			if (CCLeftRight(player, ei, OFFSET+1)){
				Die(player, camera);
				i = envItemsLength;
			}
			
			if (CCRightLeft(player, ei, OFFSET+1)){
				Die(player, camera);
				i = envItemsLength;
			}
				
			break;
			
			case 'C':
			if (CCUpDown(player, ei, OFFSET)){
				player->respawnPos.x = ei->rect.x; player->respawnPos.y = ei->rect.y-1;
			}
			
			if (CCDownUp(player, ei, OFFSET)){
				player->respawnPos.x = ei->rect.x; player->respawnPos.y = ei->rect.y-1;
			}
			
			if (CCLeftRight(player, ei, OFFSET)){
				player->respawnPos.x = ei->rect.x; player->respawnPos.y = ei->rect.y-1;
			}
			
			if (CCRightLeft(player, ei, OFFSET)){
				player->respawnPos.x = ei->rect.x; player->respawnPos.y = ei->rect.y-1;
			}
				
			break;
			
			default:
			break;
		}
    }
}

void Die(Player *player, Camera2D *camera){
	player->hitObstacleU = 0;
	player->hitObstacleD = 0;
	player->hitObstacleR = 0;
	player->hitObstacleL = 0;
	player->mode = 0;
	player->speedX = 0;
	player->speedY = 0;
	player->position = player->respawnPos;
	camera->target = player->respawnPos;
	player->jumpCooldown = 20;
}

int CheckCollisionUpDown(Player *p, EnvItem *ei, int offsetX){
	if (((p->position.x+offsetX <= ei->rect.x && p->position.x+(32-offsetX) >= ei->rect.x+ei->rect.width) ||
		(p->position.x+offsetX >= ei->rect.x && p->position.x+offsetX <= ei->rect.x+ei->rect.width) ||
		(p->position.x+(32-offsetX) <= ei->rect.x+ei->rect.width && p->position.x+(32-offsetX) >= ei->rect.x)) &&
		ei->rect.y >= p->position.y+32 &&
		ei->rect.y <= p->position.y+32 + (p->speedY+2))
		return 1;
	return 0;
}

int CheckCollisionDownUp(Player *p, EnvItem *ei, int offsetX){
	if (((p->position.x+offsetX <= ei->rect.x && p->position.x+(32-offsetX) >= ei->rect.x+ei->rect.width) ||
		(p->position.x+offsetX >= ei->rect.x && p->position.x+offsetX <= ei->rect.x+ei->rect.width) ||
		(p->position.x+(32-offsetX) <= ei->rect.x+ei->rect.width && p->position.x+(32-offsetX) >= ei->rect.x)) &&
		ei->rect.y + ei->rect.height >= p->position.y &&
		ei->rect.y + ei->rect.height <= p->position.y - (p->speedY))
		return 1;

	return 0;
}

int CheckCollisionLeftRight(Player *p, EnvItem *ei, int offsetX){
	if (((p->position.y <= ei->rect.y && p->position.y+32 >= ei->rect.y+ei->rect.height-1) ||
		(p->position.y >= ei->rect.y && p->position.y <= ei->rect.y+ei->rect.height-1) ||
		(p->position.y+32 <= ei->rect.y+ei->rect.height-1 && p->position.y+32 >= ei->rect.y)) &&
		ei->rect.x <= p->position.x+(32-offsetX) &&
		ei->rect.x >= p->position.x+(32-offsetX) - (p->speedX+2))
		return 1;
	return 0;
}

int CheckCollisionRightLeft(Player *p, EnvItem *ei, int offsetX){
	if (((p->position.y <= ei->rect.y && p->position.y+32 >= ei->rect.y+ei->rect.height-1) ||
		(p->position.y >= ei->rect.y && p->position.y <= ei->rect.y+ei->rect.height-1) ||
		(p->position.y+32 <= ei->rect.y+ei->rect.height-1 && p->position.y+32 >= ei->rect.y)) &&
		ei->rect.x + ei->rect.width >= p->position.x+offsetX &&
		ei->rect.x + ei->rect.width <= p->position.x+offsetX - (p->speedX-2))
		return 1;
	return 0;
}

int CheckCollisionLeftRight2(Player *p, EnvItem *ei, int offsetX){
	if (((p->position.y <= ei->rect.y && p->position.y+31 >= ei->rect.y+ei->rect.height-1) ||
		(p->position.y >= ei->rect.y && p->position.y <= ei->rect.y+ei->rect.height-1) ||
		(p->position.y+31 <= ei->rect.y+ei->rect.height-1 && p->position.y+31 >= ei->rect.y)) &&
		ei->rect.x <= p->position.x+(32-offsetX) &&
		ei->rect.x >= p->position.x+(32-offsetX) - (p->speedX+2))
		return 1;
	return 0;
}

int CheckCollisionRightLeft2(Player *p, EnvItem *ei, int offsetX){
	if (((p->position.y <= ei->rect.y && p->position.y+31 >= ei->rect.y+ei->rect.height-1) ||
		(p->position.y >= ei->rect.y && p->position.y <= ei->rect.y+ei->rect.height-1) ||
		(p->position.y+31 <= ei->rect.y+ei->rect.height-1 && p->position.y+31 >= ei->rect.y)) &&
		ei->rect.x + ei->rect.width >= p->position.x+offsetX &&
		ei->rect.x + ei->rect.width <= p->position.x+offsetX - (p->speedX-2))
		return 1;
	return 0;
}
