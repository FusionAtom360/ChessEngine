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

void generatePawnCaptureMoves(const Board &board, int sq, MoveList &moves);

void generateRookCaptureMoves(const Board &board, int sq, MoveList &moves);

void generateKnightCaptureMoves(const Board &board, int sq, MoveList &moves);

void generateBishopCaptureMoves(const Board &board, int sq, MoveList &moves);

void generateQueenCaptureMoves(const Board &board, int sq, MoveList &moves);

void generateKingCaptureMoves(const Board &board, int sq, MoveList &moves);

MoveList generatePseudoLegalCaptureMoves(const Board &board);

MoveList generateCaptureMoves(Board &board);

int pieceValue(PieceType type);

int scoreMove(const Move &m, const Board &board);

MoveList generateOrderedMoves(Board &board);

MoveList generateOrderedCaptureMoves(Board &board);
