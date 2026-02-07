#include "generate.h"
#include <algorithm>

static std::array<int, 2> pawnDirections = {7, 9};
static std::array<int, 8> knightDirections = {-17, -15, -10, -6, 6, 10, 15, 17};
static std::array<int, 8> kingDirections = {-9, -8, -7, -1, 1, 7, 8, 9};
static std::array<int, 4> rookDirections = {-8, -1, 1, 8};
static std::array<int, 4> bishopDirections = {-9, -7, 7, 9};
static std::array<int, 8> queenDirections = {-9, -8, -7, -1, 1, 7, 8, 9};

bool squareOnBoard(const int &sq)
{
    return (sq >= 0 && sq < 64);
}

void generatePawnMoves(const Board &board, const int &sq, MoveList &moves)
{
    int rank = sq / 8;
    int file = sq % 8;
    Color pawnColor = board.pieceAt(sq).color;
    Color opponentColor = oppositeColor(pawnColor);

    int oneAhead = (pawnColor == Color::White) ? sq + 8 : sq - 8;
    int twoAhead = (pawnColor == Color::White) ? sq + 16 : sq - 16;
    int nextSquare;
    int nextFile;

    int promotionRank = (pawnColor == Color::White) ? 6 : 1;
    int doubleMoveRank = (pawnColor == Color::White) ? 1 : 6;

    if (squareOnBoard(oneAhead) && board.isEmpty(oneAhead))
    {
        // Promotion moves
        if (rank == promotionRank)
        {
            moves.push_back({sq, oneAhead, MoveType::Promotion, PieceType::Rook});
            moves.push_back({sq, oneAhead, MoveType::Promotion, PieceType::Knight});
            moves.push_back({sq, oneAhead, MoveType::Promotion, PieceType::Bishop});
            moves.push_back({sq, oneAhead, MoveType::Promotion, PieceType::Queen});
        }
        else
        {
            // Single pawn push
            moves.push_back({sq, oneAhead, MoveType::Standard});
            // Double pawn push
            if (squareOnBoard(twoAhead) && board.isEmpty(twoAhead) && rank == doubleMoveRank)
            {
                moves.push_back({sq, twoAhead, MoveType::DoublePawnPush});
            }
        }
    }

    for (int d : pawnDirections)
    {
        nextSquare = (pawnColor == Color::White) ? sq + d : sq - d;

        if (!squareOnBoard(nextSquare))
        {
            continue;
        }

        nextFile = nextSquare % 8;

        if (nextFile - file > 1 || file - nextFile > 1)
        {
            continue;
        }

        if (board.isEmpty(nextSquare))
        {
            if (nextSquare == board.enPassantSquare())
            {
                moves.push_back({sq, nextSquare, MoveType::EnPassant});
            }
            continue;
        }

        if (board.pieceAt(nextSquare).color == opponentColor)
        {
            if (rank == promotionRank)
            {
                moves.push_back({sq, nextSquare, MoveType::Promotion, PieceType::Rook});
                moves.push_back({sq, nextSquare, MoveType::Promotion, PieceType::Knight});
                moves.push_back({sq, nextSquare, MoveType::Promotion, PieceType::Bishop});
                moves.push_back({sq, nextSquare, MoveType::Promotion, PieceType::Queen});
            }
            else
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
    }
}

template <size_t numDirections>
void generateSliderMoves(const Board &board, const int &sq, MoveList &moves, const std::array<int, numDirections> &directions)
{
    Color pieceColor = board.pieceAt(sq).color;
    Color opponentColor = oppositeColor(pieceColor);
    int nextSquare;
    int prevSquare;
    int prevFile;
    int nextFile;

    for (int d : directions)
    {
        nextSquare = sq;
        while (true)
        {
            prevSquare = nextSquare;
            nextSquare += d;

            if (!squareOnBoard(nextSquare))
                break;

            prevFile = prevSquare % 8;
            nextFile = nextSquare % 8;

            if (nextFile - prevFile > 1 || prevFile - nextFile > 1)
                break;

            if (board.isEmpty(nextSquare))
            {
                moves.push_back({sq, nextSquare, MoveType::Standard});
            }
            else
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                break;
            }
        }
    }
}

void generateKnightMoves(const Board &board, const int &sq, MoveList &moves)
{
    int nextSquare;
    int file = sq % 8;
    int nextFile;
    Color knightColor = board.pieceAt(sq).color;
    Color opponentColor = oppositeColor(knightColor);

    for (int d : knightDirections)
    {
        nextSquare = sq + d;

        if (!squareOnBoard(nextSquare))
        {
            continue;
        }

        nextFile = nextSquare % 8;

        if (nextFile - file > 2 || file - nextFile > 2)
        {
            continue;
        }

        if (board.isEmpty(nextSquare))
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
                moves.push_back({sq, nextSquare, MoveType::Capture});
            continue;
        }
    }
}

void generateKingMoves(const Board &board, const int &sq, MoveList &moves)
{
    int nextSquare;
    int file = sq % 8;
    int nextFile;
    Color kingColor = board.pieceAt(sq).color;
    Color opponentColor = oppositeColor(kingColor);
    CastlingAllowed castling = board.castlingAllowed();

    // Standard moves

    for (int d : kingDirections)
    {
        nextSquare = sq + d;

        if (!squareOnBoard(nextSquare))
        {
            continue;
        }

        file = sq % 8;
        nextFile = nextSquare % 8;

        if (nextFile - file > 1 || file - nextFile > 1)
        {
            continue;
        }

        if (board.isEmpty(nextSquare))
        {
            moves.push_back({sq, nextSquare, MoveType::Standard});
        }
        else
        {
            if (board.pieceAt(nextSquare).color == opponentColor)
                moves.push_back({sq, nextSquare, MoveType::Capture});
            continue;
        }
    }

    // Castling
    if (sq == 4 && kingColor == Color::White)
    {
        if (castling.whiteQueenSide && board.pieceAt(0).type == PieceType::Rook && board.pieceAt(0).color == Color::White && board.pieceAt(1).type == PieceType::None && board.pieceAt(2).type == PieceType::None && board.pieceAt(3).type == PieceType::None && !board.isPieceAttacked(3))
        {
            moves.push_back({sq, 2, MoveType::QueenCastle});
        }
        if (castling.whiteKingSide && board.pieceAt(7).type == PieceType::Rook && board.pieceAt(7).color == Color::White && board.pieceAt(5).type == PieceType::None && board.pieceAt(6).type == PieceType::None && !board.isPieceAttacked(5))
        {
            moves.push_back({sq, 6, MoveType::KingCastle});
        }
    }
    if (sq == 60 && kingColor == Color::Black)
    {
        if (castling.blackQueenSide && board.pieceAt(56).type == PieceType::Rook && board.pieceAt(56).color == Color::Black && board.pieceAt(57).type == PieceType::None && board.pieceAt(58).type == PieceType::None && board.pieceAt(59).type == PieceType::None && !board.isPieceAttacked(59))
        {
            moves.push_back({sq, 58, MoveType::QueenCastle});
        }
        if (castling.blackKingSide && board.pieceAt(63).type == PieceType::Rook && board.pieceAt(63).color == Color::Black && board.pieceAt(61).type == PieceType::None && board.pieceAt(62).type == PieceType::None && !board.isPieceAttacked(61))
        {
            moves.push_back({sq, 62, MoveType::KingCastle});
        }
    }
}

MoveList generatePseudoLegalMoves(const Board &board)
{
    MoveList moves;

    for (int sq = 0; sq < 64; sq++)
    {
        const Piece p = board.pieceAt(sq);

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
            generateSliderMoves<4>(board, sq, moves, rookDirections);
            break;
        case (PieceType::Knight):
            generateKnightMoves(board, sq, moves);
            break;
        case (PieceType::Bishop):
            generateSliderMoves<4>(board, sq, moves, bishopDirections);
            break;
        case (PieceType::Queen):
            generateSliderMoves<8>(board, sq, moves, queenDirections);
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

    Color movingColor = board.sideToMove();

    for (Move m : pseudoLegalMoves)
    {
        board.makeMove(m);
        if (!board.kingInCheck(movingColor))
        {
            legalMoves.push_back(m);
        }
        board.unMakeMove();
    }

    return legalMoves;
}

void generatePawnCaptureMoves(const Board &board, const int &sq, MoveList &moves)
{
    int rank = sq / 8;
    int file = sq % 8;
    Color pawnColor = board.pieceAt(sq).color;
    Color opponentColor = oppositeColor(pawnColor);

    int nextSquare;
    int nextFile;

    int promotionRank = (pawnColor == Color::White) ? 6 : 1;

    for (int d : pawnDirections)
    {
        nextSquare = (pawnColor == Color::White) ? sq + d : sq - d;

        if (!squareOnBoard(nextSquare))
        {
            continue;
        }

        nextFile = nextSquare % 8;

        if (nextFile - file > 1 || file - nextFile > 1)
        {
            continue;
        }

        if (board.isEmpty(nextSquare))
        {
            if (nextSquare == board.enPassantSquare())
            {
                moves.push_back({sq, nextSquare, MoveType::EnPassant});
            }
            continue;
        }

        if (board.pieceAt(nextSquare).color == opponentColor)
        {
            if (rank == promotionRank)
            {
                moves.push_back({sq, nextSquare, MoveType::Promotion, PieceType::Rook});
                moves.push_back({sq, nextSquare, MoveType::Promotion, PieceType::Knight});
                moves.push_back({sq, nextSquare, MoveType::Promotion, PieceType::Bishop});
                moves.push_back({sq, nextSquare, MoveType::Promotion, PieceType::Queen});
            }
            else
            {
                moves.push_back({sq, nextSquare, MoveType::Capture});
            }
        }
    }
}

template <size_t numDirections>
void generateSliderCaptureMoves(const Board &board, const int &sq, MoveList &moves, const std::array<int, numDirections> &directions)
{
    Color pieceColor = board.pieceAt(sq).color;
    Color opponentColor = oppositeColor(pieceColor);
    int nextSquare;
    int prevSquare;
    int prevFile;
    int nextFile;

    for (int d : directions)
    {
        nextSquare = sq;
        while (true)
        {
            prevSquare = nextSquare;
            nextSquare += d;

            if (!squareOnBoard(nextSquare))
                break;

            prevFile = prevSquare % 8;
            nextFile = nextSquare % 8;

            if (nextFile - prevFile > 1 || prevFile - nextFile > 1)
                break;

            if (!board.isEmpty(nextSquare))
            {
                if (board.pieceAt(nextSquare).color == opponentColor)
                {
                    moves.push_back({sq, nextSquare, MoveType::Capture});
                    break;
                }
                break;
            }
        }
    }
}

void generateKnightCaptureMoves(const Board &board, const int &sq, MoveList &moves)
{
    int nextSquare;
    int file = sq % 8;
    int nextFile;
    Color knightColor = board.pieceAt(sq).color;
    Color opponentColor = oppositeColor(knightColor);

    for (int d : knightDirections)
    {
        nextSquare = sq + d;

        if (!squareOnBoard(nextSquare))
        {
            continue;
        }

        nextFile = nextSquare % 8;

        if (nextFile - file > 2 || file - nextFile > 2)
        {
            continue;
        }

        if (!board.isEmpty(nextSquare) && board.pieceAt(nextSquare).color == opponentColor)
        {
            moves.push_back({sq, nextSquare, MoveType::Capture});
        }
    }
}

void generateKingCaptureMoves(const Board &board, const int &sq, MoveList &moves)
{
    int nextSquare;
    int file = sq % 8;
    int nextFile;
    Color kingColor = board.pieceAt(sq).color;
    Color opponentColor = oppositeColor(kingColor);

    // Standard moves

    for (int d : kingDirections)
    {
        nextSquare = sq + d;

        if (!squareOnBoard(nextSquare))
        {
            continue;
        }

        file = sq % 8;
        nextFile = nextSquare % 8;

        if (nextFile - file > 1 || file - nextFile > 1)
        {
            continue;
        }

        if (!board.isEmpty(nextSquare) && board.pieceAt(nextSquare).color == opponentColor)
        {
            moves.push_back({sq, nextSquare, MoveType::Capture});
        }
    }
}

MoveList generatePseudoLegalCaptureMoves(const Board &board)
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
            generatePawnCaptureMoves(board, sq, moves);
            break;
        case (PieceType::Rook):
            generateSliderCaptureMoves<4>(board, sq, moves, rookDirections);
            break;
        case (PieceType::Knight):
            generateKnightCaptureMoves(board, sq, moves);
            break;
        case (PieceType::Bishop):
            generateSliderCaptureMoves<4>(board, sq, moves, bishopDirections);
            break;
        case (PieceType::Queen):
            generateSliderCaptureMoves<8>(board, sq, moves, queenDirections);
            break;
        case (PieceType::King):
            generateKingCaptureMoves(board, sq, moves);
            break;
        }
    }

    return moves;
}

MoveList generateCaptureMoves(Board &board)
{
    MoveList pseudoLegalCaptureMoves = generatePseudoLegalCaptureMoves(board);
    MoveList legalCaptureMoves;

    Color movingColor = board.sideToMove();

    for (Move m : pseudoLegalCaptureMoves)
    {
        board.makeMove(m);
        if (!board.kingInCheck(movingColor))
        {
            legalCaptureMoves.push_back(m);
        }
        board.unMakeMove();
    }

    return legalCaptureMoves;
}

int pieceValue(PieceType type)
{
    switch (type)
    {
    case PieceType::Pawn:
        return 100;
    case PieceType::Knight:
        return 320;
    case PieceType::Bishop:
        return 330;
    case PieceType::Rook:
        return 500;
    case PieceType::Queen:
        return 900;
    case PieceType::King:
        return 20000;
    case PieceType::None:
        return 0;
    }
    return 0;
}

int scoreMoveStatic(const Move &m, Board &board)
{
    int score = 0;

    if (board.pieceAt(m.to).type != PieceType::None)
    {
        int victimValue = pieceValue(board.pieceAt(m.to).type);
        int attackerValue = pieceValue(board.pieceAt(m.from).type);
        score += (victimValue * 10 - attackerValue);
    }

    if (m.promotion != PieceType::None)
    {
        score += 900;
    }

    return score;
}

MoveList generateOrderedMoves(Board &board)
{
    MoveList pseudoLegalMoves = generatePseudoLegalMoves(board);
    ScoredMoveList scoredLegalMoves;
    int moveScore;

    Color movingColor = board.sideToMove();

    for (Move m : pseudoLegalMoves)
    {
        board.makeMove(m);
        if (!board.kingInCheck(movingColor))
        {
            moveScore = scoreMoveStatic(m, board);
            if (board.kingInCheck(oppositeColor(board.sideToMove())))
            {
                moveScore += 50;
            }
            scoredLegalMoves.push_back({m, moveScore});
        }
        board.unMakeMove();
    }

    std::sort(scoredLegalMoves.begin(), scoredLegalMoves.end(),
              [](const ScoredMove &a, const ScoredMove &b)
              { return a.score > b.score; }); // descending

    MoveList orderedMoves;
    for (ScoredMove m : scoredLegalMoves)
        orderedMoves.push_back(m.move);

    return orderedMoves;
}

MoveList generateOrderedCaptureMoves(Board &board)
{
    MoveList pseudoLegalMoves = generatePseudoLegalCaptureMoves(board);
    ScoredMoveList scoredLegalMoves;
    int moveScore;

    Color movingColor = board.sideToMove();

    for (Move m : pseudoLegalMoves)
    {
        board.makeMove(m);
        if (!board.kingInCheck(movingColor))
        {
            moveScore = scoreMoveStatic(m, board);
            if (board.kingInCheck(oppositeColor(board.sideToMove())))
            {
                moveScore += 50;
            }
            scoredLegalMoves.push_back({m, moveScore});
        }
        board.unMakeMove();
    }

    std::sort(scoredLegalMoves.begin(), scoredLegalMoves.end(),
              [](const ScoredMove &a, const ScoredMove &b)
              { return a.score > b.score; }); // descending

    MoveList orderedMoves;
    for (ScoredMove m : scoredLegalMoves)
        orderedMoves.push_back(m.move);

    return orderedMoves;
}
