#include "evaluate.h"
#include <iostream>
#include <chrono>

int searchMoveCount = 0;

int mirror(int sq)
{
    return sq ^ 56; // flips rank
}

int evaluate(const Board &board)
{
    int score = 0;
    searchMoveCount++;

    for (int sq = 0; sq < 64; ++sq)
    {
        const Piece &p = board.pieceAt(sq);
        if (p.type == PieceType::None)
            continue;

        int value = 0;
        int pst = 0;
        int tableSq = (p.color == Color::White) ? sq : mirror(sq);

        switch (p.type)
        {
        case PieceType::Pawn:
            value = 100;
            pst = pawnPST[tableSq];
            break;
        case PieceType::Knight:
            value = 320;
            pst = knightPST[tableSq];
            break;
        case PieceType::Bishop:
            value = 330;
            pst = bishopPST[tableSq];
            break;
        case PieceType::Rook:
            value = 500;
            pst = rookPST[tableSq];
            break;
        case PieceType::Queen:
            value = 900;
            pst = queenPST[tableSq];
            break;
        case PieceType::King:
            value = 0; // intentional
            pst = kingMidgamePST[tableSq];
            break;
        default:
            break;
        }

        int total = value + pst;
        score += (p.color == Color::White) ? total : -total;
    }

    return score;
}

int quiescence(Board &board, int alpha, int beta, int ply)
{
    // Stand-pat only if not in check
    if (!board.kingInCheck())
    {
        int staticEval = evaluate(board);

        if (staticEval >= beta)
            return beta;

        if (staticEval > alpha)
            alpha = staticEval;
    }

    MoveList moves;
    if (board.kingInCheck())
    {
        moves = generateOrderedMoves(board); // all evasions
    }
    else
    {
        MoveList legalMoves = generateOrderedMoves(board);
        for (const Move &m : legalMoves)
            if (m.type == MoveType::Capture)
                moves.push_back(m);
    }

    if (moves.empty())
    {
        if (board.kingInCheck())
            return -MATE + ply;
        return alpha;
    }

    for (const Move &m : moves)
    {
        board.makeMove(m);
        int score = -quiescence(board, -beta, -alpha, ply + 1);
        board.unMakeMove();

        if (score >= beta)
            return beta;

        if (score > alpha)
            alpha = score;
    }

    return alpha;
}

int negamaxAlphaBeta(Board &board, int depth, int alpha, int beta, int ply)
{
    if (depth == 0)
    {
        return quiescence(board, alpha, beta, ply);
    }

    MoveList moves = generateLegalMoves(board);

    if (moves.empty())
    {
        if (board.kingInCheck())
            return -MATE + ply; // prefer faster mates
        return 0;
    }

    int best = -1000000;

    for (const Move &m : moves)
    {
        board.makeMove(m);
        int score = -negamaxAlphaBeta(board, depth - 1, -beta, -alpha, ply + 1);
        board.unMakeMove();

        if (score > best)
            best = score;

        if (best > alpha)
            alpha = best;

        if (alpha >= beta)
            break;
    }

    return alpha;
}

Move findBestMove(Board &board, int depth)
{
    searchMoveCount = 0;
    auto start = std::chrono::steady_clock::now();

    const int NEG_INF = -1000000;
    const int POS_INF = 1000000;

    MoveList moves = generateLegalMoves(board);
    if (moves.empty())
        return Move(); // or Move::none()

    Move bestMove = moves[0];
    int bestScore = NEG_INF;
    int alpha = NEG_INF;
    int beta = POS_INF;

    for (const Move &m : moves)
    {
        board.makeMove(m);
        int score = -negamaxAlphaBeta(board, depth - 1, -beta, -alpha, 0);
        board.unMakeMove();

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = m;
        }

        if (score > alpha)
            alpha = score;

        if (alpha >= beta)
            break;
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << searchMoveCount << " searched moves (" << elapsed_seconds.count() << " seconds)\n";
    return bestMove;
}
