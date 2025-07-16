#include <genesis.h>
#include "resources.h"

#define ANIM_IDLE 0
#define ANIM_WALK 1
#define ANIM_LOOK 2
#define ANIM_JUMP 3
#define ANIM_CROUCH 4
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 224

Sprite* frog;

int main() {

	SPR_init();
	PAL_setPalette(PAL1, frog_sprite_sheet.palette->data, DMA);
	frog = SPR_addSprite(&frog_sprite_sheet, SCREEN_WIDTH - 40, SCREEN_HEIGHT - 40, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
	SPR_setAnim(frog, ANIM_IDLE);

	while(TRUE) {
		SPR_update();

		static int frameCounter = 0;
		frameCounter++;
		if (frameCounter % 150 == 0) { 
			if (frog->animInd == ANIM_IDLE || frog->animInd == ANIM_LOOK) {
				SPR_setAnim(frog, ANIM_WALK);
			} else {
				SPR_setAnim(frog, ANIM_IDLE);
			}
		}
		if (frameCounter % 300 == 0) {
			if (frog->animInd == ANIM_WALK) {
				SPR_setAnim(frog, ANIM_LOOK);
			} else {
				SPR_setAnim(frog, ANIM_WALK);
			}
		}

		if (frameCounter > 900) {
			frameCounter = 0;
		}

		SYS_doVBlankProcess();
	}
	return 0;
}
