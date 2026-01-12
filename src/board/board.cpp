#include "board.h"
#include <string>
#include <sstream>
#include <cassert>

Color oppositeColor(const Color &color)
{
    if (color == Color::White)
    {
        return Color::Black;
    }
    return Color::White;
}

char Board::pieceToChar(const Piece p) const
{
    if (p.type == PieceType::None)
        return '.';

    char c;
    switch (p.type)
    {
    case PieceType::Pawn:
        c = 'p';
        break;
    case PieceType::Knight:
        c = 'n';
        break;
    case PieceType::Bishop:
        c = 'b';
        break;
    case PieceType::Rook:
        c = 'r';
        break;
    case PieceType::Queen:
        c = 'q';
        break;
    case PieceType::King:
        c = 'k';
        break;
    default:
        c = '?';
    }

    if (p.color == Color::White)
    {
        return static_cast<char>(std::toupper(c));
    }
    return c;
}

Board::Board() : state(), squares(), history()
{
    state.sideToMove = Color::White;
    state.castling = {true, true, true, true};
    state.enPassantSquare = -1;
    state.halfMoveClock = 0;
    state.fullMoveNumber = 1;

    for (int s = 0; s < 64; s++)
    {
        squares[s] = Piece{PieceType::None, Color::None};
    }

    squares[0] = Piece{PieceType::Rook, Color::White};
    squares[1] = Piece{PieceType::Knight, Color::White};
    squares[2] = Piece{PieceType::Bishop, Color::White};
    squares[3] = Piece{PieceType::Queen, Color::White};
    squares[4] = Piece{PieceType::King, Color::White};
    squares[5] = Piece{PieceType::Bishop, Color::White};
    squares[6] = Piece{PieceType::Knight, Color::White};
    squares[7] = Piece{PieceType::Rook, Color::White};

    for (int file = 0; file < 8; file++)
    {
        squares[8 + file] = Piece{PieceType::Pawn, Color::White};
    }

    squares[56] = Piece{PieceType::Rook, Color::Black};
    squares[57] = Piece{PieceType::Knight, Color::Black};
    squares[58] = Piece{PieceType::Bishop, Color::Black};
    squares[59] = Piece{PieceType::Queen, Color::Black};
    squares[60] = Piece{PieceType::King, Color::Black};
    squares[61] = Piece{PieceType::Bishop, Color::Black};
    squares[62] = Piece{PieceType::Knight, Color::Black};
    squares[63] = Piece{PieceType::Rook, Color::Black};

    for (int file = 0; file < 8; file++)
    {
        squares[48 + file] = Piece{PieceType::Pawn, Color::Black};
    }
}

std::string Board::print() const
{
    std::string outputString;
    for (int rank = 7; rank >= 0; rank--)
    {
        for (int file = 0; file < 8; file++)
        {
            int sq = rank * 8 + file;
            outputString += std::string(1, pieceToChar(squares[sq])) + " ";
        }
        outputString += (std::to_string(rank + 1) + "\n");
    }
    outputString += "a b c d e f g h\n";

    outputString += ("Move: " + std::to_string(state.fullMoveNumber) + " | ");
    if (state.sideToMove == Color::Black)
    {
        outputString += "Black to play";
    }
    else
    {
        outputString += "White to play";
    }

    outputString += " | Castling: ";
    if (state.castling.whiteKingSide)
    {
        outputString += "K";
    }
    if (state.castling.whiteQueenSide)
    {
        outputString += "Q";
    }
    if (state.castling.blackKingSide)
    {
        outputString += "k";
    }
    if (state.castling.blackQueenSide)
    {
        outputString += "q";
    }

    outputString += (" | Half moves: " + std::to_string(state.halfMoveClock) + " | En passant: " + std::to_string(state.enPassantSquare) + "\n");
    return outputString;
}

void Board::setStateFEN(std::string fen)
{
    for (int s = 0; s < 64; s++)
    {
        squares[s] = Piece{PieceType::None, Color::None};
    }

    // split string into component parts
    std::istringstream iss(fen);
    std::string piecePlacement, activeColor, castlingAllowed, enPassantTarget, halfMoveClock, fullMoveNumber;

    iss >> piecePlacement >> activeColor >> castlingAllowed >> enPassantTarget >> halfMoveClock >> fullMoveNumber;

    // start from top left
    int current_square = 56;
    for (char c : piecePlacement)
    {
        switch (c)
        {
        case ('r'):
            squares[current_square] = Piece{PieceType::Rook, Color::Black};
            current_square++;
            break;
        case ('n'):
            squares[current_square] = Piece{PieceType::Knight, Color::Black};
            current_square++;
            break;
        case ('b'):
            squares[current_square] = Piece{PieceType::Bishop, Color::Black};
            current_square++;
            break;
        case ('q'):
            squares[current_square] = Piece{PieceType::Queen, Color::Black};
            current_square++;
            break;
        case ('k'):
            squares[current_square] = Piece{PieceType::King, Color::Black};
            current_square++;
            break;
        case ('p'):
            squares[current_square] = Piece{PieceType::Pawn, Color::Black};
            current_square++;
            break;
        case ('R'):
            squares[current_square] = Piece{PieceType::Rook, Color::White};
            current_square++;
            break;
        case ('N'):
            squares[current_square] = Piece{PieceType::Knight, Color::White};
            current_square++;
            break;
        case ('B'):
            squares[current_square] = Piece{PieceType::Bishop, Color::White};
            current_square++;
            break;
        case ('Q'):
            squares[current_square] = Piece{PieceType::Queen, Color::White};
            current_square++;
            break;
        case ('K'):
            squares[current_square] = Piece{PieceType::King, Color::White};
            current_square++;
            break;
        case ('P'):
            squares[current_square] = Piece{PieceType::Pawn, Color::White};
            current_square++;
            break;
        case ('/'):
            current_square -= 16;
            break;
        case ('1'):
            current_square += 1;
            break;
        case ('2'):
            current_square += 2;
            break;
        case ('3'):
            current_square += 3;
            break;
        case ('4'):
            current_square += 4;
            break;
        case ('5'):
            current_square += 5;
            break;
        case ('6'):
            current_square += 6;
            break;
        case ('7'):
            current_square += 7;
            break;
        case ('8'):
            current_square += 8;
            break;
        }
    }

    if (activeColor == "w")
    {
        state.sideToMove = Color::White;
    }
    else
    {
        state.sideToMove = Color::Black;
    }

    state.castling.whiteKingSide = false;
    state.castling.whiteQueenSide = false;
    state.castling.blackKingSide = false;
    state.castling.blackQueenSide = false;

    for (char c : castlingAllowed)
    {
        switch (c)
        {
        case 'K':
            state.castling.whiteKingSide = true;
            break;
        case 'Q':
            state.castling.whiteQueenSide = true;
            break;
        case 'k':
            state.castling.blackKingSide = true;
            break;
        case 'q':
            state.castling.blackQueenSide = true;
            break;
        }
    }

    if (enPassantTarget != "-")
    {
        state.enPassantSquare = coordsToIndex(enPassantTarget);
    }

    state.halfMoveClock = std::stoi(halfMoveClock);
    state.fullMoveNumber = std::stoi(fullMoveNumber);
}

Piece Board::pieceAt(int sq) const
{
    assert(sq >= 0 && sq < 64);
    return squares[sq];
}

Color Board::sideToMove() const
{
    return state.sideToMove;
}

bool Board::isEmpty(int sq) const
{
    assert(sq >= 0 && sq < 64);
    return squares[sq].type == PieceType::None;
}

int Board::coordsToIndex(std::string coords)
{
    char file = coords[0];
    int rank = coords[1] - '0' - 1;
    int square = 0;

    switch (file)
    {
    case 'a':
        square = rank * 8;
        break;
    case 'b':
        square = rank * 8 + 1;
        break;
    case 'c':
        square = rank * 8 + 2;
        break;
    case 'd':
        square = rank * 8 + 3;
        break;
    case 'e':
        square = rank * 8 + 4;
        break;
    case 'f':
        square = rank * 8 + 5;
        break;
    case 'g':
        square = rank * 8 + 6;
        break;
    case 'h':
        square = rank * 8 + 7;
        break;
    }

    return square;
}

int Board::enPassantSquare() const
{
    return state.enPassantSquare;
}

bool Board::isPieceAttacked(int sq) const
{
    Color opponentColor = oppositeColor(pieceAt(sq).color);
    int startRank = sq / 8;

    int leftPawnAttack;
    int rightPawnAttack;
    if (opponentColor == Color::Black)
    {
        leftPawnAttack = sq + 7;
        rightPawnAttack = sq + 9;
        if (leftPawnAttack >= 0 && leftPawnAttack <= 63 && leftPawnAttack / 8 == startRank + 1)
        {
            if (pieceAt(leftPawnAttack).type == PieceType::Pawn && pieceAt(leftPawnAttack).color == opponentColor)
            {
                return true;
            }
        }
        if (rightPawnAttack >= 0 && rightPawnAttack <= 63 && rightPawnAttack / 8 == startRank + 1)
        {
            if (pieceAt(rightPawnAttack).type == PieceType::Pawn && pieceAt(rightPawnAttack).color == opponentColor)
            {
                return true;
            }
        }
    }
    else
    {
        leftPawnAttack = sq - 9;
        rightPawnAttack = sq - 7;
        if (leftPawnAttack >= 0 && leftPawnAttack <= 63 && leftPawnAttack / 8 == startRank - 1)
        {
            if (pieceAt(leftPawnAttack).type == PieceType::Pawn && pieceAt(leftPawnAttack).color == opponentColor)
            {
                return true;
            }
        }
        if (rightPawnAttack >= 0 && rightPawnAttack <= 63 && rightPawnAttack / 8 == startRank - 1)
        {
            if (pieceAt(rightPawnAttack).type == PieceType::Pawn && pieceAt(rightPawnAttack).color == opponentColor)
            {
                return true;
            }
        }
    }

    int nextSquare;
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
            if (!isEmpty(nextSquare))
            {
                if (pieceAt(nextSquare).color == opponentColor && (pieceAt(nextSquare).type == PieceType::Bishop || pieceAt(nextSquare).type == PieceType::Queen))
                {
                    return true;
                }
                break;
            }
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
            if (!isEmpty(nextSquare))
            {
                if (pieceAt(nextSquare).color == opponentColor && (pieceAt(nextSquare).type == PieceType::Bishop || pieceAt(nextSquare).type == PieceType::Queen))
                {
                    return true;
                }
                break;
            }
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
            if (!isEmpty(nextSquare))
            {
                if (pieceAt(nextSquare).color == opponentColor && (pieceAt(nextSquare).type == PieceType::Bishop || pieceAt(nextSquare).type == PieceType::Queen))
                {
                    return true;
                }
                break;
            }
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
            if (!isEmpty(nextSquare))
            {
                if (pieceAt(nextSquare).color == opponentColor && (pieceAt(nextSquare).type == PieceType::Bishop || pieceAt(nextSquare).type == PieceType::Queen))
                {
                    return true;
                }
                break;
            }
        }
        else
        {
            break;
        }
    }

    // left
    nextSquare = sq;
    nextRank = startRank;
    while (true)
    {
        nextSquare -= 1;
        if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
        {
            if (!isEmpty(nextSquare))
            {
                if (pieceAt(nextSquare).color == opponentColor && (pieceAt(nextSquare).type == PieceType::Rook || pieceAt(nextSquare).type == PieceType::Queen))
                {
                    return true;
                }
                break;
            }
        }
        else
        {
            break;
        }
    }

    // right
    nextSquare = sq;
    nextRank = startRank;
    while (true)
    {
        nextSquare += 1;
        if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
        {
            if (!isEmpty(nextSquare))
            {
                if (pieceAt(nextSquare).color == opponentColor && (pieceAt(nextSquare).type == PieceType::Rook || pieceAt(nextSquare).type == PieceType::Queen))
                {
                    return true;
                }
                break;
            }
        }
        else
        {
            break;
        }
    }

    // up
    nextSquare = sq;
    nextRank = startRank;
    while (true)
    {
        nextSquare += 8;
        nextRank += 1;
        if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
        {
            if (!isEmpty(nextSquare))
            {
                if (pieceAt(nextSquare).color == opponentColor && (pieceAt(nextSquare).type == PieceType::Rook || pieceAt(nextSquare).type == PieceType::Queen))
                {
                    return true;
                }
                break;
            }
        }
        else
        {
            break;
        }
    }

    // down
    nextSquare = sq;
    nextRank = startRank;
    while (true)
    {
        nextSquare -= 8;
        nextRank -= 1;
        if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
        {
            if (!isEmpty(nextSquare))
            {
                if (pieceAt(nextSquare).color == opponentColor && (pieceAt(nextSquare).type == PieceType::Rook || pieceAt(nextSquare).type == PieceType::Queen))
                {
                    return true;
                }
                break;
            }
        }
        else
        {
            break;
        }
    }

    // knight
    nextSquare = sq + 6;
    nextRank = startRank + 1;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
    {
        if (!isEmpty(nextSquare))
        {
            if (pieceAt(nextSquare).color == opponentColor && pieceAt(nextSquare).type == PieceType::Knight)
            {
                return true;
            }
        }
    }

    nextSquare = sq + 10;
    nextRank = startRank + 1;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
    {
        if (!isEmpty(nextSquare))
        {
            if (pieceAt(nextSquare).color == opponentColor && pieceAt(nextSquare).type == PieceType::Knight)
            {
                return true;
            }
        }
    }

    nextSquare = sq + 15;
    nextRank = startRank + 2;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
    {
        if (!isEmpty(nextSquare))
        {
            if (pieceAt(nextSquare).color == opponentColor && pieceAt(nextSquare).type == PieceType::Knight)
            {
                return true;
            }
        }
    }

    nextSquare = sq + 17;
    nextRank = startRank + 2;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
    {
        if (!isEmpty(nextSquare))
        {
            if (pieceAt(nextSquare).color == opponentColor && pieceAt(nextSquare).type == PieceType::Knight)
            {
                return true;
            }
        }
    }

    nextSquare = sq - 6;
    nextRank = startRank - 1;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
    {
        if (!isEmpty(nextSquare))
        {
            if (pieceAt(nextSquare).color == opponentColor && pieceAt(nextSquare).type == PieceType::Knight)
            {
                return true;
            }
        }
    }

    nextSquare = sq - 10;
    nextRank = startRank - 1;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
    {
        if (!isEmpty(nextSquare))
        {
            if (pieceAt(nextSquare).color == opponentColor && pieceAt(nextSquare).type == PieceType::Knight)
            {
                return true;
            }
        }
    }

    nextSquare = sq - 15;
    nextRank = startRank - 2;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
    {
        if (!isEmpty(nextSquare))
        {
            if (pieceAt(nextSquare).color == opponentColor && pieceAt(nextSquare).type == PieceType::Knight)
            {
                return true;
            }
        }
    }

    nextSquare = sq - 17;
    nextRank = startRank - 2;
    if (nextSquare <= 63 && nextSquare >= 0 && nextSquare / 8 == nextRank)
    {
        if (!isEmpty(nextSquare))
        {
            if (pieceAt(nextSquare).color == opponentColor && pieceAt(nextSquare).type == PieceType::Knight)
            {
                return true;
            }
        }
    }

    // en passant
    int enPassantAttackLeft = sq - 1;
    int enPassantAttackRight = sq + 1;
    int enPassantTarget;
    if (pieceAt(sq).type == PieceType::Pawn)
    {
        if (pieceAt(sq).color == Color::White)
        {
            enPassantTarget = sq - 8;
        }
        else
        {
            enPassantTarget = sq + 8;
        }
        if (enPassantAttackLeft <= 63 && enPassantAttackLeft >= 0 && enPassantAttackLeft / 8 == startRank)
        {

            if (pieceAt(enPassantAttackLeft).type == PieceType::Pawn && pieceAt(enPassantAttackLeft).color == opponentColor && enPassantSquare() == enPassantTarget)
            {
                return true;
            }
        }
        if (enPassantAttackRight <= 63 && enPassantAttackRight >= 0 && enPassantAttackRight / 8 == startRank)
        {
            if (pieceAt(enPassantAttackRight).type == PieceType::Pawn && pieceAt(enPassantAttackRight).color == opponentColor && enPassantSquare() == enPassantTarget)
            {
                return true;
            }
        }
    }

    return false;
}

int Board::kingLocation(Color color) const
{
    for (int sq = 0; sq < 64; sq++)
    {
        if (pieceAt(sq).type == PieceType::King && pieceAt(sq).color == color)
        {
            return sq;
        }
    }
    throw std::runtime_error("King not found on board");
}

CastlingAllowed Board::castlingAllowed() const
{
    return state.castling;
}

void Board::printHistoryDebug() const
{
    for (BoardArray entry : history.arrayHistory)
    {
        Board temp = Board();
        temp.squares = entry;
        temp.print();
    }
}

bool Board::kingInCheck(Color color) const
{
    int location = kingLocation(color);
    if (isPieceAttacked(location))
    {
        return true;
    }
    return false;
}

bool Board::kingInCheck() const
{
    int location = kingLocation(sideToMove());
    if (isPieceAttacked(location))
    {
        return true;
    }
    return false;
}

void Board::makeMove(Move move)
{
    history.stateHistory.push_back(state);
    history.arrayHistory.push_back(squares);
    Color movingColor = squares[move.from].color;

    if (!(squares[move.from].type == PieceType::Pawn))
    {
        state.halfMoveClock += 1;
    }
    else
    {
        state.halfMoveClock = 0;
    }

    switch (move.type)
    {
    case (MoveType::Standard):
        squares[move.to] = squares[move.from];
        squares[move.from] = Piece{PieceType::None, Color::None};
        break;
    case (MoveType::Capture):
        squares[move.to] = squares[move.from];
        squares[move.from] = Piece{PieceType::None, Color::None};
        break;
    case (MoveType::DoublePawnPush):
        squares[move.to] = squares[move.from];
        squares[move.from] = Piece{PieceType::None, Color::None};
        if (movingColor == Color::White)
        {
            state.enPassantSquare = move.to - 8;
        }
        else
        {
            state.enPassantSquare = move.to + 8;
        }
        break;
    case (MoveType::EnPassant):
        squares[move.to] = squares[move.from];
        squares[move.from] = Piece{PieceType::None, Color::None};
        if (movingColor == Color::White)
        {
            squares[move.to - 8] = Piece{PieceType::None, Color::None};
        }
        else
        {
            squares[move.to + 8] = Piece{PieceType::None, Color::None};
        }
        break;
    case (MoveType::KingCastle):
        if (movingColor == Color::White)
        {
            squares[4] = Piece{PieceType::None, Color::None};
            squares[5] = Piece{PieceType::Rook, Color::White};
            squares[6] = Piece{PieceType::King, Color::White};
            squares[7] = Piece{PieceType::None, Color::None};
        }
        else
        {
            squares[60] = Piece{PieceType::None, Color::None};
            squares[61] = Piece{PieceType::Rook, Color::Black};
            squares[62] = Piece{PieceType::King, Color::Black};
            squares[63] = Piece{PieceType::None, Color::None};
        }
        state.castling.whiteKingSide = false;
        state.castling.whiteQueenSide = false;
        break;
    case (MoveType::QueenCastle):
        if (movingColor == Color::White)
        {
            squares[0] = Piece{PieceType::None, Color::None};
            squares[2] = Piece{PieceType::King, Color::White};
            squares[3] = Piece{PieceType::Rook, Color::White};
            squares[4] = Piece{PieceType::None, Color::None};
        }
        else
        {
            squares[56] = Piece{PieceType::None, Color::None};
            squares[58] = Piece{PieceType::King, Color::Black};
            squares[59] = Piece{PieceType::Rook, Color::Black};
            squares[60] = Piece{PieceType::None, Color::None};
        }
        state.castling.blackKingSide = false;
        state.castling.blackQueenSide = false;
        break;
    case (MoveType::Promotion):
        squares[move.to] = Piece{move.promotion, movingColor};
        squares[move.from] = Piece{PieceType::None, Color::None};
        break;
    }
    state.enPassantSquare = -1;
    if (state.sideToMove == Color::White)
    {
        state.sideToMove = Color::Black;
    }
    else
    {
        state.sideToMove = Color::White;
        state.fullMoveNumber += 1;
    }
}

void Board::unMakeMove()
{
    state = history.stateHistory.back();
    squares = history.arrayHistory.back();
    history.stateHistory.pop_back();
    history.arrayHistory.pop_back();
}

std::string Board::indexToCoords(int sq)
{
    int rank = sq / 8;
    int file = sq % 8;

    switch (file)
    {
    case (0):
        return "a" + std::to_string(rank + 1);
    case (1):
        return "b" + std::to_string(rank + 1);
    case (2):
        return "c" + std::to_string(rank + 1);
    case (3):
        return "d" + std::to_string(rank + 1);
    case (4):
        return "e" + std::to_string(rank + 1);
    case (5):
        return "f" + std::to_string(rank + 1);
    case (6):
        return "g" + std::to_string(rank + 1);
    case (7):
        return "h" + std::to_string(rank + 1);
    default:
        return "";
    }
}
