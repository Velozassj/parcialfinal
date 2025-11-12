#include "board.h"
#include "ai.h"
#include <iostream>
#include <string>
#include <cctype>

static bool parseCoord(const std::string &s, Pos &out)
{
    if (s.size() < 2 || s.size() > 3)
        return false;
    char col = std::toupper(static_cast<unsigned char>(s[0]));
    if (col < 'A' || col > 'J')
        return false;
    std::string num = s.substr(1);
    for (char ch : num)
        if (!std::isdigit(static_cast<unsigned char>(ch)))
            return false;
    int row = std::stoi(num);
    if (row < 1 || row > 10)
        return false;
    out.c = col - 'A';
    out.r = 10 - row;
    return true;
}

int main()
{
    Pieza board[N][N];
    initBoard(board);
    initAI();

    std::cout << "=== DAMAS INTERNACIONALES ===\n";
    std::cout << "1) Vs IA\n2) 2 jugadores\n0) Salir\nElija opcion: ";
    int opt;
    if (!(std::cin >> opt))
        return 0;
    if (opt == 0)
        return 0;
    bool vsAI = (opt == 1);

    Color turno = BLANCO;
    std::string linea;
    std::getline(std::cin, linea);

    while (true)
    {
        printBoard(board);
        std::cout << "Turno: " << (turno == BLANCO ? "Blancas (w)" : "Negras (b)") << "\n";

        if (countPieces(board, BLANCO) == 0)
        {
            std::cout << "Negras ganan!\n";
            break;
        }
        if (countPieces(board, NEGRO) == 0)
        {
            std::cout << "Blancas ganan!\n";
            break;
        }

        Move tmp[500];
        int capCount = generateCaptures(board, turno, tmp, 500);
        int simCount = generateSimples(board, turno, tmp, 500);
        if (capCount == 0 && simCount == 0)
        {
            std::cout << (turno == BLANCO ? "Negras" : "Blancas") << " ganan (oponente sin movimientos)!\n";
            break;
        }

        Move chosen;
        bool moved = false;

        if (vsAI && turno == NEGRO)
        {
            std::cout << "IA pensando...\n";
            chosen = aiMove(board, turno);
            if (applyMove(board, chosen))
                moved = true;
            else
            {
                std::cout << "IA no pudo mover.\n";
                break;
            }
        }
        else
        {
            // jugador humano
            if (capCount > 0)
            {
                std::cout << "Capturas obligatorias disponibles (escoja numero):\n";
                for (int i = 0; i < capCount; ++i)
                {
                    Move &m = tmp[i];
                    char fromC = 'A' + m.from.c;
                    int fromR = 10 - m.from.r;
                    char toC = 'A' + m.to.c;
                    int toR = 10 - m.to.r;
                    std::cout << i << ") " << fromC << fromR << " -> " << toC << toR << " (cap: " << m.capCount << ")\n";
                }
                int choice;
                std::cout << "Elija indice: ";
                if (!(std::cin >> choice) || choice < 0 || choice >= capCount)
                {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "Opcion invalida.\n";
                    continue;
                }
                chosen = tmp[choice];
                if (applyMove(board, chosen))
                    moved = true;
            }
            else
            {
                simCount = generateSimples(board, turno, tmp, 500);
                std::cout << "Movimientos disponibles (escoja numero):\n";
                for (int i = 0; i < simCount; ++i)
                {
                    Move &m = tmp[i];
                    char fromC = 'A' + m.from.c;
                    int fromR = 10 - m.from.r;
                    char toC = 'A' + m.to.c;
                    int toR = 10 - m.to.r;
                    std::cout << i << ") " << fromC << fromR << " -> " << toC << toR << "\n";
                }
                int choice;
                std::cout << "Elija indice: ";
                if (!(std::cin >> choice) || choice < 0 || choice >= simCount)
                {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "Opcion invalida.\n";
                    continue;
                }
                chosen = tmp[choice];
                if (applyMove(board, chosen))
                    moved = true;
            }
        }

        if (!moved)
        {
            std::cout << "No se aplico movimiento. Intente de nuevo.\n";
            continue;
        }

        // alternar turno
        turno = (turno == BLANCO ? NEGRO : BLANCO);
    }

    std::cout << "Fin de la partida.\n";
    return 0;
}
