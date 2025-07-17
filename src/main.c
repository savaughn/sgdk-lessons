#include <genesis.h>
#include "resources.h"

#define ANIM_IDLE 0
#define ANIM_WALK 1
#define ANIM_LOOK 2
#define ANIM_JUMP 3
#define ANIM_CROUCH 4
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 224
#define WALK_SPEED 2

Sprite* frog, *frog2;

typedef enum {
	STATE_IDLE,
	STATE_WALK,
	STATE_LOOK
} PlayerState;

typedef struct {
	int x;
	int y;
	Sprite* sprite;
	int frameCounter;
	bool can_idle;
	PlayerState state;
} Player;

static void walk(Player* player) {
	SPR_setAnim(player->sprite, ANIM_WALK);
	player->frameCounter = 0;
	player->can_idle = TRUE;
}

static void animatePlayer(Player* player, u16 joyState) {
	switch (player->state) {
		case STATE_WALK:
			if (joyState & BUTTON_UP) {
				player->y -= WALK_SPEED;
			} else if (joyState & BUTTON_DOWN) {
				player->y += WALK_SPEED;
			}
			if (joyState & BUTTON_LEFT) {
				player->x -= WALK_SPEED;
				SPR_setHFlip(player->sprite, TRUE);
			} else if (joyState & BUTTON_RIGHT) {
				player->x += WALK_SPEED;
				SPR_setHFlip(player->sprite, FALSE);
			}
			walk(player);
			break;
		case STATE_LOOK:
			SPR_setAnim(player->sprite, ANIM_LOOK);
			player->frameCounter = 0;
			player->can_idle = FALSE;
			break;
		case STATE_IDLE:
			SPR_setAnim(player->sprite, ANIM_IDLE);
			player->frameCounter++;
			player->can_idle = TRUE;
			break;
	}

	// Reset frame counter if done with look state
	if (player->state == STATE_LOOK && player->sprite->animInd == ANIM_LOOK && player->sprite->frameInd == player->sprite->animation->numFrame - 1) {
		player->frameCounter = 0;
		player->can_idle = TRUE;
	}

	SPR_setPosition(player->sprite, player->x, player->y);

}

static void handleInput(Player* player, u16* joyState, u16 joy) {
	*joyState = JOY_readJoypad(joy);
	// Determine state
	if (*joyState & (BUTTON_UP | BUTTON_DOWN | BUTTON_LEFT | BUTTON_RIGHT)) {
		player->state = STATE_WALK;
	} else if (player->sprite->animInd == ANIM_IDLE && player->frameCounter > 240) {
		player->state = STATE_LOOK;
	} else if (player->sprite->animInd == ANIM_LOOK && player->sprite->frameInd == player->sprite->animation->numFrame - 1) {
		player->state = STATE_IDLE;
	} else if (player->can_idle) {
		player->state = STATE_IDLE;
	}
}

int main() {

	SPR_init();
	PAL_setPalette(PAL1, frog_sprite_sheet.palette->data, DMA);
	frog = SPR_addSprite(&frog_sprite_sheet, 0, 0, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
	frog2 = SPR_addSprite(&frog_sprite_sheet, 0, 0, TILE_ATTR(PAL1, FALSE, FALSE, TRUE));


	Player player_1 = {
		.x = 64,
		.y = SCREEN_HEIGHT / 2,
		.sprite = frog,
		.frameCounter = 0,
		.can_idle = TRUE,
		.state = STATE_IDLE
	};

	Player player_2 = {
		.x = SCREEN_WIDTH - 64,
		.y = SCREEN_HEIGHT / 2,
		.sprite = frog2,
		.frameCounter = 0,
		.can_idle = TRUE,
		.state = STATE_IDLE
	};
	SPR_setAnim(frog, ANIM_IDLE);
	SPR_setAnim(frog2, ANIM_IDLE);
	SPR_setVisibility(frog2, HIDDEN);


	static bool player_2_joined = FALSE;

	while(TRUE) {
		u16 joyState_1 = 0;
		u16 joyState_2 = 0;
		handleInput(&player_1, &joyState_1, JOY_1);

		if ((JOY_readJoypad(JOY_2) & BUTTON_START)) {
			player_2_joined = !player_2_joined;
			SPR_setVisibility(frog2, player_2_joined ? VISIBLE : HIDDEN);
		}
		if (player_2_joined) handleInput(&player_2, &joyState_2, JOY_2);
		animatePlayer(&player_1, joyState_1);
		if (player_2_joined) animatePlayer(&player_2, joyState_2);

		SPR_update();

		SYS_doVBlankProcess();
	}
	return 0;
}
