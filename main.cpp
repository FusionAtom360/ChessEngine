#include <iostream>
#include <array>
#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <cassert>
#include <stdexcept>
#include <limits>
#include "board/board.h"


void generatePawnMoves(const Board &board, int sq, MoveList &moves)
{
    int rank = sq / 8;
    int file = sq % 8;
    int oneAhead;
    int twoAhead;
    int leftCapture;
    int rightCapture;
    int promotionRank;
    int doubleMoveRank;
    Color pawnColor = board.pieceAt(sq).color;
    Color opponentColor = oppositeColor(pawnColor);

    if (pawnColor == Color::White)
    {
        oneAhead = sq + 8;
        twoAhead = sq + 16;
        leftCapture = sq + 7;
        rightCapture = sq + 9;
        promotionRank = 6;
        doubleMoveRank = 1;
    }
    else
    {
        oneAhead = sq - 8;
        twoAhead = sq - 16;
        leftCapture = sq - 9;
        rightCapture = sq - 7;
        promotionRank = 1;
        doubleMoveRank = 6;
    }

    if ((pawnColor == Color::White && rank < 7) || (pawnColor == Color::Black && rank > 0))
    {
        if (oneAhead >= 0 && oneAhead <= 63 && board.isEmpty(oneAhead))
        {
            // promotable
            if (rank == promotionRank)
            {
                moves.push_back({sq, oneAhead, MoveType::Promotion, PieceType::Rook});
                moves.push_back({sq, oneAhead, MoveType::Promotion, PieceType::Knight});
                moves.push_back({sq, oneAhead, MoveType::Promotion, PieceType::Bishop});
                moves.push_back({sq, oneAhead, MoveType::Promotion, PieceType::Queen});
            }
            else
            {
                moves.push_back({sq, oneAhead, MoveType::Standard});
                if (twoAhead >= 0 && twoAhead <= 63 && board.isEmpty(twoAhead) && rank == doubleMoveRank)
                {
                    moves.push_back({sq, twoAhead, MoveType::Standard});
                }
            }
        }
        if (file > 0)
        {
            // if black piece available to capture
            if (!board.isEmpty(leftCapture) && board.pieceAt(leftCapture).color == opponentColor)
            {
                if (rank == promotionRank)
                {
                    moves.push_back({sq, leftCapture, MoveType::Promotion, PieceType::Rook});
                    moves.push_back({sq, leftCapture, MoveType::Promotion, PieceType::Knight});
                    moves.push_back({sq, leftCapture, MoveType::Promotion, PieceType::Bishop});
                    moves.push_back({sq, leftCapture, MoveType::Promotion, PieceType::Queen});
                }
                else
                {
                    moves.push_back({sq, leftCapture, MoveType::Capture});
                }
            }
            else if (leftCapture == board.enPassantSquare())
            {
                moves.push_back({sq, leftCapture, MoveType::EnPassant});
            }
        }

        if (file < 7)
        {
            // if black piece available to capture
            if (!board.isEmpty(rightCapture) && board.pieceAt(rightCapture).color == opponentColor)
            {
                if (rank == promotionRank)
                {
                    moves.push_back({sq, rightCapture, MoveType::Promotion, PieceType::Rook});
                    moves.push_back({sq, rightCapture, MoveType::Promotion, PieceType::Knight});
                    moves.push_back({sq, rightCapture, MoveType::Promotion, PieceType::Bishop});
                    moves.push_back({sq, rightCapture, MoveType::Promotion, PieceType::Queen});
                }
                else
                {
                    moves.push_back({sq, rightCapture, MoveType::Capture});
                }
            }
            else if (rightCapture == board.enPassantSquare())
            {
                moves.push_back({sq, rightCapture, MoveType::EnPassant});
            }
        }
    }
}

void generateRookMoves(const Board &board, int sq, MoveList &moves)
{
    int rank = sq / 8;
    int nextSquare;
    Color opponentColor = oppositeColor(board.pieceAt(sq).color);

    // up
    nextSquare = sq;
    while (true)
    {
        nextSquare += 8;
        if (nextSquare <= 63 && nextSquare >= 0)
        {
            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                // break if white piece
                break;
            }
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            break;
        }
    }

    // down
    nextSquare = sq;
    while (true)
    {
        nextSquare -= 8;
        if (nextSquare <= 63 && nextSquare >= 0)
        {
            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                // break if white piece
                break;
            }
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            break;
        }
    }

    // left
    nextSquare = sq;
    while (true)
    {
        nextSquare -= 1;
        if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == rank)
        {
            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                // break if white piece
                break;
            }
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            break;
        }
    }

    // right
    nextSquare = sq;
    while (true)
    {
        nextSquare += 1;
        if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == rank)
        {
            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                // break if white piece
                break;
            }
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            break;
        }
    }
}

void generateKnightMoves(const Board &board, int sq, MoveList &moves)
{
    int startRank = sq / 8;
    int nextSquare;
    Color opponentColor = oppositeColor(board.pieceAt(sq).color);

    nextSquare = sq + 6;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank + 1)
    {
        if (!board.isEmpty(nextSquare))
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
        else
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
    }

    nextSquare = sq + 10;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank + 1)
    {
        if (!board.isEmpty(nextSquare))
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
        else
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
    }

    nextSquare = sq + 15;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank + 2)
    {
        if (!board.isEmpty(nextSquare))
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
        else
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
    }

    nextSquare = sq + 17;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank + 2)
    {
        if (!board.isEmpty(nextSquare))
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
        else
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
    }

    nextSquare = sq - 6;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank - 1)
    {
        if (!board.isEmpty(nextSquare))
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
        else
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
    }

    nextSquare = sq - 10;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank - 1)
    {
        if (!board.isEmpty(nextSquare))
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
        else
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
    }

    nextSquare = sq - 15;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank - 2)
    {
        if (!board.isEmpty(nextSquare))
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
        else
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
    }

    nextSquare = sq - 17;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank - 2)
    {
        if (!board.isEmpty(nextSquare))
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
        else
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
    }
}

void generateBishopMoves(const Board &board, int sq, MoveList &moves)
{
    int startRank = sq / 8;
    int nextSquare;
    int nextRank;
    Color opponentColor = oppositeColor(board.pieceAt(sq).color);

    // up left
    nextSquare = sq;
    nextRank = startRank;
    while (true)
    {
        nextSquare += 7;
        nextRank += 1;
        if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
        {
            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                // break if white piece
                break;
            }
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            break;
        }
    }

    // up right
    nextSquare = sq;
    nextRank = startRank;
    while (true)
    {
        nextSquare += 9;
        nextRank += 1;
        if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
        {
            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                // break if white piece
                break;
            }
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            break;
        }
    }

    // down left
    nextSquare = sq;
    nextRank = startRank;
    while (true)
    {
        nextSquare -= 9;
        nextRank -= 1;
        if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
        {
            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                // break if white piece
                break;
            }
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            break;
        }
    }

    // down right
    nextSquare = sq;
    nextRank = startRank;
    while (true)
    {
        nextSquare -= 7;
        nextRank -= 1;
        if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
        {
            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                // break if white piece
                break;
            }
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            break;
        }
    }
}

void generateQueenMoves(const Board &board, int sq, MoveList &moves)
{
    int startRank = sq / 8;
    int nextSquare;
    Color opponentColor = oppositeColor(board.pieceAt(sq).color);

    // up
    nextSquare = sq;
    while (true)
    {
        nextSquare += 8;
        if (nextSquare <= 63 && nextSquare >= 0)
        {
            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                // break if white piece
                break;
            }
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            break;
        }
    }

    // down
    nextSquare = sq;
    while (true)
    {
        nextSquare -= 8;
        if (nextSquare <= 63 && nextSquare >= 0)
        {
            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                // break if white piece
                break;
            }
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            break;
        }
    }

    // left
    nextSquare = sq;
    while (true)
    {
        nextSquare -= 1;
        if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank)
        {
            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                // break if white piece
                break;
            }
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            break;
        }
    }

    // right
    nextSquare = sq;
    while (true)
    {
        nextSquare += 1;
        if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank)
        {
            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                // break if white piece
                break;
            }
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            break;
        }
    }

    int nextRank;

    // up left
    nextSquare = sq;
    nextRank = startRank;
    while (true)
    {
        nextSquare += 7;
        nextRank += 1;
        if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
        {
            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                // break if white piece
                break;
            }
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            break;
        }
    }

    // up right
    nextSquare = sq;
    nextRank = startRank;
    while (true)
    {
        nextSquare += 9;
        nextRank += 1;
        if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
        {
            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                // break if white piece
                break;
            }
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            break;
        }
    }

    // down left
    nextSquare = sq;
    nextRank = startRank;
    while (true)
    {
        nextSquare -= 9;
        nextRank -= 1;
        if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
        {
            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                // break if white piece
                break;
            }
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            break;
        }
    }

    // down right
    nextSquare = sq;
    nextRank = startRank;
    while (true)
    {
        nextSquare -= 7;
        nextRank -= 1;
        if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
        {
            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                // break if white piece
                break;
            }
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            break;
        }
    }
}

void generateKingMoves(const Board &board, int sq, MoveList &moves)
{
    int startRank = sq / 8;
    int nextSquare;
    Color opponentColor = oppositeColor(board.pieceAt(sq).color);

    nextSquare = sq + 1;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank)
    {
        if (!board.isEmpty(nextSquare))
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
        else
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
    }

    nextSquare = sq + 7;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank + 1)
    {
        if (!board.isEmpty(nextSquare))
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
        else
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
    }

    nextSquare = sq + 8;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank + 1)
    {
        if (!board.isEmpty(nextSquare))
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
        else
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
    }

    nextSquare = sq + 9;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank + 1)
    {
        if (!board.isEmpty(nextSquare))
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
        else
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
    }

    nextSquare = sq - 1;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank)
    {
        if (!board.isEmpty(nextSquare))
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
        else
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
    }

    nextSquare = sq - 7;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank - 1)
    {
        if (!board.isEmpty(nextSquare))
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
        else
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
    }

    nextSquare = sq - 8;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank - 1)
    {
        if (!board.isEmpty(nextSquare))
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
        else
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
    }

    nextSquare = sq - 9;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == startRank - 1)
    {
        if (!board.isEmpty(nextSquare))
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
        else
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
    }

    // castling
    CastlingAllowed castling = board.castlingAllowed();
    if (castling.whiteQueenSide)
    {
        if (sq == 4 && board.pieceAt(sq).color == Color::White && board.pieceAt(0).type == PieceType::Rook && board.pieceAt(0).color == Color::White && board.pieceAt(1).type == PieceType::None && board.pieceAt(2).type == PieceType::None && board.pieceAt(3).type == PieceType::None)
        {
            if (!board.isPieceAttacked(3))
            {
                moves.push_back({sq, 2, MoveType::QueenCastle});
            }
        }
    }
    if (castling.whiteKingSide)
    {
        if (sq == 4 && board.pieceAt(sq).color == Color::White && board.pieceAt(7).type == PieceType::Rook && board.pieceAt(7).color == Color::White && board.pieceAt(5).type == PieceType::None && board.pieceAt(6).type == PieceType::None)
        {
            if (!board.isPieceAttacked(5))
            {
                moves.push_back({sq, 6, MoveType::KingCastle});
            }
        }
    }
    if (castling.blackQueenSide)
    {
        if (sq == 60 && board.pieceAt(sq).color == Color::Black && board.pieceAt(56).type == PieceType::Rook && board.pieceAt(56).color == Color::Black && board.pieceAt(57).type == PieceType::None && board.pieceAt(58).type == PieceType::None && board.pieceAt(59).type == PieceType::None)
        {
            if (!board.isPieceAttacked(59))
            {
                moves.push_back({sq, 58, MoveType::QueenCastle});
            }
        }
    }
    if (castling.blackKingSide)
    {
        if (sq == 60 && board.pieceAt(sq).color == Color::Black && board.pieceAt(63).type == PieceType::Rook && board.pieceAt(63).color == Color::Black && board.pieceAt(61).type == PieceType::None && board.pieceAt(62).type == PieceType::None)
        {
            if (!board.isPieceAttacked(61))
            {
                moves.push_back({sq, 62, MoveType::KingCastle});
            }
        }
    }
}

MoveList generatePseudoLegalMoves(const Board &board)
{
    MoveList moves;

    for (int sq = 0; sq < 64; sq++)
    {
        Piece p = board.pieceAt(sq);

        if (p.color != board.sideToMove())
        {
            continue;
        }

        switch (p.type)
        {
        case (PieceType::None):
            break;
        case (PieceType::Pawn):
            generatePawnMoves(board, sq, moves);
            break;
        case (PieceType::Rook):
            generateRookMoves(board, sq, moves);
            break;
        case (PieceType::Knight):
            generateKnightMoves(board, sq, moves);
            break;
        case (PieceType::Bishop):
            generateBishopMoves(board, sq, moves);
            break;
        case (PieceType::Queen):
            generateQueenMoves(board, sq, moves);
            break;
        case (PieceType::King):
            generateKingMoves(board, sq, moves);
            break;
        }
    }

    return moves;
}

MoveList generateLegalMoves(Board &board)
{
    MoveList pseudoLegalMoves = generatePseudoLegalMoves(board);
    MoveList legalMoves;

    for (Move m : pseudoLegalMoves)
    {
        board.makeMove(m);
        if (!board.kingInCheck(oppositeColor(board.sideToMove())))
        {
            legalMoves.push_back(m);
        }
        board.unMakeMove();
    }

    return legalMoves;
}

const int MATE = 100000;

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

    Color stm = board.sideToMove();

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
        score += (p.color == stm) ? total : -total;
    }

    return score;
}

int quiescence(Board &board, int alpha, int beta)
{
    int staticEval = evaluate(board);

    // Fail-hard beta cutoff
    if (staticEval >= beta)
        return beta;

    if (staticEval > alpha)
        alpha = staticEval;

    // Generate captures only
    MoveList legalMoves = generateLegalMoves(board);
    MoveList captureMoves;

    for (Move m : legalMoves)
    {
        if (m.type == MoveType::Capture) {
            captureMoves.push_back(m);
        }
    }

    // If no captures, return static eval
    for (const Move &m : captureMoves)
    {
        board.makeMove(m);
        int score = -quiescence(board, -beta, -alpha);
        board.unMakeMove();

        if (score >= beta)
            return beta;

        if (score > alpha)
            alpha = score;
    }

    return alpha;
}

int negamaxAlphaBeta(Board &board, int depth, int alpha, int beta)
{
    if (depth == 0)
    {
        return quiescence(board, alpha, beta);
    }

    MoveList moves = generateLegalMoves(board);

    if (moves.empty())
    {
        if (board.kingInCheck())
            return -MATE + depth; // prefer faster mates
        return 0;
    }

    int best = -1000000;

    for (const Move &m : moves)
    {
        board.makeMove(m);
        int score = -negamaxAlphaBeta(board, depth - 1, -beta, -alpha);
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
        int score = -negamaxAlphaBeta(board, depth - 1, -beta, -alpha);
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
    board.print();

    while (true)
    {
        Move bestMove = findBestMove(board, 5);
        std::cout << searchMoveCount << " searched moves \n";
        searchMoveCount = 0;
        board.makeMove(bestMove);
        board.print();

        std::string userMove;
        std::cin >> userMove;

        Move m = parseMove(userMove, board);
        board.makeMove(m);
        board.print();
    }

    return 0;
}
