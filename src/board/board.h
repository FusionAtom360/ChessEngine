#pragma once

#include <vector>
#include <array>
#include <string>

enum struct Color
{
    None,
    White,
    Black
};

std::string toString(Color color);


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
    int from = 0;
    int to = 0;
    MoveType type = MoveType::Standard;
    PieceType promotion = PieceType::None;
};

struct ScoredMove
{
    Move move;
    int score;
};

using BoardArray = std::array<Piece, 64>;
using MoveList = std::vector<Move>;
using ScoredMoveList = std::vector<ScoredMove>;

struct UndoHistory
{
    std::vector<GameState> stateHistory;
    std::vector<BoardArray> arrayHistory;
    UndoHistory() : stateHistory(), arrayHistory() {}
};

Color oppositeColor(const Color &color);

class Board
{
private:
    GameState state;
    BoardArray squares;
    UndoHistory history;

    char pieceToChar(const Piece p) const;

public:
    Board();

    std::string print() const;

    void setStateFEN(std::string fen);

    Piece pieceAt(int sq) const;

    Color sideToMove() const;

    bool isEmpty(int sq) const;

    int coordsToIndex(std::string coords);

    int enPassantSquare() const;

    bool isPieceAttacked(int sq) const;

    int kingLocation(Color color) const;

    CastlingAllowed castlingAllowed() const;

    void printHistoryDebug() const;

    bool kingInCheck(Color color) const;

    bool kingInCheck() const;

    void makeMove(Move move);

    void unMakeMove();

    std::string indexToCoords(int sq);

    std::string FEN();

    int halfMoveCounter();

    std::string toString(Move move);
};
