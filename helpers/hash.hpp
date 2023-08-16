#ifndef HASH_H
#define HASH_H
#include <unordered_map>
#include <bitset>
#include <cstdint>
#include "../constants/zobrist_keys.hpp"

uint64_t get_zobrist_hash(std::unordered_map<char, uint64_t> bitboards, bool player, std::bitset<4> castling_rights, uint64_t en_passant) {
    // 0 1 2 3 4 5 6 7 8 9 10 11
    // K Q R N B P k q r n  b  p
    uint64_t hash = 0;
    uint64_t number = bitboards['K'];
    while (number) {
        int square = __builtin_ctzll(number);
        number &= ~(1ull << square);
        hash = hash ^ zobrist_keys[square*12];
    }

    number = bitboards['Q'];
    while (number) {
        int square = __builtin_ctzll(number);
        number &= ~(1ull << square);
        hash = hash ^ zobrist_keys[square*12 + 1];
    }

    number = bitboards['R'];
    while (number) {
        int square = __builtin_ctzll(number);
        number &= ~(1ull << square);
        hash = hash ^ zobrist_keys[square*12 + 2];
    }

    number = bitboards['N'];
    while (number) {
        int square = __builtin_ctzll(number);
        number &= ~(1ull << square);
        hash = hash ^ zobrist_keys[square*12 + 3];
    }

    number = bitboards['B'];
    while (number) {
        int square = __builtin_ctzll(number);
        number &= ~(1ull << square);
        hash = hash ^ zobrist_keys[square*12 + 4];
    }

    number = bitboards['P'];
    while (number) {
        int square = __builtin_ctzll(number);
        number &= ~(1ull << square);
        hash = hash ^ zobrist_keys[square*12 + 5];
    }

    number = bitboards['k'];
    while (number) {
        int square = __builtin_ctzll(number);
        number &= ~(1ull << square);
        hash = hash ^ zobrist_keys[square*12 + 6];
    }

    number = bitboards['q'];
    while (number) {
        int square = __builtin_ctzll(number);
        number &= ~(1ull << square);
        hash = hash ^ zobrist_keys[square*12 + 7];
    }

    number = bitboards['r'];
    while (number) {
        int square = __builtin_ctzll(number);
        number &= ~(1ull << square);
        hash = hash ^ zobrist_keys[square*12 + 8];
    }

    number = bitboards['n'];
    while (number) {
        int square = __builtin_ctzll(number);
        number &= ~(1ull << square);
        hash = hash ^ zobrist_keys[square*12 + 9];
    }

    number = bitboards['b'];
    while (number) {
        int square = __builtin_ctzll(number);
        number &= ~(1ull << square);
        hash = hash ^ zobrist_keys[square*12 + 10];
    }

    number = bitboards['p'];
    while (number) {
        int square = __builtin_ctzll(number);
        number &= ~(1ull << square);
        hash = hash ^ zobrist_keys[square*12 + 11];
    }

    int shift = 768;
    if (player) hash = hash ^ zobrist_keys[shift];
    shift++;

    if (castling_rights.test(3))
        hash = hash ^ zobrist_keys[shift];
    shift++;
    if (castling_rights.test(2))
        hash = hash ^ zobrist_keys[shift];
    shift++;
    if (castling_rights.test(1))
        hash = hash ^ zobrist_keys[shift];
    shift++;
    if (castling_rights.test(0))
        hash = hash ^ zobrist_keys[shift];
    shift++;

    if (en_passant == 0) return hash;
    int square = __builtin_ctzll(en_passant);
    square = square % 8;
    hash = hash ^ zobrist_keys[shift + square];

    return hash;
}

#endif