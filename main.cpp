#include <iostream>
#include <array>
#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <cassert>
#include <stdexcept>
#include <limits>

enum struct Color
{
    None,
    White,
    Black
};

enum struct PieceType
{
    None,
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

struct Piece
{
    PieceType type;
    Color color;
};

struct CastlingAllowed
{
    bool whiteKingSide;
    bool whiteQueenSide;
    bool blackKingSide;
    bool blackQueenSide;
};

struct GameState
{
    Color sideToMove;
    CastlingAllowed castling;
    int enPassantSquare;
    int halfMoveClock;
    int fullMoveNumber;
};

enum struct MoveType
{
    Standard,
    Capture,
    DoublePawnPush,
    KingCastle,
    QueenCastle,
    EnPassant,
    Promotion
};

struct Move
{
    int from;
    int to;
    MoveType type;
    PieceType promotion = PieceType::None;
};

using BoardArray = std::array<Piece, 64>;
using MoveList = std::vector<Move>;

struct UndoHistory
{
    std::vector<GameState> stateHistory;
    std::vector<BoardArray> arrayHistory;
    UndoHistory() : stateHistory(), arrayHistory() {}
};

Color oppositeColor(const Color &color)
{
    if (color == Color::White)
    {
        return Color::Black;
    }
    return Color::White;
}

class Board
{
private:
    GameState state;
    BoardArray squares;
    UndoHistory history;

    char pieceToChar(const Piece p) const
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

public:
    Board() : state(), squares(), history()
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

    void print() const
    {
        for (int rank = 7; rank >= 0; rank--)
        {
            for (int file = 0; file < 8; file++)
            {
                int sq = rank * 8 + file;
                std::cout << pieceToChar(squares[sq]) << " ";
            }
            std::cout << std::to_string(rank + 1) << "\n";
        }
        std::cout << "a b c d e f g h\n";

        std::cout << "Move: " << state.fullMoveNumber << " | ";
        if (state.sideToMove == Color::Black)
        {
            std::cout << "Black to play";
        }
        else
        {
            std::cout << "White to play";
        }

        std::cout << " | Castling: ";
        if (state.castling.whiteKingSide)
        {
            std::cout << "K";
        }
        if (state.castling.whiteQueenSide)
        {
            std::cout << "Q";
        }
        if (state.castling.blackKingSide)
        {
            std::cout << "k";
        }
        if (state.castling.blackQueenSide)
        {
            std::cout << "q";
        }

        std::cout << " | Half moves: " << state.halfMoveClock << " | En passant: " << state.enPassantSquare << "\n";
    }

    void setStateFEN(std::string fen)
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

    Piece pieceAt(int sq) const
    {
        assert(sq >= 0 && sq < 64);
        return squares[sq];
    }

    Color sideToMove() const
    {
        return state.sideToMove;
    }

    bool isEmpty(int sq) const
    {
        assert(sq >= 0 && sq < 64);
        return squares[sq].type == PieceType::None;
    }

    int coordsToIndex(std::string coords)
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

    int enPassantSquare() const
    {
        return state.enPassantSquare;
    }

    bool isPieceAttacked(int sq) const
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

    int kingLocation(Color color) const
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

    CastlingAllowed castlingAllowed() const
    {
        return state.castling;
    }

    void printHistoryDebug() const
    {
        for (BoardArray entry : history.arrayHistory)
        {
            Board temp = Board();
            temp.squares = entry;
            temp.print();
        }
    }

    bool kingInCheck(Color color) const
    {
        int location = kingLocation(color);
        if (isPieceAttacked(location))
        {
            return true;
        }
        return false;
    }

    bool kingInCheck() const
    {
        int location = kingLocation(sideToMove());
        if (isPieceAttacked(location))
        {
            return true;
        }
        return false;
    }

    void makeMove(Move move)
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

    void unMakeMove()
    {
        state = history.stateHistory.back();
        squares = history.arrayHistory.back();
        history.stateHistory.pop_back();
        history.arrayHistory.pop_back();
    }

    std::string indexToCoords(int sq)
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
        }
    }
};

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
