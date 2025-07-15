#include <genesis.h>
#include "resources.h"

int main() {

	u16 index = TILE_USER_INDEX;
	PAL_setPalette(PAL0, background1.palette->data, DMA);

	VDP_drawImageEx(
		BG_B,
		&background1,
		TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, index),
		0,
		0,
		FALSE,
		TRUE
	);

	index += background1.tileset->numTile;
	PAL_setPalette(PAL1, foreground1.palette->data, DMA);

	VDP_drawImageEx(
		BG_A,
		&foreground1,
		TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, index),
		0,
		0,
		FALSE,
		TRUE
	);
	index += foreground1.tileset->numTile;

	while(TRUE) {
		static int hscroll_offset = 0;

		VDP_setHorizontalScroll(BG_B, hscroll_offset);
		hscroll_offset -= 1;

		VDP_waitVSync();
	}
	return 0;
}
