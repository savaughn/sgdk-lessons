#include <genesis.h>
#include "resources.h"
#include "context.h"
#include "player.h"
#include "input.h"

#include "sgp.h"

Sprite* frog, *sonic;
SGP sgp;

static GameContext* ctx = NULL;
GameContext* getGameContext(void) {
	return ctx;
}

static void walk(Player* player) {
	if (player->index == JOY_1) {
		if (player->sprite->animInd != ANIM_WALK) {
			SPR_setAnim(player->sprite, ANIM_WALK);
		}
	} else if (player->index == JOY_2) {
		if (player->sprite->animInd != SONIC_WALK) {
			SPR_setAnim(player->sprite, SONIC_WALK);
		}
	}
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
			if (player->index == JOY_1) {
				SPR_setAnim(player->sprite, ANIM_LOOK);
			} else if (player->index == JOY_2) {
				SPR_setAnim(player->sprite, SONIC_LOOK);
			}
			player->frameCounter = 0;
			player->can_idle = FALSE;
			break;
		case STATE_IDLE:
			if (player->index == JOY_1) {
				SPR_setAnim(player->sprite, ANIM_IDLE);
			} else if (player->index == JOY_2) {
				SPR_setAnim(player->sprite, SONIC_IDLE);
			}
			player->frameCounter++;
			player->can_idle = TRUE;
			break;
		case STATE_JUMP:
			if (player->index == JOY_1) {
				SPR_setAnimAndFrame(player->sprite, ANIM_JUMP, 0);
			} else if (player->index == JOY_2) {
				SPR_setAnim(player->sprite, SONIC_JUMP);
			}
			player->frameCounter = 0;
			player->can_idle = FALSE;
			break;
		case STATE_CROUCH:
			if (player->index == JOY_1) {
				SPR_setAnim(player->sprite, ANIM_CROUCH);
			} else if (player->index == JOY_2) {
				SPR_setAnim(player->sprite, SONIC_CROUCH);
			}
			player->frameCounter = 0;
			player->can_idle = FALSE;
			break;
	}

	// Reset frame counter if done with look state
	if (player->state == STATE_LOOK && player->sprite->animInd == ANIM_LOOK && player->sprite->frameInd == player->sprite->animation->numFrame - 1) {
		player->frameCounter = 0;
		player->can_idle = TRUE;
	}

	SPR_setPosition(player->sprite, player->x, player->y);
}

u16 ind = TILE_USER_INDEX;
Map* level_1_map = NULL;

int main() {
	SGP_init();
	VDP_init();

	VDP_loadTileSet(&tileset, ind, DMA);
	level_1_map = MAP_create(&our_level_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind));
	PAL_setPalette(PAL0, our_palette.data, DMA);

	SPR_init();
	PAL_setPalette(PAL2, frog_sprite_sheet.palette->data, DMA);
	PAL_setPalette(PAL3, sonic_image.palette->data, DMA);
	frog = SPR_addSprite(&frog_sprite_sheet, 0, 0, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	sonic = SPR_addSprite(&sonic_image, 0, 0, TILE_ATTR(PAL3, FALSE, FALSE, TRUE));

	static GameContext context;
	ctx = &context;

	ctx->player_1 = (Player){
		.x = 64,
		.y = SCREEN_HEIGHT / 2,
		.sprite = frog,
		.frameCounter = 0,
		.can_idle = TRUE,
		.state = STATE_IDLE,
		.index = JOY_1
	};

	ctx->player_2 = (Player){
		.x = SCREEN_WIDTH - 64,
		.y = SCREEN_HEIGHT / 2,
		.sprite = sonic,
		.frameCounter = 0,
		.can_idle = TRUE,
		.state = STATE_IDLE,
		.index = JOY_2
	};

	SPR_setAnim(frog, ANIM_IDLE);
	SPR_setAnim(sonic, ANIM_IDLE);
	SPR_setVisibility(sonic, HIDDEN);

	JOY_setEventHandler(&joyEvent);

	static bool player_2_joined = FALSE;

	while(TRUE) {
		static u16 mapScrollX = 0;
		MAP_scrollTo(level_1_map, mapScrollX, 768);
		mapScrollX += 2;

		SGP_PollInput();
		handleInput(&ctx->player_1, sgp.input.joy1_state, JOY_1);

		// Use SGP_ButtonPressed for player 2 join/leave
		if (SGP_ButtonPressed(JOY_2, BUTTON_START)) {
			player_2_joined = !player_2_joined;
			SPR_setVisibility(ctx->player_2.sprite, player_2_joined ? VISIBLE : HIDDEN);
		}
		if (player_2_joined) handleInput(&ctx->player_2, sgp.input.joy2_state, JOY_2);
		animatePlayer(&ctx->player_1, sgp.input.joy1_state);
		if (player_2_joined) animatePlayer(&ctx->player_2, sgp.input.joy2_state);

		SPR_update();

		SYS_doVBlankProcess();
	}
	return 0;
}
