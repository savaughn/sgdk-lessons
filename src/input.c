#include "input.h"

// Continuous press event handler for joypad input
void handleInput(Player* player, u16* joyState, u16 joy) {
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
	if (*joyState & BUTTON_A) {
		if (joy == JOY_1) {
			player->state = STATE_CROUCH;
			player->can_idle = FALSE;
		} else if (joy == JOY_2) {
			player->state = STATE_CROUCH;
			player->can_idle = FALSE;
		}
	} else {
		if (player->state == STATE_CROUCH) {
			player->state = STATE_IDLE;
			player->can_idle = TRUE;
		}
	}
}

// Single press event handler for joypad input
void joyEvent(u16 joy, u16 changed, u16 state) {
	GameContext* ctx = getGameContext();
	if (changed & state & BUTTON_A) {
		if (joy == JOY_1) {
			ctx->player_1.state = ctx->player_1.state == STATE_CROUCH ? STATE_IDLE : STATE_CROUCH;
			ctx->player_1.can_idle = FALSE;
		} else if (joy == JOY_2) {
			ctx->player_2.state = ctx->player_2.state == STATE_CROUCH ? STATE_IDLE : STATE_CROUCH;
			ctx->player_2.can_idle = FALSE;
		}
	}
	SPR_setPosition(ctx->player_1.sprite, ctx->player_1.x, ctx->player_1.y);
	SPR_setPosition(ctx->player_2.sprite, ctx->player_2.x, ctx->player_2.y);
}
