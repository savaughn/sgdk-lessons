#include <genesis.h>
#include "resources.h"

int main() {

	u16 index = TILE_USER_INDEX;
	PAL_setPalette(PAL0, background.palette->data, DMA);

	VDP_drawImageEx(
		BG_B,
		&background,
		TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, index),
		0,
		-4,
		FALSE,
		TRUE
	);

	index += background.tileset->numTile;
	PAL_setPalette(PAL1, foreground.palette->data, DMA);

	VDP_drawImageEx(
		BG_A,
		&foreground,
		TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, index),
		0,
		-4,
		FALSE,
		TRUE
	);
	index += foreground.tileset->numTile;
	VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

	while(TRUE) {
		static float hscroll_offset = 0.0f;
		static float vscroll_offset = 0.0f;

		VDP_setHorizontalScroll(BG_B, hscroll_offset);
		hscroll_offset -= 0.25f;

		VDP_setHorizontalScroll(BG_A, vscroll_offset);
		vscroll_offset -= 0.75f;

		VDP_waitVSync();
	}
	return 0;
}
