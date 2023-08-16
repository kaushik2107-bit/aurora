#include <bits/stdc++.h>
#include "../src/analysis.h"

uint64_t rook_mask(int square) {
    uint64_t mask = 0ull;
    int rank = square / 8;
    int file = square % 8;

    for (int i=1; i<7; i++) {
        if (i != file) mask |= (1ull << (rank * 8 + i));
    }

    for (int i=1; i<7; i++) {
        if (i != rank) mask |= (1ull << (i*8 + file));
    }
    return mask;
}

signed main() {
    std::string fen = "7k/4Q3/8/8/8/8/1RK5/8 w - - 2 2";
    // std::string fen = "rnb1kbnr/pppp1ppp/4p3/8/7q/3P1P2/PPP1P1PP/RNBQKBNR w KQkq - 0 1";
    Analysis chess(fen);
    chess.get_info();
    auto start = std::chrono::high_resolution_clock::now();
    chess.generate_legal_moves();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "               Finding legal moves using brute force -> Time: " << duration << " microseconds" << std::endl;
    std::cout << std::endl;

    while (true) {
        chess.print_board();
        chess.generate_legal_moves();
        printNumberAsBoard(chess.get_player() == "White" ? chess.black_attacks : chess.white_attacks);
        std::cout << std::endl;
        std::cout << chess.get_fen() << std::endl;
        for(auto x: chess.legal_moves) std::cout << x << " ";
        std::cout << std::endl;
        std::string move;
        std::cin >> move;
        chess.push(move);
    }
}

