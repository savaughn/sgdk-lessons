#ifndef INPUT_H
#define INPUT_H

#include <genesis.h>
#include "player.h"
#include "context.h"

void handleInput(Player* player, u16 joyState, u16 joy);
void joyEvent(u16 joy, u16 changed, u16 state);

#endif
