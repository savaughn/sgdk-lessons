#include <genesis.h>
#include "resources.h"
#include "context.h"
#include "player.h"
#include "input.h"

#include "level_data/level_1.h"

#include "sgp.h"

Sprite *frog, *sonic;
SGP sgp;

static GameContext *ctx = NULL;
GameContext *getGameContext(void)
{
	return ctx;
}

static int check_up = 0;
static int check_down = 0;
static int check_left = 0;
static int check_right = 0;

typedef struct
{
	u16 length;
	const u8 *collision_data;
} SGPLevelCollisionData;

static s16 current_level_index = 0;

const SGPLevelCollisionData *levels[1] = {
	&(SGPLevelCollisionData){
		.length = level_1_map_collision_length,
		.collision_data = level_1_map_collision}};

typedef enum
{
	direction_up = 1,
	direction_down = 2,
	direction_left = 4,
	direction_right = 8
} Direction;

/**
 * Checks for player collision with the level tiles using look-ahead logic.
 * Returns TRUE if a collision is detected in the specified direction.
 */
static bool SGP_PlayerLevelCollision(s16 player_x, s16 player_y, s16 player_width, s16 player_height, SGPLevelCollisionData *level, u8 direction)
{
	s16 tile_x_left;
	s16 tile_x_right;
	s16 tile_y_top = player_y >> 4;
	s16 tile_y_bottom = (player_y + player_height - 1) >> 4;

	u16 arr_ind_top_left;
	u16 arr_ind_top_right;
	u16 arr_ind_bottom_left;
	u16 arr_ind_bottom_right;

	u8 type_topleft, type_topright, type_bottom_left, type_bottom_right;

	if (direction & direction_up) // UP
	{
		tile_y_top = (player_y - 1) >> 4;
		tile_x_left = (player_x + 1) >> 4;
		arr_ind_top_left = tile_x_left + (tile_y_top * level->length);
		type_topleft = level->collision_data[arr_ind_top_left];

		tile_x_right = ((player_x + player_width - 1) >> 4);
		arr_ind_top_right = tile_x_right + (tile_y_top * level->length);
		type_topright = level->collision_data[arr_ind_top_right];

		return (type_topleft == SOLID_TILE || type_topright == SOLID_TILE);
	}
	else if (direction & direction_down) // DOWN
	{
		tile_y_bottom = (player_y + player_height) >> 4;
		tile_x_left = (player_x + 1) >> 4;
		arr_ind_bottom_left = tile_x_left + (tile_y_bottom * level->length);
		type_bottom_left = level->collision_data[arr_ind_bottom_left];

		tile_x_right = ((player_x + player_width - 1) >> 4);
		arr_ind_bottom_right = tile_x_right + (tile_y_bottom * level->length);
		type_bottom_right = level->collision_data[arr_ind_bottom_right];

		return (type_bottom_left == SOLID_TILE || type_bottom_right == SOLID_TILE);
	}
	if (direction & direction_left) // LEFT
	{
		// check the x - 1 position moving left
		tile_x_left = (player_x - 1) >> 4;
		arr_ind_top_left = tile_x_left + (tile_y_top * level->length);
		type_topleft = level->collision_data[arr_ind_top_left];

		arr_ind_bottom_left = tile_x_left + (tile_y_bottom * level->length);
		type_bottom_left = level->collision_data[arr_ind_bottom_left];

		return (type_topleft == SOLID_TILE || type_bottom_left == SOLID_TILE);
	}
	else if (direction & direction_right) // RIGHT
	{
		// check the x + 1 position moving right
		tile_x_right = (player_x + player_width + 1) >> 4;
		arr_ind_top_right = tile_x_right + (tile_y_top * level->length);
		arr_ind_bottom_right = tile_x_right + (tile_y_bottom * level->length);
		type_topright = level->collision_data[arr_ind_top_right];
		type_bottom_right = level->collision_data[arr_ind_bottom_right];

		return (type_topright == SOLID_TILE || type_bottom_right == SOLID_TILE);
	}

	return FALSE; // No collision detected
}

static void walk(Player *player)
{
	if (player->index == JOY_1)
	{
		if (player->sprite->animInd != ANIM_WALK)
		{
			SPR_setAnim(player->sprite, ANIM_WALK);
		}
	}
	else if (player->index == JOY_2)
	{
		if (player->sprite->animInd != SONIC_WALK)
		{
			SPR_setAnim(player->sprite, SONIC_WALK);
		}
	}
	player->frameCounter = 0;
	player->can_idle = TRUE;
}

static inline bool isPlayerInNewTile(Player *player, s16 new_tile_x, s16 new_tile_y)
{
	return (new_tile_x != player->prev_tile_x || new_tile_y != player->prev_tile_y);
}
static bool is_colliding_down = FALSE;
static bool is_colliding_up = FALSE;
static bool is_colliding_left = FALSE;
static bool is_colliding_right = FALSE;
static void animatePlayer(Player *player, u16 joyState)
{

	switch (player->state)
	{
	case STATE_WALK:
		if (SGP_ButtonDown(player->index, BUTTON_UP))
		{
			// About to enter a new tile, check collision at the next position
			if (!SGP_PlayerLevelCollision(
					F32_toInt(player->x),
					F32_toInt(player->y),
					player->width,
					player->height,
					levels[current_level_index],
					direction_up))
			{
				player->y -= WALK_SPEED;
				is_colliding_up = FALSE;
			}
			else
			{
				is_colliding_up = TRUE;
			}
		}
		else if (SGP_ButtonDown(player->index, BUTTON_DOWN))
		{
			// About to enter a new tile, check collision at the next position
			if (!SGP_PlayerLevelCollision(
					F32_toInt(player->x),
					F32_toInt(player->y),
					player->width,
					player->height,
					levels[current_level_index],
					direction_down))
			{
				player->y += WALK_SPEED;
				is_colliding_down = FALSE;
			}
			else
			{
				is_colliding_down = TRUE;
			}
		}
		if (SGP_ButtonDown(player->index, BUTTON_LEFT))
		{
			// About to enter a new tile, check collision at the next position
			if (!SGP_PlayerLevelCollision(
					F32_toInt(player->x),
					F32_toInt(player->y),
					player->width,
					player->height,
					levels[current_level_index],
					direction_left))
			{
				player->x -= WALK_SPEED;
				SPR_setHFlip(player->sprite, TRUE);
				is_colliding_left = FALSE;
			}
			else
			{
				is_colliding_left = TRUE;
			}
		}
		else if (SGP_ButtonDown(player->index, BUTTON_RIGHT))
		{
			if (!SGP_PlayerLevelCollision(
					F32_toInt(player->x),
					F32_toInt(player->y),
					player->width,
					player->height,
					levels[current_level_index],
					direction_right))
			{
				player->x += WALK_SPEED;
				SPR_setHFlip(player->sprite, FALSE);
				is_colliding_right = FALSE;
			}
			else
			{
				is_colliding_right = TRUE;
				// player->x -= FIX32(1.1); 
			}
		}
		walk(player);
		break;
	case STATE_LOOK:
		if (player->index == JOY_1)
		{
			SPR_setAnim(player->sprite, ANIM_LOOK);
		}
		else if (player->index == JOY_2)
		{
			SPR_setAnim(player->sprite, SONIC_LOOK);
		}
		player->frameCounter = 0;
		player->can_idle = FALSE;
		break;
	case STATE_IDLE:
		if (player->index == JOY_1)
		{
			SPR_setAnim(player->sprite, ANIM_IDLE);
		}
		else if (player->index == JOY_2)
		{
			SPR_setAnim(player->sprite, SONIC_IDLE);
		}
		player->frameCounter++;
		player->can_idle = TRUE;
		is_colliding_up = FALSE;
		is_colliding_down = FALSE;
		is_colliding_left = FALSE;
		is_colliding_right = FALSE;
		break;
	case STATE_JUMP:
		if (player->index == JOY_1)
		{
			SPR_setAnimAndFrame(player->sprite, ANIM_JUMP, 0);
		}
		else if (player->index == JOY_2)
		{
			SPR_setAnim(player->sprite, SONIC_JUMP);
		}
		player->frameCounter = 0;
		player->can_idle = FALSE;
		break;
	case STATE_CROUCH:
		if (player->index == JOY_1)
		{
			SPR_setAnim(player->sprite, ANIM_CROUCH);
		}
		else if (player->index == JOY_2)
		{
			SPR_setAnim(player->sprite, SONIC_CROUCH);
		}
		player->frameCounter = 0;
		player->can_idle = FALSE;
		break;
	}

	// Reset frame counter if done with look state
	if (player->state == STATE_LOOK && player->sprite->animInd == ANIM_LOOK && player->sprite->frameInd == player->sprite->animation->numFrame - 1)
	{
		player->frameCounter = 0;
		player->can_idle = TRUE;
	}

	SGP_ClampPositionToMapBounds(&player->x, &player->y, player->width, player->height);
}

u16 ind = TILE_USER_INDEX;
Map *level_1_map = NULL;
int main(_Bool)
{
	SGP_init();
	VDP_init();
	SPR_init();

	PAL_setPalette(PAL0, background.palette->data, DMA);
	PAL_setPalette(PAL1, fg_palette.data, DMA);
	PAL_setPalette(PAL2, frog_sprite_sheet.palette->data, DMA);

	VDP_loadTileSet(&fg_tileset, ind, DMA);
	level_1_map = MAP_create(&level_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind));
	ind += fg_tileset.numTile;

	VDP_drawImageEx(BG_B, &background, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);

	frog = SPR_addSprite(&frog_sprite_sheet, 0, 0, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));

	static GameContext context;
	ctx = &context;

	ctx->player_1 = (Player){
		.x = FIX32(0),
		.y = FIX32(0),
		.sprite = frog,
		.frameCounter = 0,
		.can_idle = TRUE,
		.state = STATE_IDLE,
		.index = JOY_1,
		.velocity_x = FIX32(1.5),
		.velocity_y = FIX32(0),
		.height = 32,
		.width = 32};

	SPR_setAnim(frog, ANIM_IDLE);

	if (!SGP_CameraInit(level_1_map))
	{
		SGP_HandleError("camera init failed!");
	}

	while (TRUE)
	{
		static int map_x = 0, map_y = 0;

		SGP_PollInput();

		handleInput(&ctx->player_1, sgp.input.joy1_state, JOY_1);

		animatePlayer(&ctx->player_1, sgp.input.joy1_state);
		SGP_CameraFollowTarget(
			&(SGPCameraTarget){
				.sprite = ctx->player_1.sprite,
				.target_x_ptr = &ctx->player_1.x,
				.target_y_ptr = &ctx->player_1.y,
				.sprite_width = ctx->player_1.width,
				.sprite_height = ctx->player_1.height,
			});

		if (!SGP_isCameraActive())
		{
			SPR_setPosition(ctx->player_1.sprite,
							F32_toInt(ctx->player_1.x),
							F32_toInt(ctx->player_1.y));
		}

		SPR_update();
		SYS_doVBlankProcess();
#ifdef DEBUG
		// /********DEBUG PRINT**********/
		char buffer[64];
		sprintf(buffer, "p_pos (%d, %d)\n", F32_toInt(ctx->player_1.x), F32_toInt(ctx->player_1.y));
		SGP_DebugPrint(buffer, 0, 0);

		// char buffer2[64];
		// sprintf(buffer2, "c_pos (%d, %d)\n", sgp.camera.current_x, sgp.camera.current_y);
		// SGP_DebugPrint(buffer2, 0, 1);

		char buffer3[64];
		sprintf(buffer3, "chk u%d d%d l%d r%d", check_up, check_down, check_left, check_right);
		SGP_DebugPrint(buffer3, 0, 1);

		char buffer4[64];
		sprintf(buffer4, "u%d, d%d, l%d, r%d", is_colliding_up, is_colliding_down, is_colliding_left, is_colliding_right);
		SGP_DebugPrint(buffer4, 0, 2);
		// /********DEBUG PRINT**********/

		if (SGP_ButtonDown(JOY_1, BUTTON_MODE))
		{
			SGP_ToggleDebug();
			if (SGP_isCameraActive())
			{
				SGP_ShakeCamera(10, 2); // Shake camera for 10 frames with intensity 5
			}
			else
			{
				SGP_activateCamera();
			}
		}
#endif
	}
	return 0;
}

// 	JOY_setEventHandler(&joyEvent);

// PAL_setPalette(PAL3, sonic_image.palette->data, DMA);
// sonic = SPR_addSprite(&sonic_image, FIX32(0), FIX32(0), TILE_ATTR(PAL3, FALSE, FALSE, TRUE));

// ctx->player_2 = (Player){
// 	.x = FIX32(SCREEN_WIDTH - 64),
// 	.y = FIX32(SCREEN_HEIGHT / 2),
// 	.sprite = sonic,
// 	.frameCounter = 0,
// 	.can_idle = TRUE,
// 	.state = STATE_IDLE,
// 	.index = JOY_2,
// 	.velocity_x = FIX32(0),
// 	.velocity_y = FIX32(0)
// };

// SPR_setAnim(sonic, ANIM_IDLE);
// SPR_setVisibility(sonic, HIDDEN);
// static bool player_2_joined = FALSE;

// Use SGP_ButtonPressed for player 2 join/leave
// if (SGP_ButtonPressed(JOY_2, BUTTON_START)) {
// 	player_2_joined = !player_2_joined;
// 	SPR_setVisibility(ctx->player_2.sprite, player_2_joined ? VISIBLE : HIDDEN);
// }
// if (player_2_joined) handleInput(&ctx->player_2, sgp.input.joy2_state, JOY_2);
// animatePlayer(&ctx->player_1, sgp.input.joy1_state);
// if (player_2_joined) animatePlayer(&ctx->player_2, sgp.input.joy2_state);
