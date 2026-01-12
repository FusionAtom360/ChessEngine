#pragma once

#include "../board/board.h"

void generatePawnMoves(const Board &board, int sq, MoveList &moves);

void generateRookMoves(const Board &board, int sq, MoveList &moves);

void generateKnightMoves(const Board &board, int sq, MoveList &moves);

void generateBishopMoves(const Board &board, int sq, MoveList &moves);

void generateQueenMoves(const Board &board, int sq, MoveList &moves);

void generateKingMoves(const Board &board, int sq, MoveList &moves);

MoveList generatePseudoLegalMoves(const Board &board);

MoveList generateLegalMoves(Board &board);
