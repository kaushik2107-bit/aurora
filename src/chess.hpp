#ifndef CHESS_H
#define CHESS_H
#include "../helpers/help.hpp"

class Chess {
protected:
    std::string fen; // fen position
    std::unordered_map<char, uint64_t> bitboard; // stores all 12 bitboards for each piece
    std::bitset<4> castling_rights; // KQkq -> 1101 means no black king side castling
    std::uint64_t en_passant; // the number has one set bit which represent the en_passant square
    bool player; // true for white and false for black
    uint64_t merged_bitboards = 0;
    uint64_t white_bitboards = 0;
    uint64_t black_bitboards = 0;
    std::vector<std::string> fen_moves;
    std::unordered_map<std::string, int> positions;
    uint64_t knight_check_squares = 0;
    uint64_t bishop_check_squares = 0;
    uint64_t rook_check_squares = 0;
private:
public:
    unsigned int fullmove_number; // full moves played
    unsigned int halfmove_clock; // moves played after capture or pawn moves
    std::vector<std::string> pseudo_legal_moves;
    std::vector<std::string> legal_moves;
    std::vector<std::string> played_moves;
    uint64_t black_attacks = 0;
    uint64_t white_attacks = 0;
    bool is_white_king_check = false;
    bool is_black_king_check = false;
    bool is_double_check = false;
    int checked_by = 111;
    int second_checked_by = 111;
    bool is_invalid = false;
    std::unordered_map<int, int> pinned_piece_map;
    bool is_checkmate = false;
    bool is_stalemate = false;
    bool is_fifty_moves = false;
    bool is_threefold_repetition = false;

    std::string get_fen() {
        std::string current_fen = "";

        for (int rank = 7; rank >= 0; rank--) {
            int emptySquares = 0;
            for (int file = 0; file < 8; file++) {
                char piece = '0';
                int squareIndex = rank * 8 + file;
                for (auto x: bitboard) {
                    char pc = x.first;
                    uint64_t brd = x.second;
                    if (brd & (1ull << squareIndex)) {
                        piece = pc;
                        break;
                    }
                }

                if (piece != '0') {
                    if (emptySquares > 0) current_fen += std::to_string(emptySquares);
                    emptySquares = 0;
                    current_fen += piece;
                } else emptySquares++;
            }
            if (emptySquares > 0) current_fen += std::to_string(emptySquares);
            if (rank != 0 ) current_fen += '/';
        }

        current_fen += player ? " w " : " b ";
        current_fen += get_castling_rights();
        current_fen += " ";
        if (en_passant == 111) current_fen += "-";
        else current_fen += convert_to_notation(en_passant);
        current_fen += " ";
        current_fen += std::to_string(halfmove_clock);
        current_fen += " ";
        current_fen += std::to_string(fullmove_number);

        return current_fen;
    }

    std::string get_castling_rights() {
        return generate_castling_rights(castling_rights);
    }

    std::string get_en_passant_square() {
        return convert_to_notation(en_passant);
    }
    
    std::string get_player() {
        return player ? "White" : "Black";
    }
};

#endif