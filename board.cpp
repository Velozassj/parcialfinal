#include "board.h"
#include <iomanip> // formato salida
#include <cstring> // cadenas / std::memset
#include <cstdlib> // funciones / std::abs

// tablero
void initBoard(Pieza board[N][N])
{
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            board[i][j] = {VACIO, false};

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < N; ++j)
            if ((i + j) % 2 == 1)
                board[i][j].color = NEGRO;

    for (int i = 6; i < 10; ++i)
        for (int j = 0; j < N; ++j)
            if ((i + j) % 2 == 1)
                board[i][j].color = BLANCO;
}

bool inside(int r, int c) { return r >= 0 && r < N && c >= 0 && c < N; }
bool isDark(int r, int c) { return ((r + c) % 2) != 0; }

void printBoard(const Pieza board[N][N])
{
    std::cout << "\n   A B C D E F G H I J\n";
    for (int i = 0; i < N; ++i)
    {
        std::cout << std::setw(2) << (10 - i) << ' ';
        for (int j = 0; j < N; ++j)
        {
            if (!isDark(i, j))
                std::cout << ". ";
            else
            {
                if (board[i][j].color == VACIO)
                    std::cout << "- ";
                else if (board[i][j].color == BLANCO)
                    std::cout << (board[i][j].dama ? 'W' : 'w') << ' ';
                else
                    std::cout << (board[i][j].dama ? 'B' : 'b') << ' ';
            }
        }
        std::cout << (10 - i) << '\n';
    }
    std::cout << "   A B C D E F G H I J\n";
}

int countPieces(const Pieza board[N][N], Color who)
{
    int total = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (board[i][j].color == who)
                ++total;
    return total;
}

static void pushMove(Move out[], int &count, int maxOut, const Move &m)
{
    if (count < maxOut)
        out[count++] = m;
}

int generateSimples(const Pieza board[N][N], Color who, Move out[], int maxOut)
{
    int total = 0;
    for (int r = 0; r < N; ++r)
    {
        for (int c = 0; c < N; ++c)
        {
            const Pieza &p = board[r][c];
            if (p.color != who)
                continue;
            if (!p.dama)
            {
                int dir = (who == BLANCO ? -1 : 1);
                for (int dc = -1; dc <= 1; dc += 2)
                {
                    int nr = r + dir, nc = c + dc;
                    if (inside(nr, nc) && isDark(nr, nc) && board[nr][nc].color == VACIO)
                    {
                        Move m;
                        m.from = {r, c};
                        m.to = {nr, nc};
                        m.capCount = 0;
                        pushMove(out, total, maxOut, m);
                    }
                }
            }
            else
            {
                int drs[4] = {1, 1, -1, -1};
                int dcs[4] = {1, -1, 1, -1};
                for (int k = 0; k < 4; ++k)
                {
                    int nr = r + drs[k], nc = c + dcs[k];
                    while (inside(nr, nc) && isDark(nr, nc) && board[nr][nc].color == VACIO)
                    {
                        Move m;
                        m.from = {r, c};
                        m.to = {nr, nc};
                        m.capCount = 0;
                        pushMove(out, total, maxOut, m);
                        nr += drs[k];
                        nc += dcs[k];
                    }
                }
            }
        }
    }
    return total;
}

// capturas
static void dfsManCaptures(const Pieza board[N][N], int r, int c, bool visited[10][10],
                           Move &cur, Move results[], int &resCount, int &best)
{
    int dr[4] = {1, 1, -1, -1};
    int dc[4] = {1, -1, 1, -1};
    bool foundAny = false;
    for (int k = 0; k < 4; ++k)
    {
        int mr = r + dr[k], mc = c + dc[k];
        int lr = r + 2 * dr[k], lc = c + 2 * dc[k];
        if (!inside(mr, mc) || !inside(lr, lc))
            continue;
        if (!isDark(mr, mc) || !isDark(lr, lc))
            continue;
        if (board[mr][mc].color == VACIO)
            continue;
        if (board[mr][mc].color == board[r][c].color)
            continue;
        if (visited[mr][mc])
            continue;
        if (board[lr][lc].color != VACIO)
            continue;
        visited[mr][mc] = true;
        cur.captured[cur.capCount] = {mr, mc};
        cur.capCount++;
        Pos oldTo = cur.to;
        cur.to = {lr, lc};
        dfsManCaptures(board, lr, lc, visited, cur, results, resCount, best);
        if (cur.capCount > 0)
        {
            if (cur.capCount > best)
            {
                best = cur.capCount;
                resCount = 0;
            }
            if (cur.capCount == best)
            {
                if (resCount < 500)
                    results[resCount++] = cur;
            }
        }
        cur.capCount--;
        cur.to = oldTo;
        visited[mr][mc] = false;
        foundAny = true;
    }
    if (!foundAny)
        return;
}

// damas voladoras
static void dfsKingCaptures(const Pieza board[N][N], int r, int c, bool visited[10][10],
                            Move &cur, Move results[], int &resCount, int &best)
{
    int dr[4] = {1, 1, -1, -1};
    int dc[4] = {1, -1, 1, -1};
    bool foundAny = false;
    for (int k = 0; k < 4; ++k)
    {
        int rr = r + dr[k], cc = c + dc[k];
        while (inside(rr, cc) && isDark(rr, cc) && board[rr][cc].color == VACIO)
        {
            rr += dr[k];
            cc += dc[k];
        }
        if (!inside(rr, cc) || board[rr][cc].color == VACIO)
            continue;
        if (board[rr][cc].color == board[r][c].color)
            continue;
        int lr = rr + dr[k], lc = cc + dc[k];
        while (inside(lr, lc) && isDark(lr, lc))
        {
            if (board[lr][lc].color != VACIO)
                break;
            if (!visited[rr][cc])
            {
                visited[rr][cc] = true;
                cur.captured[cur.capCount] = {rr, cc};
                cur.capCount++;
                Pos oldTo = cur.to;
                cur.to = {lr, lc};
                dfsKingCaptures(board, lr, lc, visited, cur, results, resCount, best);
                if (cur.capCount > 0)
                {
                    if (cur.capCount > best)
                    {
                        best = cur.capCount;
                        resCount = 0;
                    }
                    if (cur.capCount == best)
                    {
                        if (resCount < 500)
                            results[resCount++] = cur;
                    }
                }
                cur.capCount--;
                cur.to = oldTo;
                visited[rr][cc] = false;
            }
            lr += dr[k];
            lc += dc[k];
        }
        foundAny = true;
    }
    if (!foundAny)
        return;
}

// capturas
int generateCaptures(const Pieza board[N][N], Color who, Move out[], int maxOut)
{
    Move tmpResults[1000];
    int tmpCount = 0;
    int globalBest = 0;

    for (int r = 0; r < N; ++r)
        for (int c = 0; c < N; ++c)
        {
            if (board[r][c].color != who)
                continue;
            bool visited[10][10];
            std::memset(visited, 0, sizeof(visited));
            Move cur;
            cur.from = {r, c};
            cur.to = {r, c};
            cur.capCount = 0;
            int localCount = 0;
            int best = 0;
            if (!board[r][c].dama)
                dfsManCaptures(board, r, c, visited, cur, tmpResults, localCount, best);
            else
                dfsKingCaptures(board, r, c, visited, cur, tmpResults, localCount, best);
            if (best > 0)
            {
                if (best > globalBest)
                {
                    globalBest = best;
                    tmpCount = 0;
                }
            }
        }

    if (globalBest == 0)
        return 0;

    int finalCount = 0;
    for (int r = 0; r < N; ++r)
        for (int c = 0; c < N; ++c)
        {
            if (board[r][c].color != who)
                continue;
            bool visited[10][10];
            std::memset(visited, 0, sizeof(visited));
            Move cur;
            cur.from = {r, c};
            cur.to = {r, c};
            cur.capCount = 0;
            Move localResults[1000];
            int localCount = 0;
            int best = 0;
            if (!board[r][c].dama)
                dfsManCaptures(board, r, c, visited, cur, localResults, localCount, best);
            else
                dfsKingCaptures(board, r, c, visited, cur, localResults, localCount, best);
            for (int i = 0; i < localCount && finalCount < maxOut; ++i)
            {
                if (localResults[i].capCount == globalBest)
                    out[finalCount++] = localResults[i];
            }
        }
    return finalCount;
}

bool applyMove(Pieza board[N][N], const Move &m)
{
    if (!inside(m.from.r, m.from.c) || !inside(m.to.r, m.to.c))
        return false;
    Pieza p = board[m.from.r][m.from.c];
    if (p.color == VACIO)
        return false;

    for (int i = 0; i < m.capCount; ++i)
    {
        int rr = m.captured[i].r, cc = m.captured[i].c;
        if (inside(rr, cc))
        {
            board[rr][cc].color = VACIO;
            board[rr][cc].dama = false;
        }
    }

    board[m.to.r][m.to.c] = board[m.from.r][m.from.c];
    board[m.from.r][m.from.c].color = VACIO;
    board[m.from.r][m.from.c].dama = false;

    if (board[m.to.r][m.to.c].color == BLANCO && m.to.r == 0)
        board[m.to.r][m.to.c].dama = true;
    if (board[m.to.r][m.to.c].color == NEGRO && m.to.r == 9)
        board[m.to.r][m.to.c].dama = true;

    return true;
}
