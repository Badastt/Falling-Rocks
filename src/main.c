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
#define OFFSET 7

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
	int hitA, hitD, hitW;
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

int down = 0, left = 0, right = 0;
int frame = 0;
int main(void)
{
	InitWindow(1366, 768, "FallRocks");
	
	if (!IsWindowMaximized())
		MaximizeWindow();
	
	Player Rill = {0};
	Rill.position = (Vector2){0, -32-10*T};
	Rill.size = (Vector2){32, 32};
	Rill.speed = 0;
	Rill.speedY = 0;
	Rill.speedX = 0;
	Rill.canJump = false;
	Rill.shouldJump = false;
	Rill.jumpCooldown = 0;
	Rill.texture = LoadTexture("data/RillWalking.png");
	Rill.frameRec = (Rectangle){0.0f, 0.0f, (double)Rill.texture.width/9, (double)Rill.texture.height/2};
	Rill.hitObstacleL = 0;
	Rill.hitObstacleR = 0;
	Rill.respawnPos = Rill.position;
	Rill.state = 0;
	Rill.mode = 0;
	Rill.hookPosition = (Vector2){0, 0};
	Rill.hookDistance = 0;
	Rill.hookAngle = 0;
	Rill.maxHookAngle = 0;
	Rill.hookForce = 0;
	Rill.hookStrength = 0;
	Rill.canSwing = 1;
	Rill.hitA = 0;
	Rill.hitD = 0;
	Rill.hitW = 0;

	Camera2D camera = {0};
	camera.target = Rill.position;
	camera.offset = (Vector2){1366/2, 768/2};	
	camera.rotation = 0.0f;
	camera.zoom = 4.0f;
	
	Texture2D map = LoadTexture("data/Mapa.png");
	
	//posX posY width height
	EnvItem envItems[] = {
		{{0, -6000, 2000, 6000}, GRAY, 'B'},
		{{-200, -6200, 200, 6400}, BROWN, 'F'},
		{{2000, -6200, 200, 6400}, BROWN, 'F'},	
		{{1792, -660, 2, 80}, WHITE, 's'},
		{{1706, -860, 2, 280}, WHITE, 's'},
		{{1000, -2700, 2, 844}, WHITE, 's'},
		{{1048, -3073, 260, 2}, WHITE, 's'},
		{{644, -3073, 260, 2}, WHITE, 's'},
		{{1096, -4100, 2, 332}, WHITE, 's'},
		{{1016, -4000, 2, 300}, WHITE, 's'},
		{{646, -4000, 2, 300}, WHITE, 's'},
		{{406, -5000, 2, 1000}, WHITE, 's'},
		{{472, -5000, 2, 900}, WHITE, 's'},
		{{0, 0, 2000, 200}, BROWN, 'F'},
		{{0, -80, 120, 80}, BROWN, 'F'},
		{{0, -500, 736, 300}, BROWN, 'F'},
		{{776, -500, 800, 120}, BROWN, 'F'},
		{{240, -32, 48, 32}, BROWN, 'F'},
		{{288, -64, 48, 64}, BROWN, 'F'},
		{{336, -96, 48, 96}, BROWN, 'F'},
		{{384, -72, 24, 8}, BLACK, 'P'},
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
		{{1566, -500, 80, 320}, BROWN, 'F'},
		{{1646, -188, 40, 8}, BLACK, 'P'},
		{{1646, -500, 320, 240}, BROWN, 'F'},
		{{1750, -80, 40, 8}, BLACK, 'P'},
		{{1790, -120, 40, 120}, BROWN, 'F'},
		{{1830, -120, 40, 8}, BLACK, 'P'},
		{{1930, -160, 40, 8}, BLACK, 'P'},
		{{1970, -160, 40, 160}, BROWN, 'F'},
		{{1800, -652, 200, 72}, BROWN, 'F'},
		{{1400, -860, 300, 280}, BROWN, 'F'},
		{{1832, -660, 80, 8}, BROWN, 'F'},
		{{900, -1260, 300, 760}, BROWN, 'F'},
		{{1240, -596, 80, 16}, BROWN, 'F'},
		{{1320, -588, 80, 8}, BROWN, 'F'},
		{{1360, -680, 40, 8}, BLACK, 'P'},
		{{1280, -860, 120, 24}, BROWN, 'F'},
		{{1220, -760, 80, 8}, BLACK, 'P'},
		{{1100, -1292, 510, 292}, BROWN, 'F'},
		{{1690, -1800, 310, 800}, BROWN, 'F'},
		{{1610, -940, 80, 8}, BLACK, 'P'},
		{{1666, -1008, 24, 8}, BLACK, 'P'},
		{{1610, -1108, 24, 8}, BLACK, 'P'},
		{{1666, -1208, 24, 8}, BLACK, 'P'},
		{{1610, -1300, 24, 8}, BLACK, 'P'},
		{{1560, -1300, 50, 8}, BROWN, 'F'},
		{{1008, -2700, 992, 1272}, BROWN, 'F'},
		{{1480, -1308, 16, 16}, BROWN, 'F'},
		{{1330, -1308, 24, 16}, BROWN, 'F'},
		{{1270, -1308, 16, 16}, BROWN, 'F'},
		{{1200, -1308, 16, 16}, BROWN, 'F'},
		{{1100, -1308, 26, 16}, BROWN, 'F'},
		{{0, -1308, 1100, 200}, BROWN, 'F'},
		{{960, -1500, 40, 8}, BLACK, 'P'},
		{{600, -2700, 200, 1400}, BROWN, 'F'},
		{{1000, -1728, 8, 300}, BROWN, 'F'},
		{{1000, -1856, 8, 60}, BROWN, 'F'},
		{{936, -2700, 8, 360}, BROWN, 'F'},
		{{928, -2700, 8, 430}, BROWN, 'F'},
		{{920, -2700, 8, 500}, BROWN, 'F'},
		{{912, -2700, 8, 570}, BROWN, 'F'},
		{{904, -2700, 8, 640}, BROWN, 'F'},
		{{896, -2700, 8, 710}, BROWN, 'F'},
		{{888, -2700, 8, 780}, BROWN, 'F'},
		{{880, -2700, 8, 850}, BROWN, 'F'},
		{{872, -2700, 8, 900}, BROWN, 'F'},
		{{864, -2700, 8, 950}, BROWN, 'F'},
		{{856, -2700, 8, 1000}, BROWN, 'F'},
		{{848, -2700, 8, 1050}, BROWN, 'F'},
		{{840, -2700, 8, 1100}, BROWN, 'F'},
		{{832, -2700, 8, 1150}, BROWN, 'F'},
		{{824, -2700, 8, 1200}, BROWN, 'F'},
		{{816, -2700, 8, 1250}, BROWN, 'F'},
		{{808, -2700, 8, 1300}, BROWN, 'F'},
		{{800, -2700, 8, 1350}, BROWN, 'F'},
		{{888, -1858, 32, 8}, BLACK, 'P'},
		{{944, -2700, 64, 8}, BLACK, 'P'},
		{{1832, -760, 80, 8}, BLACK, 'P'},
		{{1740, -820, 40, 8}, BLACK, 'P'},
		{{944, -3300, 64, 500}, BROWN, 'F'},
		{{1308, -4000, 300, 1300}, BROWN, 'F'},
		{{344, -4000, 300, 1300}, BROWN, 'F'},
		{{1008, -2980, 40, 8}, BLACK, 'P'},
		{{904, -2980, 40, 8}, BLACK, 'P'},
		{{1268, -2780, 40, 8}, BLACK, 'P'},
		{{1168, -2860, 40, 8}, BLACK, 'P'},
		{{1168, -2940, 40, 8}, BLACK, 'P'},
		{{1048, -3300, 260, 220}, BROWN, 'F'},
		{{644, -2780, 40, 8}, BLACK, 'P'},
		{{744, -2860, 40, 8}, BLACK, 'P'},
		{{744, -2940, 40, 8}, BLACK, 'P'},
		{{644, -3700, 260, 620}, BROWN, 'F'},
		{{904, -3300, 40, 8}, BLACK, 'P'},
		{{1008, -3300, 40, 8}, BLACK, 'P'},
		{{904, -3700, 340, 300}, BROWN, 'F'},
		{{1104, -4300, 220, 532}, BROWN, 'F'},
		{{970, -4000, 40, 300}, BROWN, 'F'},
		{{0, -5000, 400, 1000}, BROWN, 'F'},
		{{480, -5000, 800, 900}, BROWN, 'F'}, 
		{{420, -4080, 40, 8}, BLACK, 'P'},
		{{930, -3800, 40, 8}, BLACK, 'P'},
		{{800, -3760, 40, 8}, BLACK, 'P'},
		{{680, -3760, 40, 8}, BLACK, 'P'},
		{{680, -3960, 40, 8}, BLACK, 'P'},
		{{400, -5000, 80, 8}, BLACK, 'P'},
		{{1750, -532, 8, 32}, BLACK, 'C'},
		{{1072, -152, 8, 32}, BLACK, 'C'},
		{{1580, -1332, 8, 32}, BLACK, 'C'},
		{{496, -128, 8, 32}, BLACK, 'C'},
		{{944+28, -2732, 8, 32}, BLACK, 'C'},
		{{1204, -3732, 8, 32}, BLACK, 'C'},
		{{1048, -3080, 260, 8}, RED, 'S'},
		{{400, -4992, 8, 992}, RED, 'S'},
		{{472, -4992, 8, 892}, RED, 'S'},
		{{640, -4000, 8, 300}, RED, 'S'},
		{{1010, -4000, 8, 300}, RED, 'S'},
		{{640, -3708, 330, 8}, RED, 'S'},
		{{644, -3080, 260, 8}, RED, 'S'},
		{{1096, -4100, 8, 332}, RED, 'S'},
		{{1000, -2692, 8, 836}, RED, 'S'},//
		{{1100, -1300, 460, 8}, RED, 'S'},//
		{{1800, -660, 32, 8}, RED, 'S'},//
		{{1700, -860, 8, 280}, RED, 'S'},//
		{{1792, -660, 8, 80}, RED, 'S'},//
		{{1912, -660, 88, 8}, RED, 'S'},//
		{{1104, -24, 200, 12}, RED, 'S'},//
		{{576, -24, 80, 12}, RED, 'S'},//
		{{1624, -24, 376, 12}, RED, 'S'},//
		{{1320, -596, 80, 8}, RED, 'S'}//
		
		
		
    };
	
	int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);
	
	SetTargetFPS(MAX_FPS);
	
	while (!WindowShouldClose())
	{
		if (frame++ == 60) frame = 0;
		UpdatePlayer (&Rill, &camera, envItems, envItemsLength);
		
		BeginDrawing();
		ClearBackground(RAYWHITE);
		BeginMode2D(camera);
			DrawTextureRec(map, (Rectangle){0, 0, 2400, 6400}, (Vector2){-200, -6200}, WHITE);
			DrawTextureRec(Rill.texture, Rill.frameRec, Rill.position, WHITE);
			if (Rill.mode)
				DrawLineV(Rill.hookPosition, (Vector2){Rill.position.x+OFFSET+10, Rill.position.y+16}, WHITE);
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
	
	if (player->shouldJump && player->canJump && !player->jumpCooldown){
		player->speedY = -PLAYER_JUMP_SPD;
		player->shouldJump = false;
		player->jumping = true;
		player->jumpCooldown = 6;
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
	
	if ((player->hitA || player->hitD || player->hitW) && player->mode){
		player->jumpCooldown = 15;
		player->speedX = 0;
		player->speedY = 0;
	}
	
	player->position.y += player->speedY;
	player->position.x += player->speedX;
	camera->target = player->position;
	
	if (player->speedX > 0 && frame % 1 == 0 && player->hitObstacleD){
		player->frameRec.y = 0;
		player->frameRec.x += 32;
		if (player->frameRec.x >= player->texture.width)
			player->frameRec.x = 0;
	} else if (player->speedX < 0 && frame % 1 == 0 && player->hitObstacleD){
		player->frameRec.y = 32;
		player->frameRec.x += 32;
		if (player->frameRec.x >= player->texture.width)
			player->frameRec.x = 0;
	} else if (player->speedX < 0){
		player->frameRec.y = 32;
		player->frameRec.x = 0;
	} else if (player->speedX > 0){
		player->frameRec.y = 0;
		player->frameRec.x = 0;
	} else 
		player->frameRec.x = 0;
}

void getInput(Player *player){
	
	if (player->hookDistance < 40 && down && player->mode);
	else if (player->hitW) down = 1;
	else down = 0;
	
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
		
		if (player->hookDistance >= 25){
			if (player->hookAngle == 0)
				player->hookAngle = atan((fabs(player->hookPosition.x) - fabs(player->position.x+OFFSET+10)) / (fabs(player->hookPosition.y) - fabs(player->position.y+16)));
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
		
		if (!player->hitObstacleL && (player->hitA || left)){
			left = 1;
			player->speedX -= 1;
			down = 0;
		} else
			left = 0;
		
		if (!player->hitObstacleR && (player->hitD || right)){
			right = 1;
			player->speedX += 1;
			down = 0;
		} else 
			right = 0;
			
		if (((player->hookDistance <= 40) && (player->hitW || down)) || ((player->hookDistance <= 40) && IsKeyDown(KEY_DOWN))){
			player->speedY += 1.2f;
			player->hookDistance = sqrt(pow(fabs(player->position.x+OFFSET+10) - fabs(player->hookPosition.x), 2) + pow(fabs(player->position.y) - fabs(player->hookPosition.y), 2));
			player->maxHookAngle = 0;
		}
		
		if (IsKeyDown(KEY_RIGHT) && player->hookDistance >= 25){
			if (player->hookStrength != 10 && player->canSwing){
				player->hookStrength++;
				player->hookAngle -= 0.03;
			}
		}
		if (IsKeyDown(KEY_LEFT) && player->hookDistance >= 25){
			if (player->hookStrength != -10 && player->canSwing){
				player->hookStrength--;
				player->hookAngle += 0.03;
			}
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
	
	if (IsKeyPressed(KEY_A) && player->state > -2)
		player->hitA = 2;
	else if (player->hitA > 0)
		player->hitA--;
	else
		player->hitA = 0;
	
	if (IsKeyPressed(KEY_D) && player->state < 2)
		player->hitD = 2;
	else if (player->hitD > 0)
		player->hitD--;
	else
		player->hitD = 0;
	
	if (IsKeyPressed(KEY_W) && player->state != -2 && player->state != 2)
		player->hitW = 2;
	else if (player->hitW > 0)
		player->hitW--;
	else
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
						
						player->hookPosition = (Vector2){ei->rect.x+ei->rect.width, p->y+16};
						

						
						player->hookDistance = sqrt(pow(fabs(player->position.x+OFFSET+10) - fabs(player->hookPosition.x), 2) + pow(fabs(player->position.y+16) - fabs(player->hookPosition.y), 2));
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

						player->hookPosition = (Vector2){ei->rect.x, p->y+16};
						player->hookDistance = sqrt(pow(fabs(player->position.x+OFFSET+10) - fabs(player->hookPosition.x), 2) + pow(fabs(player->position.y+16) - fabs(player->hookPosition.y), 2));
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
						player->state = 0;
					else if (player->state == 1)
						player->state = 2;
						
					player->hookPosition = (Vector2){p->x+OFFSET+10, ei->rect.y+ei->rect.height};
					
					player->hookDistance = sqrt(pow(fabs(player->position.x+OFFSET+10) - fabs(player->hookPosition.x), 2) + pow(fabs(player->position.y+16) - fabs(player->hookPosition.y), 2));
				}
			}
			break;
			
			case 'B':
			break;
			
			case 'S':
			if (CCUpDown(player, ei, OFFSET+2) && !player->hitObstacleD){
				Die(player, camera);
				i = envItemsLength;
			}
			
			if (CCDownUp(player, ei, OFFSET+2)){
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
			
			case 's':
			
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
						
						player->hookPosition = (Vector2){ei->rect.x+ei->rect.width, p->y+16};
						

						
						player->hookDistance = sqrt(pow(fabs(player->position.x+OFFSET+10) - fabs(player->hookPosition.x), 2) + pow(fabs(player->position.y+16) - fabs(player->hookPosition.y), 2));
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

						player->hookPosition = (Vector2){ei->rect.x, p->y+16};
						player->hookDistance = sqrt(pow(fabs(player->position.x+OFFSET+10) - fabs(player->hookPosition.x), 2) + pow(fabs(player->position.y+16) - fabs(player->hookPosition.y), 2));
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
						player->state = 0;
					else if (player->state == 1)
						player->state = 2;
						
					player->hookPosition = (Vector2){p->x+OFFSET+10, ei->rect.y+ei->rect.height};
					
					player->hookDistance = sqrt(pow(fabs(player->position.x+OFFSET+10) - fabs(player->hookPosition.x), 2) + pow(fabs(player->position.y+16) - fabs(player->hookPosition.y), 2));
				}
			}
			
			if (CCUpDown(player, ei, OFFSET+3) && player->mode)
				player->hitObstacleD = i+1;
			
			if (CCDownUp(player, ei, OFFSET+3) && player->mode)
				player->hitObstacleU = i+1;
			
			if (CCLeftRight(player, ei, OFFSET) && player->mode)
				player->hitObstacleR = i+1;
			
			if (CCRightLeft(player, ei, OFFSET) && player->mode)
				player->hitObstacleL = i+1;

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
	left = 0;
	right = 0;
	down = 0;
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