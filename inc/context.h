#ifndef CONTEXT_H
#define CONTEXT_H

#include "player.h"

typedef struct {
	Player player_1;
	Player player_2;
} GameContext;

GameContext* getGameContext(void);

#endif // CONTEXT_H
