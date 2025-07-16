#include <genesis.h>
#include "resources.h"

Sprite* sprite, *hero, *frog;

int main() {

	SPR_init();
	PAL_setPalette(PAL2, sonic_sprite.palette->data, DMA);
	PAL_setPalette(PAL3, my_hero.palette->data, DMA);
	PAL_setPalette(PAL1, frog_sprite.palette->data, DMA);

	sprite = SPR_addSprite(&sonic_sprite, 100, 100, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	hero = SPR_addSprite(&my_hero, 50, 50, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
	frog = SPR_addSprite(&frog_sprite, 150, 150, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));

	while(TRUE) {
		SPR_update();

		SYS_doVBlankProcess();
	}
	return 0;
}
