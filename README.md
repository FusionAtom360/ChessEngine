# Chess Engine (C++)

A simple yet functional chess engine implemented in C++, featuring move generation, board evaluation, and search algorithms with alpha–beta pruning. Designed as a modular engine suitable for experimentation with AI, evaluation functions, and engine optimizations.

## Features

- Complete board representation

- 64-square array (`BoardArray`) with `Piece` structs

- Full tracking of game state (`GameState`) including castling rights, en passant, half-move clock, and full-move number

- Move generation

- Pseudo-legal and legal moves

- Support for all chess rules: standard moves, captures, promotions, en passant, castling

- Specialized move generation for pawns, knights, sliders (rook, bishop, queen), and king

- Ordered move lists for alpha–beta efficiency

- Search algorithms

- Negamax search with alpha–beta pruning

- Time-limited search support

- Quiescence search to reduce horizon effect

- Evaluation

- Material and positional evaluation using Piece-Square Tables (PSTs)

- Separate PSTs for all pieces including king midgame

- Easy extension for endgame PSTs

- Undo functionality

- Full history of board states for efficient move retraction

- Performance testing

- `perft` for move counting and validation

## Data Structures

- `Piece`: `type` (Pawn, Knight, etc.) and `color` (White, Black, None)

- `Move`: `from`, `to`, `type` (Standard, Capture, Promotion, etc.), and optional promotion piece

- `Board`: Contains `BoardArray`, `GameState`, and undo history

- `UndoHistory`: Stores previous board arrays and game states for move retraction

## Usage

### Initialize a board

```cpp
Board board;

board.setFEN("startpos"); // Optional: use FEN string
```

### Generate moves

```cpp
MoveList legalMoves = generateLegalMoves(board);
MoveList captureMoves = generateCaptureMoves(board);
```

### Make and undo moves

```cpp
Move m = legalMoves[0];
board.makeMove(m);
// ...evaluate or search...
board.unMakeMove();
```

### Search for best move

```cpp
int depth = 4;
Move best = findBestMove(board, depth);
```

### Evaluation


```cpp
int score = evaluate(board); // Material + PST evaluation
```

### Perft testing

```cpp
int nodes = perft(board, 4, true); // Count all legal moves to depth 4
```

## Engine Architecture

### Board Representation

- 64-element array

- Tracks piece type and color

- Maintains game state including castling, en passant, and clocks

### Move Generation

- Generates pseudo-legal moves first

- Filters moves that leave own king in check to produce legal moves

- Specialized functions for sliders, knights, pawns, and king

### Search

- Negamax recursive search with alpha–beta pruning

- Quiescence search for tactical stability

- Optional time-limited search

### Evaluation

- Material values per piece

- Piece-square tables (PSTs) for positional evaluation

- King safety considered via midgame PST

### Undo System

- Efficient push/pop of game state and board arrays

- Enables move retraction during search

## Piece-Square Tables (Midgame)

| Piece | Table       |
| ------ | ----------- |
| Pawn | `pawnPST[64]` |
| Knight | `knightPST[64]` |
| Bishop | `bishopPST[64]` |
| Rook |	`rookPST[64]` |
| Queen |	`queenPST[64]` |
| King |	`kingMidgamePST[64]` |

*Black’s perspective is mirrored automatically in evaluation.*

## Compilation

Requires C++17 or later.

```bash
g++ -std=c++17 -O2 -I include/ src/*.cpp -o chess_engine
```

## Future Improvements

- Endgame evaluation: separate king PST and tablebases

- Move ordering heuristics: history heuristic, killer moves, MVV-LVA

- Iterative deepening for progressive deep search

- Transposition tables to cache positions and improve speed

- Algebraic notation input for user-friendly CLI interaction

- UCI protocol implementation to interface with GUIs

## References

- [Chess Programming Wiki](https://www.chessprogramming.org/Main_Page)

- [ROCE - Roman's Own Chess Engine](http://www.rocechess.ch/rocee.html)
