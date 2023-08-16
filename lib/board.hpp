#ifndef BOARD_H
#define BOARD_H
#include "../helpers/help.hpp"
#include "../src/chess.hpp"
#include <unordered_map>
#include <bitset>
#include <string>

class Board:virtual public Chess {
private:
    std::unordered_map<char, uint64_t> generate_bitboard(std::string pos);
    std::bitset<4> generate_castling_rights(std::string rights);
    void generate_merged_bitboards();
    void generate_white_bitboards();
    void generate_black_bitboards();

public:    
    void get_info();
    void print_board();
};

void Board::print_board() {
    print_chessboard(bitboard);
}

void Board::get_info() {
    std::vector<std::string> parts;
    std::stringstream ss(fen);
    std::string token;
    while (std::getline(ss, token, ' ')) {
        parts.push_back(token);
    }

    bitboard = generate_bitboard(parts[0]);
    player = parts[1] == "w" ? true : false;
    castling_rights = generate_castling_rights(parts[2]);
    en_passant = parts[3] == "-" ? 111 : convert_to_number(parts[3]);
    halfmove_clock = std::stoi(parts[4]);
    fullmove_number = std::stoi(parts[5]);
    white_bitboards = 0ull, black_bitboards = 0ull, merged_bitboards = 0ull;
    generate_merged_bitboards();
    generate_black_bitboards();
    generate_white_bitboards();
}

std::unordered_map<char, uint64_t> Board::generate_bitboard(std::string pos) {
    int rank = 7, file = 0;
    std::unordered_map<char, uint64_t> bitboards;
    for (char c : pos) {
        if (c > '0' && c <= '8') {
            file += (c - '0');
        } else if (c == '/') {
            rank--;
            file = 0;
        } else {
            uint64_t bitboard = 1ULL << (rank * 8 + file);
            char piece = c;
            bitboards[piece] |= bitboard;
            file++;
        }
    }
    return bitboards;
}

std::bitset<4> Board::generate_castling_rights(std::string rights) {
    std::bitset<4> num(0);
    for (auto cc: rights) {
        if (cc == 'K') num.set(3);
        else if (cc == 'k') num.set(1);
        else if (cc == 'Q') num.set(2);
        else if (cc == 'q') num.set(0);
    }
    return num;
}

void Board::generate_merged_bitboards() {
    for (auto x: bitboard) {
        merged_bitboards |= x.second;
    }
}

void Board::generate_white_bitboards() {
    for (auto x: bitboard) {
        if (x.first >= 'A' && x.first <= 'Z') {
            white_bitboards |= x.second;
        }
    }
}

void Board::generate_black_bitboards() {
    for (auto x: bitboard) {
        if (x.first >= 'a' && x.first <= 'z') {
            black_bitboards |= x.second;
        }
    }
}

#endif