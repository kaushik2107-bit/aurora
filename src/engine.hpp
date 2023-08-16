#ifndef ENGINE_H
#define ENGINE_H
#include <iostream>
#include <cstdint>
#include <sstream>
#include "../lib/board.hpp"
#include "../lib/move.hpp"

class Engine: public Move, public Board {
public:
    void push(std::string);
    void pop();
    void generate_legal_moves();
    void move_ordering();
    std::vector<std::string> move_ordering_captures(std::vector<std::string>);
};

void Engine::generate_legal_moves() {
    legal_moves.clear();
    generate_pseudo_legal_moves();
    std::vector<std::string> ps_lm = pseudo_legal_moves;
    uint64_t attack_bitboards = player ? black_attacks : white_attacks;
    uint64_t king_check = player ? is_white_king_check : is_black_king_check;
    for (auto move: ps_lm) {
        std::string start = move.substr(0, 2);
        std::string end = move.substr(2, 2);
        int initial = convert_to_number(start);
        int final = convert_to_number(end);
        if (move == "e1g1" || move == "e1c1" || move == "e8g8" || move == "e8c8") {
            if ((bitboard['K'] & (1ull << initial)) || (bitboard['k'] & (1ull << initial))) {
                if (king_check) continue;
                if (attack_bitboards & (1ull << final)) continue;
                if (attack_bitboards & (1ull << ((initial + final)/2))) continue;
                legal_moves.push_back(move); 
                continue;
            }
        }
        
        if (king_check && is_double_check) {
            if ((bitboard[player ? 'K' : 'k'] & (1ull << initial)) && !(attack_bitboards & (1ull << final))) legal_moves.push_back(move);
        } else if (king_check) {
            int first_point = __builtin_ctzll(bitboard[player ? 'K' : 'k']);
            int second_point = checked_by;
            uint64_t btb = two_points[first_point][second_point];
            if (final == checked_by) {
                if ((attack_bitboards & (1ull << final)) && first_point != initial && !(pinned_piece_map.count(initial))) {
                    legal_moves.push_back(move);
                } else if (!(attack_bitboards & (1ull << final)) && !(pinned_piece_map.count(initial))) legal_moves.push_back(move);
            } else if (final == en_passant && (bitboard[player ? 'p' : 'P'] & (1ull << checked_by))) {
                legal_moves.push_back(move);
            } else if (bitboard[player ? 'K' : 'k'] & (1ull << initial)) {
                if (!(attack_bitboards & (1ull << final))) legal_moves.push_back(move);
            } else if (btb & (1ull << final) && !(pinned_piece_map.count(initial))) legal_moves.push_back(move);
            else if (final == en_passant) en_passant = 111;
        } else {
            if (pinned_piece_map.count(initial)) {
                int first_point = __builtin_ctzll(bitboard[player ? 'K' : 'k']);
                int second_point = pinned_piece_map[initial];
                uint64_t btb = two_points[first_point][second_point];
                if (btb & (1ull << final)) {
                    legal_moves.push_back(move);
                }
            } else {
                if (final == en_passant && (bitboard[player ? 'P' : 'p'] & (1ull << initial))) {
                    int first_point = en_passant + (player ? -8 : 8);
                    int second_point = initial;
                    if (first_point > second_point) std::swap(first_point, second_point);
                    uint64_t firstRookPoint = rook_mask_edges_divider[first_point*4 + 3];
                    uint64_t secondRookPoint = rook_mask_edges_divider[second_point*4 + 1];
                    uint64_t firstMask = merged_bitboards & firstRookPoint;
                    uint64_t secondMask = merged_bitboards & secondRookPoint;
                    int leftSquare = 63 - __builtin_clzll(firstMask);
                    int rightSquare = __builtin_ctzll(secondMask);
                    if ((bitboard[player ? 'K' : 'k'] & (1ull << leftSquare)) && (((bitboard[player ? 'r' : 'R'] & (1ull << rightSquare)) || (bitboard[player ? 'q' : 'Q'] & (1ull << rightSquare))))) {
                        en_passant = 111;
                        continue;
                    } else if((bitboard[player ? 'K' : 'k'] & (1ull << rightSquare)) && (((bitboard[player ? 'r' : 'R'] & (1ull << leftSquare)) || (bitboard[player ? 'q' : 'Q'] & (1ull << leftSquare))))) {
                        en_passant = 111;
                        continue;
                    } 

                    first_point = initial;
                    firstRookPoint = rook_mask_edges_divider[first_point*4];
                    secondRookPoint = rook_mask_edges_divider[first_point*4 + 2];
                    firstMask = merged_bitboards & firstRookPoint;
                    secondMask = merged_bitboards & secondRookPoint;
                    int topSquare = __builtin_ctzll(firstMask);
                    int bottomSquare = 63 - __builtin_clzll(secondMask);
                    if ((bitboard[player ? 'K' : 'k'] & (1ull << topSquare)) && ((bitboard[player ? 'r' : 'R'] & (1ull << bottomSquare)) || (bitboard[player ? 'q' : 'Q'] & (1ull << bottomSquare)))) {
                        en_passant = 111;
                    } else if ((bitboard[player ? 'K' : 'k'] & (1ull << bottomSquare)) && ((bitboard[player ? 'r' : 'R'] & (1ull << topSquare)) || (bitboard[player ? 'q' : 'Q'] & (1ull << topSquare)))) {
                        en_passant = 111;
                    } else legal_moves.push_back(move);
                } else if (bitboard[player ? 'K' : 'k'] & (1ull << initial)) {
                    if (!(attack_bitboards & (1ull << final)))legal_moves.push_back(move);
                } else legal_moves.push_back(move);
            }
        }   
    }

    if (legal_moves.size() == 0) {
        if (is_check()) {
            is_checkmate = true;
            is_stalemate = false;
        } else {
            is_checkmate = false;
            is_stalemate = true;   
        }
    } else {
        is_checkmate = false;
        is_stalemate = false;
    }
    move_ordering();
}

void Engine::move_ordering() {
    // Promotion Moves
        // Queen - 100
        // Rook - 70
        // Night - 40
        // Bishop - 35
    // Capture Moves
        // Captured Piece > Played Piece - 100
        // else - 50
    // Non capture moves
        // higher piece on attack squares bad move

    std::vector<std::string> ordered_moves;
    std::vector<std::pair<std::string, int> > score_moves;
    for (auto move: legal_moves) {
        std::string first_square = move.substr(0, 2);
        std::string second_square = move.substr(2, 2);
        int initial = convert_to_number(first_square);
        int final = convert_to_number(second_square);

        if (move.size() == 5) {
            // making promotion moves better
            if (move[move.size() - 1] == 'q') {
                if ((rook_check_squares & (1ull << final)) || (bishop_check_squares & (1ull << final))) score_moves.push_back({move, 450});
                else score_moves.push_back({move, 400});
            } else if (move[move.size() - 1] == 'r') {
                if (rook_check_squares & (1ull << final)) score_moves.push_back({move, 400});
                else score_moves.push_back({move, 370});
            } else if (move[move.size() - 1] == 'n') {
                if (knight_check_squares & (1ull << final)) score_moves.push_back({move, 330});
                else score_moves.push_back({move, 300});
            } else if (move[move.size() - 1] == 'b') {
                if (bishop_check_squares & (1ull << final)) score_moves.push_back({move, 260});
                score_moves.push_back({move, 230});
            }
        } else {
            uint64_t attack_bitboards = player ? black_attacks : white_attacks;

            char first_piece = '-';
            char second_piece = '-';

            for (auto x: bitboard) {
                if (x.second & (1ull << initial)) first_piece = x.first;
                if (x.second & (1ull << final)) second_piece = x.first;
            }

            if (second_piece != '-' && second_piece <= 'Z') second_piece += 32;
            if (first_piece <= 'Z') first_piece += 32;

            if (second_piece != '-') {
                if (piece_score[first_piece] < piece_score[second_piece]) {
                    score_moves.push_back({move, 100});
                } else if (piece_score[first_piece] == piece_score[second_piece]) {
                        if (second_piece == 'q') score_moves.push_back({move, 90});
                        else if (second_piece == 'r') score_moves.push_back({move, 85});
                        else if (second_piece == 'b' || second_piece == 'n') score_moves.push_back({move, 80});
                        else score_moves.push_back({move, 40});
                } else {
                    if (attack_bitboards & (1ull << final)) {
                        if (first_piece == 'q') score_moves.push_back({move, -200});
                        else if (first_piece == 'r') score_moves.push_back({move, -130});
                        else score_moves.push_back({move, -100});
                    } else {
                        if (second_piece == 'q') score_moves.push_back({move, 130});
                        else if (second_piece == 'r') score_moves.push_back({move, 125});
                        else if (second_piece == 'b' || second_piece == 'n') score_moves.push_back({move, 110});
                        else score_moves.push_back({move, 100});
                    }
                }
            } else {
                if (first_piece == 'q' && (attack_bitboards & (1ull << final))) {
                    score_moves.push_back({move, -50});
                } else if (first_piece == 'r' && (attack_bitboards & (1ull << final))) {
                    score_moves.push_back({move, -20});
                } else if (first_piece == 'n' && (attack_bitboards & (1ull << final))) {
                    score_moves.push_back({move, 0});
                } else if (first_piece == 'b' && (attack_bitboards & (1ull << final))) {
                    score_moves.push_back({move, 0});
                } else {
                    if (first_piece == 'q' && (rook_check_squares & (1ull << final)) || (bishop_check_squares & (1ull << final))) {
                        score_moves.push_back({move, 50});
                    } else if (first_piece == 'r' && (rook_check_squares & (1ull << final))) {
                        score_moves.push_back({move, 45});
                    } else if (first_piece == 'b' && (bishop_check_squares & (1ull << final))) {
                        score_moves.push_back({move, 45});
                    } else if (first_piece == 'n' && (knight_check_squares & (1ull << final))) {
                        score_moves.push_back({move, 40});
                    } else {
                        score_moves.push_back({move, 10});
                    }
                }
            }
        }
    }

    std::sort(score_moves.begin(), score_moves.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    for (auto x: score_moves) ordered_moves.push_back(x.first);
    legal_moves = ordered_moves;
}

std::vector<std::string> Engine::move_ordering_captures(std::vector<std::string> capture_moves) {
    // Promotion Moves
        // Queen - 100
        // Rook - 70
        // Night - 40
        // Bishop - 35
    // Capture Moves
        // Captured Piece > Played Piece - 100
        // else - 50
    // Non capture moves
        // higher piece on attack squares bad move

    std::vector<std::string> ordered_moves;
    std::vector<std::pair<std::string, int> > score_moves;
    for (auto move: capture_moves) {
        std::string first_square = move.substr(0, 2);
        std::string second_square = move.substr(2, 2);
        int initial = convert_to_number(first_square);
        int final = convert_to_number(second_square);
        uint64_t attack_bitboards = player ? black_attacks : white_attacks;

        char first_piece = '-';
        char second_piece = '-';

        for (auto x: bitboard) {
            if (x.second & (1ull << initial)) first_piece = x.first;
            if (x.second & (1ull << final)) second_piece = x.first;
        }

        if (second_piece != '-' && second_piece <= 'Z') second_piece += 32;
        if (first_piece <= 'Z') first_piece += 32;

        if (first_piece == 'k') {
            score_moves.push_back({move, 1000});
        } else if (first_piece == 'p' && second_piece == 'q') {
            score_moves.push_back({move, 990});
        } else if (first_piece == 'n' && second_piece == 'q') {
            score_moves.push_back({move, 985});
        } else if (first_piece == 'b' && second_piece == 'q') {
            score_moves.push_back({move, 980});
        } else if (first_piece == 'r' && second_piece == 'q') {
            score_moves.push_back({move, 880});
        } else if (first_piece == 'q' && second_piece == 'q') {
            score_moves.push_back({move, 200});
        } else if (first_piece == 'p' && second_piece == 'r') {
            score_moves.push_back({move, 950});
        } else if (first_piece == 'n' && second_piece == 'r') {
            score_moves.push_back({move, 600});
        } else if (first_piece == 'b' && second_piece == 'r') {
            score_moves.push_back({move, 550});
        } else if (first_piece == 'r' && second_piece == 'r') {
            score_moves.push_back({move, 150});
        } else if (first_piece == 'q' && second_piece == 'r') {
            score_moves.push_back({move, 140});
        } else if (first_piece == 'p' && second_piece == 'b') {
            score_moves.push_back({move, 900});
        } else if (first_piece == 'n' && second_piece == 'b') {
            score_moves.push_back({move, 160});
        } else if (first_piece == 'b' && second_piece == 'b') {
            score_moves.push_back({move, 150});
        } else if (first_piece == 'r' && second_piece == 'b') {
            score_moves.push_back({move, 140});
        } else if (first_piece == 'q' && second_piece == 'b') {
            score_moves.push_back({move, 120});
        } else if (first_piece == 'p' && second_piece == 'n') {
            score_moves.push_back({move, 850});
        } else if (first_piece == 'n' && second_piece == 'n') {
            score_moves.push_back({move, 160});
        } else if (first_piece == 'b' && second_piece == 'n') {
            score_moves.push_back({move, 145});
        } else if (first_piece == 'r' && second_piece == 'n') {
            score_moves.push_back({move, 135});
        } else if (first_piece == 'q' && second_piece == 'n') {
            score_moves.push_back({move, 120});
        } else if (first_piece == 'p' && second_piece == 'p') {
            score_moves.push_back({move, 800});
        } else if (first_piece == 'n' && second_piece == 'p') {
            score_moves.push_back({move, 110});
        } else if (first_piece == 'b' && second_piece == 'p') {
            score_moves.push_back({move, 110});
        } else if (first_piece == 'r' && second_piece == 'p') {
            score_moves.push_back({move, 105});
        } else if (first_piece == 'q' && second_piece == 'p') {
            score_moves.push_back({move, 100});
        } else score_moves.push_back({move, 99});
    }

    std::sort(score_moves.begin(), score_moves.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    for (auto x: score_moves) ordered_moves.push_back(x.first);
    return ordered_moves;
}


void Engine::push(std::string move) {
    if (std::find(legal_moves.begin(), legal_moves.end(), move) != legal_moves.end()) {
        uint64_t attack_bitboards = player ? black_bitboards : white_bitboards;
        std::string start = move.substr(0, 2);
        std::string end = move.substr(2, 2);
        int initial = convert_to_number(start);
        int final = convert_to_number(end);
        char piece, attack_piece = '0';
        for (auto [pc, board]: bitboard) {
            if (board & (1ull << initial)) {
                piece = pc;
            }
            if (board & (1ull << final)) {
                attack_piece = pc;
            }
        }


        if (piece == 'p' || piece == 'P') {
            // removing the pawn if enPassant played
            if (final == en_passant) {
                if (piece == 'p') bitboard['P'] &= ~(1ull << (final + 8));
                else if (piece == 'P') bitboard['p'] &= ~(1ull << (final - 8));
            }
            halfmove_clock = -1;
            if (piece == 'P' && initial >= 8 && initial < 16 && final >= 24 && final < 32) {
                if ((final < 31 && (bitboard['p'] & (1ull << (final + 1)))) || (final > 24 && (bitboard['p'] & (1ull << (final - 1))))) en_passant = final - 8;
                else en_passant = 111;
            } else if (piece == 'p' && initial >= 48 && initial < 56 && final >= 32 && final < 40) {
                if ((final < 39 && (bitboard['P'] & (1ull << (final + 1)))) || (final > 32 && (bitboard['P'] & (1ull << (final - 1))))) en_passant = final + 8;
                else en_passant = 111;
            } else en_passant = 111;
        } else en_passant = 111;

        if (piece == 'K') {
            castling_rights.reset(3);
            castling_rights.reset(2);
        } else if (piece == 'k') {
            castling_rights.reset(1);
            castling_rights.reset(0);
        }

        if (move.size() == 5) {
            if ((bitboard['R'] & (1ull << final)) || (bitboard['r'] & (1ull << final))) {
                if (final == 0) castling_rights.reset(2);
                else if (final == 7) castling_rights.reset(3);
                else if(final == 56) castling_rights.reset(0);
                else if (final == 63) castling_rights.reset(1);
            }
            bitboard[piece] &= ~(1ull << initial);
            bitboard[player ? (move[4] - 32) : move[4]] |= (1ull << final);
            if (attack_bitboards & (1ull << final)) {
                bitboard[attack_piece] &= ~(1ull << final);
            }
        } else {
            if (attack_bitboards & (1ull << final)) {
                halfmove_clock = -1;
                if ((bitboard['R'] & (1ull << final)) || (bitboard['r'] & (1ull << final))) {
                    if (final == 0) castling_rights.reset(2);
                    else if (final == 7) castling_rights.reset(3);
                    else if(final == 56) castling_rights.reset(0);
                    else if (final == 63) castling_rights.reset(1);
                }
                if (piece == 'R' || piece == 'r') {
                    if (initial == 0) castling_rights.reset(2);
                    else if (initial == 7) castling_rights.reset(3);
                    else if(initial == 56) castling_rights.reset(0);
                    else if (initial == 63) castling_rights.reset(1);
                }
                bitboard[piece] &= ~(1ull << initial);
                bitboard[piece] |= (1ull << final);
                bitboard[attack_piece] &= ~(1ull << final);
            } else {
                std::pair<int, int> _castling = player ? std::make_pair(2, 6) : std::make_pair(58, 62);
                // moving the piece to its currect position
                bitboard[piece] &= ~(1ull << initial);
                bitboard[piece] |= (1ull << final);
                if ((piece == 'K' || piece == 'k') && (initial == (player ? 4 : 60)) && (final == _castling.first || final == _castling.second)) {
                    // castling the rook
                    if (final == 2) {
                        bitboard['R'] &= ~(1ull << 0);
                        bitboard['R'] |= (1ull << 3);
                    } else if (final == 6) {
                        bitboard['R'] &= ~(1ull << 7);
                        bitboard['R'] |= (1ull << 5);
                    } else if (final == 58) {
                        bitboard['r'] &= ~(1ull << 56);
                        bitboard['r'] |= (1ull << 59);
                    } else if (final == 62) {
                        bitboard['r'] &= ~(1ull << 63);
                        bitboard['r'] |= (1ull << 61);
                    }
                } else if (piece == 'R' || piece == 'r') {
                    if (initial == 0) castling_rights.reset(2);
                    else if (initial == 7) castling_rights.reset(3);
                    else if(initial == 56) castling_rights.reset(0);
                    else if (initial == 63) castling_rights.reset(1);
                }
            }
        }
        merged_bitboards = 0ull; white_bitboards = 0ull; black_bitboards = 0ull; 
        for (auto x: bitboard) {
            merged_bitboards |= x.second;
            if (x.first >= 'A' && x.first <= 'Z') white_bitboards |= x.second;
            else if (x.first >= 'a' && x.first <= 'z') black_bitboards |= x.second;
        }
        if (!player) fullmove_number += 1;
        halfmove_clock += 1;
        played_moves.push_back(move);
        fen_moves.push_back(fen);
        if (halfmove_clock >= 100) {
            is_fifty_moves = true;
            is_threefold_repetition = false;
        } else {
            is_fifty_moves = false;
            is_threefold_repetition = false;
        }
        generate_attack_bitboards();
        this->player = !player;
        this->fen = get_fen();

        // add a threefold repetition rule
        std::vector<std::string> parts;
        std::stringstream ss(fen);
        std::string token;
        while (std::getline(ss, token, ' ')) {
            parts.push_back(token);
        }
        parts.pop_back(); parts.pop_back();
        std::ostringstream oss;
        for (const std::string& word : parts) {
            if (&word != &parts[0]) {
                oss << " "; // Add a space before each word except the first one
            }
            oss << word;
        }
        std::string fen_hash = oss.str();
        positions[fen_hash]++;
        // std::cout << fen_hash << std::endl;
        if (positions[fen_hash] >= 3) {
            is_threefold_repetition = true;
        } else is_threefold_repetition = false;
    } else {
        std::cout << "<----------------ERROR----------------->\n";
        std::cout << fen << std::endl;
        for (auto x: played_moves) std::cout << x << " ";
        std::cout << std::endl;
        std::cout << std::endl;
        for (auto x: legal_moves) std::cout << x << " ";
        std::cout << std::endl;
        print_board();
        printNumberAsBoard(black_bitboards);
        std::cout << std::endl;
        std::cout << move << std::endl;
        throw std::runtime_error("An illegal move");
    }
}

void Engine::pop() {
    played_moves.pop_back();
    std::string prev_fen = fen_moves[fen_moves.size() - 1];
    fen_moves.pop_back();

    std::vector<std::string> parts;
    std::stringstream ss(fen);
    std::string token;
    while (std::getline(ss, token, ' ')) {
        parts.push_back(token);
    }
    parts.pop_back(); parts.pop_back();
    std::ostringstream oss;
    for (const std::string& word : parts) {
        if (&word != &parts[0]) {
            oss << " "; // Add a space before each word except the first one
        }
        oss << word;
    }
    std::string fen_hash = oss.str();
    positions[fen_hash]--;
    if (positions[fen_hash] == 0) {
        positions.erase(fen_hash);
    }
    
    this->fen = prev_fen;
    get_info();

    this->player = !this->player;
    generate_attack_bitboards();
    this->player = !this->player;
    generate_legal_moves();
}


#endif