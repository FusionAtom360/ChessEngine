#include "generate.h"

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
