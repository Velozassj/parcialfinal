#ifndef AI_H
#define AI_H

#include "board.h"

void initAI();
Move aiMove(const Pieza board[N][N], Color who);

#endif
