#include "tests.h"
#include <iostream>
#include <chrono>

int perft2(Board &board, int depth)
{
    if (depth == 0)
        return 1;

    int nodes = 0;
    MoveList legalMoves = generateLegalMoves(board);

    for (Move m : legalMoves)
    {
        board.makeMove(m);
        nodes += perft2(board, depth - 1);
        board.unMakeMove();
    }

    return nodes;
}

int perft(Board &board, int depth, bool display)
{
    auto start = std::chrono::steady_clock::now();
    int totalNodes = 0;
    MoveList legalMoves = generateOrderedMoves(board);

    for (Move m : legalMoves)
    {
        board.makeMove(m);
        int nodes = perft2(board, depth - 1);
        board.unMakeMove();

        if (display) {
            std::cout << board.toString(m) << ": " << nodes << std::endl;
        }
        totalNodes += nodes;
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    if (display) {
        std::cout << totalNodes << " nodes, " << elapsed_seconds.count() << " seconds (" << (totalNodes / elapsed_seconds.count()) << " nodes/second)" << std::endl;
    }

    return totalNodes;
}
