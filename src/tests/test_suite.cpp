#include "test_suite.h"
#include "tests.h"
#include "board/board.h"
#include "generate/generate.h"
#include "evaluate/evaluate.h"
#include <iostream>
#include <stdexcept>

Move parseMove(const std::string &moveStr, Board &board);

static int testsRun = 0;
static int testsFailed = 0;

static void report(bool ok, const std::string &name)
{
    ++testsRun;
    if (ok)
        std::cout << "[PASS] " << name << std::endl;
    else
    {
        ++testsFailed;
        std::cout << "[FAIL] " << name << std::endl;
    }
}

static void test_initial_move_count()
{
    Board board;
    MoveList moves = generateLegalMoves(board);
    report(moves.size() == 20, "Initial position has 20 legal moves");
}

static void test_perft_initial()
{
    Board board;
    // known perft counts for initial position
    int d1 = perft(board, 1, false);
    int d2 = perft(board, 2, false);
    int d3 = perft(board, 3, false);

    report(d1 == 20, "Perft depth 1 = 20");
    report(d2 == 400, "Perft depth 2 = 400");
    report(d3 == 8902, "Perft depth 3 = 8902");
}

static void test_make_unmake_restores_board()
{
    Board board;
    MoveList moves = generateOrderedMoves(board);
    if (moves.empty())
    {
        report(false, "Make/unmake: no moves to test");
        return;
    }

    Move m = moves[0];
    auto fromPiece = board.pieceAt(m.from);
    auto toPiece = board.pieceAt(m.to);

    board.makeMove(m);
    board.unMakeMove();

    auto fromPiece2 = board.pieceAt(m.from);
    auto toPiece2 = board.pieceAt(m.to);

    bool ok = (fromPiece.type == fromPiece2.type && fromPiece.color == fromPiece2.color &&
               toPiece.type == toPiece2.type && toPiece.color == toPiece2.color);

    report(ok, "Make/unmake restores pieces");
}

static void test_simple_evaluation_smoke()
{
    Board board;
    int score = evaluate(board);
    // Just ensure evaluation returns a finite integer; starting position often near 0
    report(score > -100000 && score < 100000, "Evaluate returns plausible value for start pos");
}

static void test_fen_serialization()
{
    Board board;
    report(board.getFEN() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
           "FEN serialization preserves initial position");
}

static void test_invalid_fen_is_rejected()
{
    Board board;
    bool rejected = false;
    try
    {
        board.setFEN("8R/8/8/8/8/8/8/8 w - - 0 1");
    }
    catch (const std::invalid_argument &)
    {
        rejected = true;
    }

    report(rejected, "Invalid FEN piece placement is rejected");
}

static void test_castling_while_in_check_is_rejected()
{
    Board board;
    board.setFEN("4k3/8/8/8/8/8/4r3/R3K2R w KQ - 0 1");
    MoveList moves = generateLegalMoves(board);
    bool canCastle = false;
    for (const Move &move : moves)
    {
        canCastle = canCastle || move.type == MoveType::KingCastle || move.type == MoveType::QueenCastle;
    }

    report(!canCastle, "Castling while in check is rejected");
}

static void test_capture_resets_halfmove_clock()
{
    Board board;
    board.setFEN("4k3/8/8/8/8/8/r7/R3K2K w - - 99 1");
    board.makeMove({0, 8, MoveType::Capture});

    report(board.halfMoveCounter() == 0, "Capture resets halfmove clock");
}

static void test_fifty_move_rule_threshold()
{
    Board board;
    board.setFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 99 1");

    report(!gameOver(board), "Fifty-move rule waits for 100 half-moves");
}

static void test_timed_search_returns_legal_move_on_immediate_timeout()
{
    Board board;
    Move expected = findBestMove(board, 10, 0.0);
    MoveList legalMoves = generateLegalMoves(board);
    bool legal = false;
    for (const Move &move : legalMoves)
    {
        legal = legal || (move.from == expected.from && move.to == expected.to &&
                          move.type == expected.type && move.promotion == expected.promotion);
    }

    report(legal, "Timed search returns a legal move on immediate timeout");
}

static void test_iterative_search_preserves_legal_move()
{
    Board board;
    Move selected = findBestMove(board, 2, 1.0);
    MoveList legalMoves = generateLegalMoves(board);
    bool legal = false;
    for (const Move &move : legalMoves)
    {
        legal = legal || (move.from == selected.from && move.to == selected.to &&
                          move.type == selected.type && move.promotion == selected.promotion);
    }

    report(legal, "Iterative search preserves a legal move");
}

static void test_illegal_user_move_is_rejected()
{
    Board board;
    bool rejected = false;
    try
    {
        parseMove("e2e5", board);
    }
    catch (const std::invalid_argument &)
    {
        rejected = true;
    }

    report(rejected, "Illegal user move is rejected");
}

static void test_fen_clears_en_passant_state()
{
    Board board;
    board.setFEN("4k3/8/8/8/8/8/8/4K3 w - e3 0 1");
    board.setFEN("4k3/8/8/8/8/8/8/4K3 w - - 0 1");

    report(board.enPassantSquare() == -1, "FEN without en passant clears prior state");
}

static void test_empty_move_history_is_rejected()
{
    Board board;
    bool rejected = false;
    try
    {
        board.unMakeMove();
    }
    catch (const std::logic_error &)
    {
        rejected = true;
    }

    report(rejected, "Undo with empty history is rejected");
}

static void test_move_ordering_prioritizes_high_value_capture()
{
    Board board;
    board.setFEN("4k3/3p4/8/8/8/1q6/P7/3QK3 w - - 0 1");
    MoveList moves = generateOrderedMoves(board);

    report(!moves.empty() && moves[0].from == board.coordsToIndex("a2") &&
               moves[0].to == board.coordsToIndex("b3"),
           "Move ordering prioritizes high-value capture");
}

static void test_move_ordering_prioritizes_check()
{
    Board board;
    board.setFEN("4k3/8/8/8/8/8/3Q4/4K3 w - - 0 1");
    MoveList moves = generateOrderedMoves(board);

    report(!moves.empty() && moves[0].from == board.coordsToIndex("d2") &&
               moves[0].to == board.coordsToIndex("d8"),
           "Move ordering prioritizes checking move");
}

void runAllTests()
{
    std::cout << "Running unit tests..." << std::endl;

    test_initial_move_count();
    test_perft_initial();
    test_make_unmake_restores_board();
    test_simple_evaluation_smoke();
    test_fen_serialization();
    test_invalid_fen_is_rejected();
    test_castling_while_in_check_is_rejected();
    test_capture_resets_halfmove_clock();
    test_fifty_move_rule_threshold();
    test_timed_search_returns_legal_move_on_immediate_timeout();
    test_iterative_search_preserves_legal_move();
    test_illegal_user_move_is_rejected();
    test_fen_clears_en_passant_state();
    test_empty_move_history_is_rejected();
    test_move_ordering_prioritizes_high_value_capture();
    test_move_ordering_prioritizes_check();

    std::cout << "\nTests run: " << testsRun << ", failed: " << testsFailed << std::endl;

    if (testsFailed == 0)
        std::cout << "All tests passed." << std::endl;
    else
        std::cout << "Some tests failed." << std::endl;
}
