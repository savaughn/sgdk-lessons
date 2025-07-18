#ifndef PLAYER_H
#define PLAYER_H

#include <genesis.h>

#define ANIM_IDLE 0
#define ANIM_WALK 1
#define ANIM_LOOK 2
#define ANIM_JUMP 3
#define ANIM_CROUCH 4
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 224
#define WALK_SPEED 2

#define SONIC_IDLE 0
#define SONIC_LOOK 1
#define SONIC_WALK 2
#define SONIC_CROUCH 6
#define SONIC_JUMP 7

typedef enum {
	STATE_IDLE,
	STATE_WALK,
	STATE_LOOK,
	STATE_JUMP,
	STATE_CROUCH
} PlayerState;

typedef struct {
	int x;
	int y;
	Sprite* sprite;
	int frameCounter;
	bool can_idle;
	PlayerState state;
	u16 index;
} Player;

#endif // PLAYER_H
