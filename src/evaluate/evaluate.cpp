#include "evaluate.h"
#include <iostream>

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

    return (board.sideToMove() == Color::White) ? score : -score;
}

int quiescence(Board &board, int alpha, int beta, int ply)
{
    if (ply > maxPly)
    {
        return evaluate(board);
    }

    // Stand-pat only if not in check
    if (!board.kingInCheck())
    {
        int staticEval = evaluate(board);

        if (staticEval >= beta)
        {
            return beta;
        }

        if (staticEval < alpha - deltaMargin)
        {
            return alpha;
        }

        if (staticEval > alpha)
        {
            alpha = staticEval;
        }
    }

    MoveList moves;
    if (board.kingInCheck())
    {
        moves = generateOrderedMoves(board);
    }
    else
    {
        moves = generateOrderedCaptureMoves(board);
    }

    if (moves.empty())
    {
        if (board.kingInCheck())
            return -MATE + ply;
        return alpha;
    }

    for (const Move &m : moves)
    {
        searchMoveCount++;
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

bool timeIsUp(std::chrono::steady_clock::time_point &startTime, double &timeLimit)
{
    auto now = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(now - startTime).count();
    return elapsed > timeLimit;
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

    return best;
}

int negamaxAlphaBeta(Board &board, int depth, int alpha, int beta, int ply, std::chrono::steady_clock::time_point &startTime, double &timeLimit)
{
    if (timeIsUp(startTime, timeLimit))
    {
        return evaluate(board);
    }

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
        int score = -negamaxAlphaBeta(board, depth - 1, -beta, -alpha, ply + 1, startTime, timeLimit);
        board.unMakeMove();

        if (score > best)
            best = score;

        if (best > alpha)
            alpha = best;

        if (alpha >= beta)
            break;
    }

    return best;
}

Move findBestMove(Board &board, int depth)
{
    searchMoveCount = 0;
    auto start = std::chrono::steady_clock::now();

    const int NEG_INF = -1000000;
    const int POS_INF = 1000000;

    MoveList moves = generateLegalMoves(board);
    if (moves.empty())
        return Move();

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

Move findBestMove(Board &board, int maxDepth, double timeLimit)
{
    searchMoveCount = 0;
    auto start = std::chrono::steady_clock::now();

    const int NEG_INF = -1000000;
    const int POS_INF = 1000000;

    MoveList moves = generateLegalMoves(board);
    if (moves.empty())
    {
        return Move();
    }
    if (moves.size() == 1)
    {
        return moves[0];
    }

    Move bestMove;
    size_t bestIndex;
    int bestScore = NEG_INF;
    int highestDepth;

    // Iterative deepening
    for (int depth = 1; depth <= maxDepth; depth++)
    {
        int alpha = NEG_INF;
        int beta = POS_INF;

        int currentBestScore = NEG_INF;
        Move currentBestMove = moves[0];

        for (size_t i = 0; i < moves.size(); i++)
        {
            board.makeMove(moves[i]);
            int score = -negamaxAlphaBeta(board, depth - 1, -beta, -alpha, 0, start, timeLimit);
            board.unMakeMove();

            if (score > bestScore)
            {
                currentBestScore = score;
                currentBestMove = moves[i];
                bestIndex = i;
            }

            if (score > alpha)
            {
                alpha = score;
            }

            if (alpha >= beta)
            {
                break;
            }
        }

        // Do not apply changes if depth is unfinished
        if (timeIsUp(start, timeLimit))
        {
            break;
        }

        // Set best move to top of list for next depth
        std::swap(moves[0], moves[bestIndex]);

        bestMove = currentBestMove;
        bestScore = currentBestScore;
        highestDepth = depth;
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << searchMoveCount << " searched moves (depth " << highestDepth << ", " << elapsed_seconds.count() << " seconds)\n";
    std::cout << board.toString(bestMove) << std::endl;

    return bestMove;
}

bool gameOver(Board &board)
{
    MoveList legalMoves = generateLegalMoves(board);
    if (legalMoves.empty())
    {
        return true;
    }
    if (board.halfMoveCounter() >= 50)
    {
        return true;
    }
    return false;
}
