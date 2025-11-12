#ifndef BOARD_H
#define BOARD_H

#include <iostream>

const int N = 10;

enum Color
{
    VACIO = 0,
    BLANCO = 1,
    NEGRO = 2
};

struct Pos
{
    int r, c;
};

struct Pieza
{
    Color color;
    bool dama;
};

struct Move
{
    Pos from;
    Pos to;
    Pos captured[20];
    int capCount;
    Move()
    {
        capCount = 0;
        from = {-1, -1};
        to = {-1, -1};
    }
};

void initBoard(Pieza board[N][N]);
void printBoard(const Pieza board[N][N]);
bool inside(int r, int c);
bool isDark(int r, int c);
int countPieces(const Pieza board[N][N], Color who);
int generateSimples(const Pieza board[N][N], Color who, Move out[], int maxOut);
int generateCaptures(const Pieza board[N][N], Color who, Move out[], int maxOut);
bool applyMove(Pieza board[N][N], const Move &m);

#endif
