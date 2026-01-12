#include <iostream>
#include <array>
#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <cassert>
#include <stdexcept>
#include "board/board.h"
#include "generate/generate.h"

const int MATE = 32000;

const int pawnPST[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5, 5, 10, 25, 25, 10, 5, 5,
    0, 0, 0, 20, 20, 0, 0, 0,
    5, -5, -10, 0, 0, -10, -5, 5,
    5, 10, 10, -20, -20, 10, 10, 5,
    0, 0, 0, 0, 0, 0, 0, 0};

const int knightPST[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50};

const int bishopPST[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 10, 10, 5, 0, -10,
    -10, 5, 5, 10, 10, 5, 5, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 10, 10, 10, 10, 10, 10, -10,
    -10, 5, 0, 0, 0, 0, 5, -10,
    -20, -10, -10, -10, -10, -10, -10, -20};

const int rookPST[64] = {
    0, 0, 0, 5, 5, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    5, 10, 10, 10, 10, 10, 10, 5,
    0, 0, 0, 0, 0, 0, 0, 0};

const int queenPST[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 5, 5, 5, 0, -10,
    -5, 0, 5, 5, 5, 5, 0, -5,
    0, 0, 5, 5, 5, 5, 0, -5,
    -10, 5, 5, 5, 5, 5, 0, -10,
    -10, 0, 5, 0, 0, 0, 0, -10,
    -20, -10, -10, -5, -5, -10, -10, -20};

const int kingMidgamePST[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    20, 20, 0, 0, 0, 0, 20, 20,
    20, 30, 10, 0, 0, 10, 30, 20};

int mirror(int sq)
{
    return sq ^ 56; // flips rank
}

int searchMoveCount = 0;

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
        moves = generateLegalMoves(board); // all evasions
    }
    else
    {
        MoveList legalMoves = generateLegalMoves(board);
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

    std::cout << bestScore << std::endl;
    return bestMove;
}

int fileCharToInt(char f) { return f - 'a'; }
int rankCharToInt(char r) { return r - '1'; }

int squareFromString(const std::string &s)
{
    int file = fileCharToInt(s[0]);
    int rank = rankCharToInt(s[1]);
    return rank * 8 + file;
}

Move parseMove(const std::string &moveStr, const Board &board)
{
    if (moveStr.length() < 4)
        throw std::invalid_argument("Move string too short");

    Move m;
    m.from = squareFromString(moveStr.substr(0, 2));
    m.to = squareFromString(moveStr.substr(2, 2));
    m.type = MoveType::Standard;
    m.promotion = PieceType::None;

    Piece movingPiece = board.pieceAt(m.from);
    Piece targetPiece = board.pieceAt(m.to);

    if (moveStr.length() == 5)
    {
        char promoChar = moveStr[4];
        switch (promoChar)
        {
        case 'q':
            m.promotion = PieceType::Queen;
            break;
        case 'r':
            m.promotion = PieceType::Rook;
            break;
        case 'b':
            m.promotion = PieceType::Bishop;
            break;
        case 'n':
            m.promotion = PieceType::Knight;
            break;
        default:
            throw std::invalid_argument("Invalid promotion piece");
        }
        m.type = MoveType::Promotion;
    }

    if (movingPiece.type == PieceType::King)
    {
        if (m.from == 4 && m.to == 6)
            m.type = MoveType::KingCastle;
        if (m.from == 4 && m.to == 2)
            m.type = MoveType::QueenCastle;
    }

    if (targetPiece.type != PieceType::None && targetPiece.color != movingPiece.color)
    {
        m.type = MoveType::Capture;
    }

    if (movingPiece.type == PieceType::Pawn)
    {
        int rankFrom = m.from / 8;
        int rankTo = m.to / 8;
        if (std::abs(rankTo - rankFrom) == 2)
            m.type = MoveType::DoublePawnPush;
    }

    if (movingPiece.type == PieceType::Pawn && targetPiece.type == PieceType::None)
    {
        int fileFrom = m.from % 8;
        int fileTo = m.to % 8;
        if (std::abs(fileTo - fileFrom) == 1 && m.to == board.enPassantSquare())
        {
            m.type = MoveType::EnPassant;
        }
    }

    return m;
}

// 56 57 58 59 60 61 62 63
// 48 49 50 51 52 53 54 55
// 40 41 42 43 44 45 46 47
// 32 33 34 35 36 37 38 39
// 24 25 26 27 28 29 30 31
// 16 17 18 19 20 21 22 23
//  8  9 10 11 12 13 14 15
//  0  1  2  3  4  5  6  7

// a8 b8 c8 d8 e8 f8 g8 h8
// a7 b7 c7 d7 e7 f7 g7 h7
// a6 b6 c6 d6 e6 f6 g6 h6
// a5 b5 c5 d5 e5 f5 g5 h5
// a4 b4 c4 d4 e4 f4 g4 h4
// a3 b3 c3 d3 e3 f3 g3 h3
// a2 b2 c2 d2 e2 f2 g2 h2
// a1 b1 c1 d1 e1 f1 g1 h1

int main()
{

    Board board = Board();
    board.setStateFEN("6R1/k1N5/Bq1P4/P1BQ3R/4PK2/5P2/8/4r3 w - - 19 68");
    std::cout << board.print();

    while (true)
    {
        Move bestMove = findBestMove(board, 5);
        std::cout << searchMoveCount << " searched moves \n";
        searchMoveCount = 0;
        board.makeMove(bestMove);
        std::cout << board.print();

        std::string userMove;
        std::cin >> userMove;

        Move m = parseMove(userMove, board);
        board.makeMove(m);
        std::cout << board.print();
    }

    return 0;
}
