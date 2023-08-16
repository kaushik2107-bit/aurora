#ifndef HELP_H
#define HELP_H
#include <bits/stdc++.h>

std::string generate_castling_rights(std::bitset<4> castling_rights) {
    std::string castlingRights;
    if (castling_rights.test(3))
        castlingRights += 'K';
    if (castling_rights.test(2))
        castlingRights += 'Q';
    if (castling_rights.test(1))
        castlingRights += 'k';
    if (castling_rights.test(0))
        castlingRights += 'q';
    if (castlingRights.size() == 0) return "-";
    return castlingRights;
}

std::uint64_t convert_to_number(std::string square) {
    int col = square[0] - 'a';
    int row = square[1] - '1';

    return row*8 + col;
}

std::string convert_to_notation(std::uint64_t number) {
    int row = number / 8;
    int col = number % 8;

    std::string square;
    square += ('a' + col);
    square += ('1' + row);

    return square;
}

std::string convert_to_uci(uint64_t initial, uint64_t final, char promotion = '-') {
    std::string uci_notation = "";
    uci_notation += convert_to_notation(initial);
    uci_notation += convert_to_notation(final);
    if (promotion != '-') uci_notation += promotion;
    return uci_notation;
}

void print_chessboard(const std::unordered_map<char, uint64_t>& bitboards) {
    std::unordered_map<char, std::string> piece_symbols = {
        {'p', "♙"}, {'n', "♘"}, {'b', "♗"}, {'r', "♖"}, {'q', "♕"}, {'k', "♔"},
        {'P', "♟"}, {'N', "♞"}, {'B', "♝"}, {'R', "♜"}, {'Q', "♛"}, {'K', "♚"}
    };

    std::vector<std::vector<std::string>> board(8, std::vector<std::string>(8, "."));

    for (const auto& pair : bitboards) {
        char piece_type = pair.first;
        unsigned long long bitboard = pair.second;

        for (int square = 0; square < 64; ++square) {
            if (bitboard & ((uint64_t)1 << square)) {
                int row = 7 - (square / 8);
                int col = square % 8;
                // board[row][col] = piece_symbols[piece_type];
                board[row][col] = piece_type;
            }
        }
    }

    for (const auto& row : board) {
        for (const std::string& square : row) {
            std::cout << square << ' ';
        }
        std::cout << std::endl;
    }
}

void printNumberAsBoard(uint64_t number) {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            uint64_t bit = (number >> ((7 - row) * 8 + col)) & 1;
            std::cout << bit;
        }
        std::cout << std::endl;
    }
}

#endif