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
#define WALK_SPEED FIX32(2)

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
	fix32 x;
	fix32 y;
	Sprite* sprite;
	int frameCounter;
	bool can_idle;
	PlayerState state;
	u16 index;
	fix32 velocity_x;
	fix32 velocity_y;
	s16 height;
	s16 width;
} Player;

#endif // PLAYER_H
