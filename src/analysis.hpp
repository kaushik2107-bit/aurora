#ifndef ANALYSIS_H
#define ANALYSIS_H
#include <chrono>
#include <cmath>
#include "../helpers/help.hpp"
#include "./engine.hpp"
#include "../constants/analysis_constants.hpp"
#include "../helpers/hash.hpp"

#ifdef __GNUC__ // Check if using GCC

#include <popcntintrin.h> // For GCC's popcnt and lzcnt intrinsics

#define CTZ(value) __builtin_ctzll(value)
#define CLZ(value) __builtin_clzll(value)
#define POPCOUNT(value) __builtin_popcountll(value)

#elif defined(_MSC_VER) // Check if using MSVC

#include <intrin.h> // For MSVC intrinsics

#define CTZ(value) _tzcnt_u64(value)
#define CLZ(value) _lzcnt_u64(value)
#define POPCOUNT(value) __popcnt64(value)

#else
#error "Unsupported compiler"
#endif

class Analysis: public Engine {
private:
    int count_material(bool);
    double force_king_to_corner_endgame(int playerKing, int opponentKing, float endgameWeight);
    double check_piece_square_table(bool, float);
    std::unordered_map<uint64_t, std::tuple<std::vector<std::string>, double, int, int, int>> transposition_table;
public:
    Analysis(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"){
        this->fen = fen;
        get_info();
        this->player = !player;
        generate_attack_bitboards();
        this->player = !player;
    }
    double evaluate_position();
    std::tuple<std::vector<std::string>, double, int> search(int, double, double, std::chrono::_V2::system_clock::time_point, int, std::string, bool startDepth);
    std::tuple<std::vector<std::string>, double, int> iterative_deepening(int, int);
    double searchAllCaptures(int depth, double alpha, double beta);
    std::vector<std::string> generate_capture_moves();
    void notifyNewGame(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
        this->fen = fen;
        get_info();
        this->player = !player;
        generate_attack_bitboards();
        this->player = !player;
    }
};

double Analysis::evaluate_position() {
    int whiteEval = 0, blackEval = 0;
    int totalPieces = POPCOUNT(merged_bitboards);
    float weight =  1.0 - (totalPieces / 16.0);

    // counting material
    whiteEval += count_material(true);
    blackEval += count_material(false);

    // piece-square table
    whiteEval += check_piece_square_table(true, weight);
    blackEval += check_piece_square_table(false, weight);

    // forcing king to corner endgame
    if (player) {
        int whiteKing = CTZ(bitboard['K']);
        int blackKing = CTZ(bitboard['k']);
        whiteEval += force_king_to_corner_endgame(whiteKing, blackKing, weight);
    } else {
        int whiteKing = CTZ(bitboard['K']);
        int blackKing = CTZ(bitboard['k']);
        blackEval += force_king_to_corner_endgame(blackKing, whiteKing, weight);
    }

    int evaluation = whiteEval - blackEval;
    return (double)evaluation * (player ? 1 : -1);
}

double Analysis::check_piece_square_table(bool player, float endgameWeight) {
    int evaluation = 0;
    if (player) {
        for (auto x: bitboard) {
            char piece = x.first;
            uint64_t board = x.second;
            int tmp = board;
            if (piece == 'Q') {
                while (tmp) {
                    int initial_square = CTZ(tmp);
                    tmp &= ~(1ull << initial_square);
                    evaluation += whiteEvalQueen[initial_square];
                }
            } else if (piece == 'R') {
                 while (tmp) {
                    int initial_square = CTZ(tmp);
                    tmp &= ~(1ull << initial_square);
                    evaluation += whiteEvalRook[initial_square];
                }
            } else if (piece == 'B') {
                 while (tmp) {
                    int initial_square = CTZ(tmp);
                    tmp &= ~(1ull << initial_square);
                    evaluation += whiteEvalBishop[initial_square];
                }
            } else if (piece == 'N') {
                 while (tmp) {
                    int initial_square = CTZ(tmp);
                    tmp &= ~(1ull << initial_square);
                    evaluation += whiteEvalKnight[initial_square];
                }
            } else if (piece == 'P') {
                 while (tmp) {
                    int initial_square = CTZ(tmp);
                    tmp &= ~(1ull << initial_square);
                    evaluation += whiteEvalPawn[initial_square];
                }
            } else if (piece == 'K') {
                 while (tmp) {
                    int initial_square = CTZ(tmp);
                    tmp &= ~(1ull << initial_square);
                    evaluation += (endgameWeight > 0.7 ? whiteEvalKingEnd[initial_square] : whiteEvalKing[initial_square]);
                }
            }
        }
    } else {
        for (auto x: bitboard) {
            char piece = x.first;
            uint64_t board = x.second;
            int tmp = board;
            if (piece == 'q') {
                while (tmp) {
                    int initial_square = CTZ(tmp);
                    tmp &= ~(1ull << initial_square);
                    evaluation += blackEvalQueen[initial_square];
                }
            } else if (piece == 'r') {
                 while (tmp) {
                    int initial_square = CTZ(tmp);
                    tmp &= ~(1ull << initial_square);
                    evaluation += blackEvalRook[initial_square];
                }
            } else if (piece == 'b') {
                 while (tmp) {
                    int initial_square = CTZ(tmp);
                    tmp &= ~(1ull << initial_square);
                    evaluation += blackEvalBishop[initial_square];
                }
            } else if (piece == 'n') {
                 while (tmp) {
                    int initial_square = CTZ(tmp);
                    tmp &= ~(1ull << initial_square);
                    evaluation += blackEvalKnight[initial_square];
                }
            } else if (piece == 'p') {
                 while (tmp) {
                    int initial_square = CTZ(tmp);
                    tmp &= ~(1ull << initial_square);
                    evaluation += blackEvalPawn[initial_square];
                }
            } else if (piece == 'k') {
                 while (tmp) {
                    int initial_square = CTZ(tmp);
                    tmp &= ~(1ull << initial_square);
                    evaluation += (endgameWeight > 0.7 ? blackEvalKingEnd[initial_square] : blackEvalKing[initial_square]);
                }
            }
        }
    }

    return evaluation;
}

double Analysis::force_king_to_corner_endgame(int playerKing, int opponentKing, float endgameWeight) {
    double evaluation = 0;
    
    int opponentKingRank = opponentKing / 8;
    int opponentKingFile = opponentKing % 8;

    int opponentKingDistToCentreRank = std::max(3 - opponentKingRank, opponentKingRank - 4);
    int opponentKingDistToCentreFile = std::max(3 - opponentKingFile, opponentKingFile - 4);

    int opponentKingDistFromCentre = opponentKingDistToCentreFile + opponentKingDistToCentreRank;
    
    evaluation += opponentKingDistFromCentre;

    int playerKingRank = playerKing / 8;
    int playerKingFile = playerKing % 8;

    int distBetweenKingsFile = std::abs(playerKingFile - opponentKingFile);
    int distBetweenKingsRank = std::abs(playerKingRank - opponentKingRank);

    int distBetweenKings = distBetweenKingsFile + distBetweenKingsRank;
    evaluation += 14 - distBetweenKings;

    return evaluation*60*endgameWeight;
}

int Analysis::count_material(bool color) {
    int material = 0;
    for (auto x: bitboard) {
        char piece = x.first;
        uint64_t board = x.second;
        int count = POPCOUNT(board);
        if (!color && piece >= 'a') material += count*piece_score[piece];
        else if (color && piece <= 'Z') material += count*piece_score[piece + 32];  
    }
    return material;
}

std::vector<std::string> Analysis::generate_capture_moves() {
    generate_legal_moves();
    std::vector<std::string> lm = legal_moves;
    std::vector<std::string> capture_moves;
    uint64_t opponent_bitboards = player ? black_bitboards : white_bitboards;
    for (auto move: lm) {
        std::string initial = move.substr(0,2);
        std::string final = move.substr(2,2);
        if (opponent_bitboards & (1ull << convert_to_number(final))) {
            capture_moves.push_back(move);
        }
    }
    capture_moves = move_ordering_captures(capture_moves);
    return capture_moves;
}

double Analysis::searchAllCaptures(int depth, double alpha, double beta) {
    double evaluation = evaluate_position();
    if (evaluation >= beta) return beta;
    alpha = std::max(alpha, evaluation);

    if (depth == 0) return alpha;

    std::vector<std::string> captureMoves = generate_capture_moves();

    for (auto move: captureMoves) {
        push(move);
        evaluation = -searchAllCaptures(depth-1, -beta, -alpha);
        pop();

        if (evaluation >= beta) return beta;
        alpha = std::max(alpha, evaluation);
    }
    return alpha;
}

std::tuple<std::vector<std::string>, double, int> Analysis::search(int depth, double alpha, double beta, std::chrono::_V2::system_clock::time_point start_time, int time_limit, std::string prevMove, bool startDepth = false) {
    if (!startDepth) generate_legal_moves();
    uint64_t hash = get_zobrist_hash(bitboard, player, castling_rights, en_passant);
    std::vector<std::string> lm = legal_moves;
    double alphaOrig = alpha;

    if (transposition_table.count(hash)) {
        std::vector<std::string> moves; double score; int nodes, type, dpth;
        std::tie(moves, score, nodes, type, dpth) = transposition_table[hash];
        if (dpth >= depth) {
            if (type == 0) return { moves, score, 0 };
            else if (type == 1) beta = std::min(beta, score);
            else alpha = std::max(alpha, score);
        }
        if (alpha >= beta) return { moves, score, 0 };
    }

    if (depth == 0) {
        double value = searchAllCaptures(4, alpha, beta);
        // transposition_table[hash] = {{}, value, 1, 0, depth};
        return {{}, value, 1};
    }
    
    if (lm.size() == 0) {
        if (is_check()) {
            // transposition_table[hash] = {{}, -9999999, 1, 0, depth};
            return {{}, -9999999, 1};
        }
        // transposition_table[hash] = {{}, 0, 1, 0, depth};
        return {{}, 0, 1};
    } 

    if (is_threefold_repetition || is_fifty_moves) {
        // transposition_table[hash] = {{}, 0, 1, 0, depth};
        return {{}, 0, 1};
    }

    int nodeCount = 0;
    double bestValue = -INFINITY;
    std::string bestMove = lm[0];
    std::vector<std::string> bmvs;
    for (auto move: lm) {
        std::vector<std::string> mvm; double evaluation; int nodes;
        push(move);
        std::tie(mvm, evaluation, nodes) = search(depth - 1, -beta, -alpha, start_time, time_limit, move);
        pop();
        evaluation = -evaluation;
        nodeCount += nodes;

        if (evaluation > bestValue) {
            bestValue = evaluation;
            bestMove = move;
            bmvs = mvm;
        }

        alpha = std::max(alpha, evaluation);

        if (alpha >= beta) break;

        // if (evaluation >= beta) {
        //     bmvs.insert(bmvs.begin(), bestMove);
        //     transposition_table[hash] = {bmvs, beta, nodeCount, 2, depth};
        //     return {bmvs, beta, nodeCount};
        // }

        // if (evaluation > alpha) {
        //     alpha = evaluation;
        //     bestMove = move;
        //     bmvs = mvm;
        // }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_time).count();
        if (duration > time_limit) {
            return {{}, 10000000, nodeCount};
        }
    }
    bmvs.insert(bmvs.begin(), bestMove);
    if (bestValue >= beta) {
        transposition_table[hash] = {bmvs, alpha, nodeCount, 2, depth};
    } else if (bestValue <= alpha) {
        transposition_table[hash] = {bmvs, alpha, nodeCount, 1, depth};
    } else {
        transposition_table[hash] = {bmvs, alpha, nodeCount, 0, depth};
    }
    return {bmvs, alpha, nodeCount};
}


std::tuple<std::vector<std::string>, double, int> Analysis::iterative_deepening(int depth, int time_limit) {
    std::vector<std::string> bestMove; double bestEval; std::vector<std::string> move; double evaluation; int nodes;
    auto start = std::chrono::high_resolution_clock::now();
    for (int current_depth=1; current_depth<=depth; current_depth++) {
        generate_legal_moves();
        if (current_depth != 1 && legal_moves.size() && bestMove.size()) {
            auto it = std::find(legal_moves.begin(), legal_moves.end(), bestMove[0]);
            if (it != legal_moves.end()) {
                legal_moves.erase(it);
                legal_moves.insert(legal_moves.begin(), bestMove[0]);
            }
        }
        std::tie(move, evaluation, nodes) = search(current_depth, -INFINITY, INFINITY, start, time_limit, "", true);
        auto end = std::chrono::high_resolution_clock::now();
        if (move.size() && evaluation != 10000000) {bestMove = move; bestEval = evaluation;}
        // checking the previous depth's good move first
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // std::cout << "   Depth:" << current_depth << "      evaluation: " << bestEval <<  "     Time: " << (duration/1000.0) << " seconds" << "     nodes: " << nodes << std::endl;
        // for (auto x: bestMove) std::cout << x << " ";
        // std::cout << std::endl;
        if (duration > time_limit) break;
    }
    return {bestMove, bestEval, nodes};
}

#endif