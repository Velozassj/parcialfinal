#include "ai.h"
#include <cstdlib>
#include <ctime> // manejar tiempo para ia

void initAI()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

Move aiMove(const Pieza board[N][N], Color who)
{
    Move moves[200];
    int numCap = generateCaptures(board, who, moves, 200);
    if (numCap > 0)
    {
        int idx = std::rand() % numCap;
        return moves[idx];
    }
    int numSim = generateSimples(board, who, moves, 200);
    if (numSim > 0)
    {
        int idx = std::rand() % numSim;
        return moves[idx];
    }
    return Move();
}
