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
#include "evaluate/evaluate.h"
#include "utils/utils.h"

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
    // board.setStateFEN("6R1/k1N5/Bq1P4/P1BQ3R/4PK2/5P2/8/4r3 w - - 19 68");
    std::cout << board.print();
    std::cout << board.FEN() << std::endl;

    while (!gameOver(board))
    {
        Move bestMove = findBestMove(board, 4);
        board.makeMove(bestMove);
        std::cout << board.print();

        // std::string userMove;
        // std::cin >> userMove;

        // Move m = parseMove(userMove, board);
    }

    return 0;
}
