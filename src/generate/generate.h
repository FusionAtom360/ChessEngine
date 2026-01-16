#pragma once

#include "board/board.h"

void generatePawnMoves(const Board &board, const int &sq, MoveList &moves);

template <int numDirections>
void generateSliderMoves(const Board &board, const int &sq, MoveList &moves, const std::array<int, numDirections> &directions);

void generateKnightMoves(const Board &board, const int &sq, MoveList &moves);

void generateKingMoves(const Board &board, const int &sq, MoveList &moves);

MoveList generatePseudoLegalMoves(const Board &board);

MoveList generateLegalMoves(Board &board);

void generatePawnCaptureMoves(const Board &board, const int &sq, MoveList &moves);

template <int numDirections>
void generateSliderCaptureMoves(const Board &board, const int &sq, MoveList &moves, const std::array<int, numDirections> &directions);

void generateKnightCaptureMoves(const Board &board, const int &sq, MoveList &moves);

void generateKingCaptureMoves(const Board &board, const int &sq, MoveList &moves);

MoveList generatePseudoLegalCaptureMoves(const Board &board);

MoveList generateCaptureMoves(Board &board);

int pieceValue(PieceType type);

int scoreMoveStatic(const Move &m, const Board &board);

MoveList generateOrderedMoves(Board &board);

MoveList generateOrderedCaptureMoves(Board &board);
