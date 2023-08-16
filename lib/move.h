#ifndef MOVE_H
#define MOVE_H
#include <bits/stdc++.h>
#include "../helpers/help.h"
#include "../constants/constants.h"
#include "../helpers/attacks.h"

class Move : virtual public Chess {
private:
    void generate_pseudo_pawn_moves();
    void generate_pseudo_knight_moves();
    void generate_pseudo_king_moves();
    void generate_pseudo_bishop_moves();
    void generate_pseudo_rook_moves();
    void generate_pseudo_queen_moves();
    
    void generate_king_attacks();
    void generate_pawn_attacks();
    void generate_knight_attacks();
    void generate_bishop_attacks();
    void generate_rook_attacks();
    void generate_queen_attacks();
    void generate_check_squares();
protected:
    void generate_attack_bitboards();
public:
    void generate_pseudo_legal_moves();
    bool is_check();
};

void Move::generate_attack_bitboards() {
    player ? white_attacks = 0 : black_attacks = 0;
    is_black_king_check = false; is_white_king_check = false; is_double_check = false;
    checked_by = 111; second_checked_by = 111;
    pinned_piece_map.clear();
    generate_king_attacks();
    generate_pawn_attacks();
    generate_knight_attacks();
    generate_bishop_attacks();
    generate_rook_attacks();
    generate_queen_attacks();
    generate_check_squares();
}

void Move::generate_check_squares() {
    uint64_t king_bitboard = bitboard[player ? 'K' : 'k'];
    uint64_t player_bitboard = player ? white_bitboards : black_bitboards;
    uint64_t opponent_bitboard = player ? black_bitboards : white_bitboards;
    uint64_t opponent_king_bitboard = bitboard[player ? 'k' : 'K'];

    while (king_bitboard) {
        int initial_square = __builtin_ctzll(king_bitboard);
        king_bitboard &= ~(1ull << initial_square);

        uint64_t rook_masked = rook_mask_edges[initial_square];
        uint64_t rook_blockers = rook_masked & merged_bitboards;

        for (int direction = 0; direction < 4; direction++) {
            int search_index = initial_square * 4 + direction;
            uint64_t tmp_rook_blockers = rook_blockers & rook_mask_edges_divider[search_index];
            if (tmp_rook_blockers) {
                int attacked_square = direction < 2 ? __builtin_ctzll(tmp_rook_blockers) : 63 - __builtin_clzll(tmp_rook_blockers);
                if (player_bitboard & (1ull << attacked_square)) {
                    uint64_t tmp_points = two_points[initial_square][attacked_square];
                    rook_check_squares |= tmp_points;
                } else {
                    tmp_rook_blockers &= ~(1ull << attacked_square);
                    if (opponent_king_bitboard & (1ull << attacked_square)) {
                        if (tmp_rook_blockers) {
                            int next_attacked_square = direction < 2 ? __builtin_ctzll(tmp_rook_blockers) : 63 - __builtin_clzll(tmp_rook_blockers);
                            uint64_t another_mask = rook_mask_edges_divider[search_index] ^ rook_mask_edges_divider[next_attacked_square*4 + direction];
                            rook_check_squares |= another_mask;
                        } else {
                            rook_check_squares |= rook_mask_edges_divider[search_index];
                        }
                    } else {
                        rook_check_squares |= rook_mask_edges_divider[search_index] ^ rook_mask_edges_divider[attacked_square * 4 + direction];
                    }
                }
            } else {
                rook_check_squares |= rook_mask_edges_divider[search_index];
            }

        }
    }

    king_bitboard = bitboard[player ? 'K' : 'k'];
    while (king_bitboard) {
        int initial_square = __builtin_ctzll(king_bitboard);
        king_bitboard &= ~(1ull << initial_square);

        uint64_t bishop_masked = bishop_mask_edges[initial_square];
        uint64_t bishop_blockers = bishop_masked & merged_bitboards;

        // Finding attacking piece
        for (int direction = 0; direction < 4; direction++) {
            int search_index = initial_square * 4 + direction;
            uint64_t tmp_bishop_blockers = bishop_blockers & bishop_mask_edges_divider[search_index];
            if (tmp_bishop_blockers) {
                int attacked_square = direction < 2 ? __builtin_ctzll(tmp_bishop_blockers) : 63 - __builtin_clzll(tmp_bishop_blockers);
                if (player_bitboard & (1ull << attacked_square)) {
                    uint64_t tmp_points = two_points[initial_square][attacked_square];
                    bishop_check_squares |= tmp_points;
                } else {
                    tmp_bishop_blockers &= ~(1ull << attacked_square);
                    if (opponent_king_bitboard & (1ull << attacked_square)) {
                        if (tmp_bishop_blockers) {
                            int next_attacked_square = direction < 2 ? __builtin_ctzll(tmp_bishop_blockers) : 63 -__builtin_clzll(tmp_bishop_blockers);
                            uint64_t another_mask = bishop_mask_edges_divider[search_index] ^ bishop_mask_edges_divider[next_attacked_square*4 + direction];
                            bishop_check_squares |= another_mask;
                        } else {
                            bishop_check_squares |= bishop_mask_edges_divider[search_index];
                        }
                    } else {
                        bishop_check_squares |= bishop_mask_edges_divider[search_index] ^ bishop_mask_edges_divider[attacked_square * 4 + direction];
                    }
                }
            } else {
                bishop_check_squares |= bishop_mask_edges_divider[search_index];
            }
        }
    }

    king_bitboard = bitboard[player ? 'K' : 'k'];
    while (king_bitboard) {
        int initial_square = __builtin_ctzll(king_bitboard);
        king_bitboard &= ~(1ull << initial_square);
        if (initial_square >= 16 && initial_square % 8 != 0) {
            knight_check_squares |= (1ull << (initial_square - 17));
        }
        if (initial_square >= 16 && initial_square % 8 != 7) {
            knight_check_squares |= (1ull << (initial_square - 15));
        }

        if (initial_square < 48 && initial_square % 8 != 0) {
            knight_check_squares |= (1ull << (initial_square + 15));
        }
        if (initial_square < 48 && initial_square % 8 != 7) {
            knight_check_squares |= (1ull << (initial_square + 17));
        }

        if (initial_square >= 8 && initial_square % 8 > 1) {
            knight_check_squares |= (1ull << (initial_square - 10));
        }
        if (initial_square < 56 && initial_square % 8 > 1) {
            knight_check_squares |= (1ull << (initial_square + 6));
        }

        if (initial_square >= 8 && initial_square % 8 < 6) {
            knight_check_squares |= (1ull << (initial_square - 6));
        }
        if (initial_square < 56 && initial_square % 8 < 6) {
            knight_check_squares |= (1ull << (initial_square + 10));
        }
    }   

}

void Move::generate_queen_attacks() {
    uint64_t current_queen_bitboard = bitboard[player ? 'Q' : 'q'];
    uint64_t player_bitboard = player ? white_bitboards : black_bitboards;
    uint64_t opponent_bitboard = player ? black_bitboards : white_bitboards;
    uint64_t opponent_king_bitboard = bitboard[player ? 'k' : 'K'];

    while (current_queen_bitboard) {
        int initial_square = __builtin_ctzll(current_queen_bitboard);
        current_queen_bitboard &= ~(1ull << initial_square);

        uint64_t queen_masked = rook_mask_edges[initial_square] | bishop_mask_edges[initial_square];
        uint64_t queen_blockers = queen_masked & merged_bitboards;
        
        // bishop
        for (int direction = 0; direction < 4; direction++) {
            int search_index = initial_square * 4 + direction;
            uint64_t tmp_bishop_blockers = queen_blockers & bishop_mask_edges_divider[search_index];
            if (tmp_bishop_blockers) {
                int attacked_square = direction < 2 ? __builtin_ctzll(tmp_bishop_blockers) : 63 - __builtin_clzll(tmp_bishop_blockers);
                if (player_bitboard & (1ull << attacked_square)) {
                    uint64_t tmp_points = two_points[initial_square][attacked_square];
                    player ? white_attacks |= tmp_points : black_attacks |= tmp_points;
                } else {
                    tmp_bishop_blockers &= ~(1ull << attacked_square);
                    if (opponent_king_bitboard & (1ull << attacked_square)) {
                        if (player) {
                            if (is_black_king_check) {
                                is_double_check = true;
                                second_checked_by = initial_square;
                            } else {
                                is_black_king_check = true;
                                checked_by = initial_square;
                            }
                        } else {
                            if (is_white_king_check) {
                                is_double_check = true;
                                second_checked_by = initial_square;
                            } else {
                                is_white_king_check = true;
                                checked_by = initial_square;
                            }
                        }
                        if (tmp_bishop_blockers) {
                            int next_attacked_square = direction < 2 ? __builtin_ctzll(tmp_bishop_blockers) : 63 - __builtin_clzll(tmp_bishop_blockers);
                            uint64_t another_mask = bishop_mask_edges_divider[search_index] ^ bishop_mask_edges_divider[next_attacked_square*4 + direction];
                            player ? white_attacks |= another_mask : black_attacks |= another_mask;
                        } else {
                            player ? white_attacks |= bishop_mask_edges_divider[search_index] : black_attacks |= bishop_mask_edges_divider[search_index];
                        }
                    } else {
                        player ? white_attacks |= bishop_mask_edges_divider[search_index] ^ bishop_mask_edges_divider[attacked_square*4 + direction] : black_attacks |= bishop_mask_edges_divider[search_index] ^ bishop_mask_edges_divider[attacked_square *4 + direction];
                        if (tmp_bishop_blockers) {
                            int next_attacked_square = direction < 2 ? __builtin_ctzll(tmp_bishop_blockers) : 63 - __builtin_clzll(tmp_bishop_blockers);
                            if (opponent_king_bitboard & (1ull << next_attacked_square)) {
                                pinned_piece_map[attacked_square] = initial_square;
                            }
                        }
                    }
                }
            } else {
                player ? white_attacks |= bishop_mask_edges_divider[search_index] : black_attacks |= bishop_mask_edges_divider[search_index];
            }
        }

        // rook
        for (int direction = 0; direction < 4; direction++) {
            int search_index = initial_square * 4 + direction;
            uint64_t tmp_rook_blockers = queen_blockers & rook_mask_edges_divider[search_index];
            if (tmp_rook_blockers) {
                int attacked_square = direction < 2 ? __builtin_ctzll(tmp_rook_blockers) : 63 - __builtin_clzll(tmp_rook_blockers);
                if (player_bitboard & (1ull << attacked_square)) {
                    uint64_t tmp_points = two_points[initial_square][attacked_square];
                    player ? white_attacks |= tmp_points : black_attacks |= tmp_points;
                } else {
                    tmp_rook_blockers &= ~(1ull << attacked_square);
                    if (opponent_king_bitboard & (1ull << attacked_square)) {
                        if (player) {
                            if (is_black_king_check) {
                                is_double_check = true;
                                second_checked_by = initial_square;
                            } else {
                                is_black_king_check = true;
                                checked_by = initial_square;
                            }
                        } else {
                            if (is_white_king_check) {
                                is_double_check = true;
                                second_checked_by = initial_square;
                            } else {
                                is_white_king_check = true;
                                checked_by = initial_square;
                            }
                        }
                        if (tmp_rook_blockers) {
                            int next_attacked_square = direction < 2 ? __builtin_ctzll(tmp_rook_blockers) : 63 - __builtin_clzll(tmp_rook_blockers);
                            uint64_t another_mask = rook_mask_edges_divider[search_index] ^ rook_mask_edges_divider[next_attacked_square*4 + direction];
                            player ? white_attacks |= another_mask : black_attacks |= another_mask;
                        } else {
                            player ? white_attacks |= rook_mask_edges_divider[search_index] : black_attacks |= rook_mask_edges_divider[search_index];
                        }
                    } else {
                        player ? white_attacks |= rook_mask_edges_divider[search_index] ^ rook_mask_edges_divider[attacked_square*4 + direction] : black_attacks |= rook_mask_edges_divider[search_index] ^ rook_mask_edges_divider[attacked_square*4 + direction];
                        if (tmp_rook_blockers) {
                            int next_attacked_square = direction < 2 ? __builtin_ctzll(tmp_rook_blockers) : 63 - __builtin_clzll(tmp_rook_blockers);
                            if (opponent_king_bitboard & (1ull << next_attacked_square)) {
                                pinned_piece_map[attacked_square] = initial_square;
                            }
                        }
                    }
                }
            } else {
                player ? white_attacks |= rook_mask_edges_divider[search_index] : black_attacks |= rook_mask_edges_divider[search_index];
            }
        }
    }
    
}

void Move::generate_rook_attacks() {
    uint64_t current_rook_bitboard = bitboard[player ? 'R' : 'r'];
    uint64_t player_bitboard = player ? white_bitboards : black_bitboards;
    uint64_t opponent_bitboard = player ? black_bitboards : white_bitboards;
    uint64_t opponent_king_bitboard = bitboard[player ? 'k' : 'K'];
    while (current_rook_bitboard) {
        int initial_square = __builtin_ctzll(current_rook_bitboard);
        current_rook_bitboard &= ~(1ull << initial_square);

        uint64_t rook_masked = rook_mask_edges[initial_square];
        uint64_t rook_blockers = rook_masked & merged_bitboards;

        for (int direction = 0; direction < 4; direction++) {
            int search_index = initial_square * 4 + direction;
            uint64_t tmp_rook_blockers = rook_blockers & rook_mask_edges_divider[search_index];
            if (tmp_rook_blockers) {
                int attacked_square = direction < 2 ? __builtin_ctzll(tmp_rook_blockers) : 63 - __builtin_clzll(tmp_rook_blockers);
                if (player_bitboard & (1ull << attacked_square)) {
                    uint64_t tmp_points = two_points[initial_square][attacked_square];
                    player ? white_attacks |= tmp_points : black_attacks |= tmp_points;
                } else {
                    tmp_rook_blockers &= ~(1ull << attacked_square);
                    if (opponent_king_bitboard & (1ull << attacked_square)) {
                        if (player) {
                            if (is_black_king_check) {
                                is_double_check = true;
                                second_checked_by = initial_square;
                            } else {
                                is_black_king_check = true;
                                checked_by = initial_square;
                            }
                        } else {
                            if (is_white_king_check) {
                                is_double_check = true;
                                second_checked_by = initial_square;
                            } else {
                                is_white_king_check = true;
                                checked_by = initial_square;
                            }
                        }
                        if (tmp_rook_blockers) {
                            int next_attacked_square = direction < 2 ? __builtin_ctzll(tmp_rook_blockers) : 63 - __builtin_clzll(tmp_rook_blockers);
                            uint64_t another_mask = rook_mask_edges_divider[search_index] ^ rook_mask_edges_divider[next_attacked_square*4 + direction];
                            player ? white_attacks |= another_mask : black_attacks |= another_mask;
                        } else {
                            player ? white_attacks |= rook_mask_edges_divider[search_index] : black_attacks |= rook_mask_edges_divider[search_index];
                        }
                    } else {
                        player ? white_attacks |= rook_mask_edges_divider[search_index] ^ rook_mask_edges_divider[attacked_square * 4 + direction] : black_attacks |= rook_mask_edges_divider[search_index] ^ rook_mask_edges_divider[attacked_square * 4 + direction];
                        if (tmp_rook_blockers) {
                            int next_attacked_square = direction < 2 ? __builtin_ctzll(tmp_rook_blockers) : 63 - __builtin_clzll(tmp_rook_blockers);
                            if (opponent_king_bitboard & (1ull << next_attacked_square)) {
                                pinned_piece_map[attacked_square] = initial_square;
                            }
                        }
                    }
                }
            } else {
                player ? white_attacks |= rook_mask_edges_divider[search_index] : black_attacks |= rook_mask_edges_divider[search_index];
            }

        }
    }

}

void Move::generate_bishop_attacks() {
    uint64_t current_bishop_bitboard = bitboard[player ? 'B' : 'b'];
    uint64_t player_bitboard = player ? white_bitboards : black_bitboards;
    uint64_t opponent_bitboard = player ? black_bitboards : white_bitboards;
    uint64_t opponent_king_bitboard = bitboard[player ? 'k' : 'K'];
    while (current_bishop_bitboard) {
        int initial_square = __builtin_ctzll(current_bishop_bitboard);
        current_bishop_bitboard &= ~(1ull << initial_square);

        uint64_t bishop_masked = bishop_mask_edges[initial_square];
        uint64_t bishop_blockers = bishop_masked & merged_bitboards;

        // Finding attacking piece
        for (int direction = 0; direction < 4; direction++) {
            int search_index = initial_square * 4 + direction;
            uint64_t tmp_bishop_blockers = bishop_blockers & bishop_mask_edges_divider[search_index];
            if (tmp_bishop_blockers) {
                int attacked_square = direction < 2 ? __builtin_ctzll(tmp_bishop_blockers) : 63 - __builtin_clzll(tmp_bishop_blockers);
                if (player_bitboard & (1ull << attacked_square)) {
                    uint64_t tmp_points = two_points[initial_square][attacked_square];
                    player ? white_attacks |= tmp_points : black_attacks |= tmp_points;
                } else {
                    tmp_bishop_blockers &= ~(1ull << attacked_square);
                    if (opponent_king_bitboard & (1ull << attacked_square)) {
                        if (player) {
                            if (is_black_king_check) {
                                is_double_check = true;
                                second_checked_by = initial_square;
                            } else {
                                is_black_king_check = true;
                                checked_by = initial_square;
                            }
                        } else {
                            if (is_white_king_check) {
                                is_double_check = true;
                                second_checked_by = initial_square;
                            } else {
                                is_white_king_check = true;
                                checked_by = initial_square;
                            }
                        }
                        if (tmp_bishop_blockers) {
                            int next_attacked_square = direction < 2 ? __builtin_ctzll(tmp_bishop_blockers) : 63 -__builtin_clzll(tmp_bishop_blockers);
                            uint64_t another_mask = bishop_mask_edges_divider[search_index] ^ bishop_mask_edges_divider[next_attacked_square*4 + direction];
                            player ? white_attacks |= another_mask : black_attacks |= another_mask;
                        } else {
                            player ? white_attacks |= bishop_mask_edges_divider[search_index] : black_attacks |= bishop_mask_edges_divider[search_index];
                        }
                    } else {
                        player ? white_attacks |= bishop_mask_edges_divider[search_index] ^ bishop_mask_edges_divider[attacked_square * 4 + direction] : black_attacks |= bishop_mask_edges_divider[search_index] ^ bishop_mask_edges_divider[attacked_square * 4 + direction];
                        if (tmp_bishop_blockers) {
                            int next_attacked_square = direction < 2 ? __builtin_ctzll(tmp_bishop_blockers) : 63 - __builtin_clzll(tmp_bishop_blockers);
                            if (opponent_king_bitboard & (1ull << next_attacked_square)) {
                                pinned_piece_map[attacked_square] = initial_square;
                            }
                        }
                    }
                }
            } else {
                player ? white_attacks |= bishop_mask_edges_divider[search_index] : black_attacks |= bishop_mask_edges_divider[search_index];
            }
        }
    }
}

void Move::generate_knight_attacks() {
    uint64_t current_knight_bitboard = bitboard[player ? 'N' : 'n'];
    uint64_t opponent_king_bitboard = bitboard[player ? 'k' : 'K'];
    while (current_knight_bitboard) {
        int initial_square = __builtin_ctzll(current_knight_bitboard);
        current_knight_bitboard &= ~(1ull << initial_square);
        if (initial_square >= 16 && initial_square % 8 != 0) {
            player ? white_attacks |= (1ull << (initial_square - 17)) : black_attacks |= (1ull << (initial_square - 17));
            if (opponent_king_bitboard & (1ull << (initial_square - 17))) {
                if (player) {
                    if (is_black_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_black_king_check = true;
                        checked_by = initial_square;
                    }
                } else {
                    if (is_white_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_white_king_check = true;
                        checked_by = initial_square;
                    }
                }
            }
        }
        if (initial_square >= 16 && initial_square % 8 != 7) {
            player ? white_attacks |= (1ull << (initial_square - 15)) : black_attacks |= (1ull << (initial_square - 15));
            if (opponent_king_bitboard & (1ull << (initial_square - 15))) {
                if (player) {
                    if (is_black_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_black_king_check = true;
                        checked_by = initial_square;
                    }
                } else {
                    if (is_white_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_white_king_check = true;
                        checked_by = initial_square;
                    }
                }
            }
        }

        if (initial_square < 48 && initial_square % 8 != 0) {
            player ? white_attacks |= (1ull << (initial_square + 15)) : black_attacks |= (1ull << (initial_square + 15));
            if (opponent_king_bitboard & (1ull << (initial_square + 15))) {
                if (player) {
                    if (is_black_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_black_king_check = true;
                        checked_by = initial_square;
                    }
                } else {
                    if (is_white_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_white_king_check = true;
                        checked_by = initial_square;
                    }
                }
            }
        }
        if (initial_square < 48 && initial_square % 8 != 7) {
            player ? white_attacks |= (1ull << (initial_square + 17)) : black_attacks |= (1ull << (initial_square + 17));
            if (opponent_king_bitboard & (1ull << (initial_square + 17))) {
                if (player) {
                    if (is_black_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_black_king_check = true;
                        checked_by = initial_square;
                    }
                } else {
                    if (is_white_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_white_king_check = true;
                        checked_by = initial_square;
                    }
                }
            }
        }

        if (initial_square >= 8 && initial_square % 8 > 1) {
            player ? white_attacks |= (1ull << (initial_square - 10)) : black_attacks |= (1ull << (initial_square - 10));
            if (opponent_king_bitboard & (1ull << (initial_square - 10))) {
                if (player) {
                    if (is_black_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_black_king_check = true;
                        checked_by = initial_square;
                    }
                } else {
                    if (is_white_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_white_king_check = true;
                        checked_by = initial_square;
                    }
                }
            }
        }
        if (initial_square < 56 && initial_square % 8 > 1) {
            player ? white_attacks |= (1ull << (initial_square + 6)) : black_attacks |= (1ull << (initial_square + 6));
            if (opponent_king_bitboard & (1ull << (initial_square + 6))) {
                if (player) {
                    if (is_black_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_black_king_check = true;
                        checked_by = initial_square;
                    }
                } else {
                    if (is_white_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_white_king_check = true;
                        checked_by = initial_square;
                    }
                }
            }

        }

        if (initial_square >= 8 && initial_square % 8 < 6) {
            player ? white_attacks |= (1ull << (initial_square - 6)) : black_attacks |= (1ull << (initial_square - 6));
            if (opponent_king_bitboard & (1ull << (initial_square - 6))) {
                if (player) {
                    if (is_black_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_black_king_check = true;
                        checked_by = initial_square;
                    }
                } else {
                    if (is_white_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_white_king_check = true;
                        checked_by = initial_square;
                    }
                }
            }
        }
        if (initial_square < 56 && initial_square % 8 < 6) {
            player ? white_attacks |= (1ull << (initial_square + 10)) : black_attacks |= (1ull << (initial_square + 10));
            if (opponent_king_bitboard & (1ull << (initial_square + 10))) {
                if (player) {
                    if (is_black_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_black_king_check = true;
                        checked_by = initial_square;
                    }
                } else {
                    if (is_white_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_white_king_check = true;
                        checked_by = initial_square;
                    }
                }
            }
        }
    }   
}

void Move::generate_pawn_attacks() {
    uint64_t current_pawn_bitboard = bitboard[player ? 'P' : 'p'];
    uint64_t opponent_king_bitboard = bitboard[player ? 'k' : 'K'];
    int direction = player ? 8 : -8;
    while (current_pawn_bitboard) {
        int initial_square = __builtin_ctzll(current_pawn_bitboard);
        current_pawn_bitboard &= ~(1ull << initial_square);
        if (initial_square % 8 != 7) {
            player ? white_attacks |= (1ull << (initial_square + direction + 1)) : black_attacks |= (1ull << (initial_square + direction + 1));
            if (opponent_king_bitboard & (1ull << (initial_square + direction + 1))) {
                if (player) {
                    if (is_black_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_black_king_check = true;
                        checked_by = initial_square;
                    }
                } else {
                    if (is_white_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_white_king_check = true;
                        checked_by = initial_square;
                    }
                }
            }
        }
        if (initial_square % 8 != 0) {
            player ? white_attacks |= (1ull << (initial_square + direction - 1)) : black_attacks |= (1ull << (initial_square + direction - 1));
            if (opponent_king_bitboard & (1ull << (initial_square + direction - 1))) {
                if (player) {
                    if (is_black_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_black_king_check = true;
                        checked_by = initial_square;
                    }
                } else {
                    if (is_white_king_check) {
                        is_double_check = true;
                        second_checked_by = initial_square;
                    } else {
                        is_white_king_check = true;
                        checked_by = initial_square;
                    }
                }
            }
        }
    }
}

void Move::generate_king_attacks() {
    uint64_t current_king_bitboard = bitboard[player ? 'K' : 'k'];
    uint64_t opponent_king_bitboard = bitboard[player ? 'k' : 'K'];
    while (current_king_bitboard) {
        int initial_square = __builtin_ctzll(current_king_bitboard);
        current_king_bitboard &= ~(1ull << initial_square);
        std::vector<std::pair<int, int>> directions{std::make_pair(1, 1), std::make_pair(1, -1), std::make_pair(-1, 1), std::make_pair(-1, -1), std::make_pair(1, 0), std::make_pair(-1, 0), std::make_pair(0, 1), std::make_pair(0, -1)};
        for (auto direction: directions) {
            if ((initial_square + direction.first*8 + direction.second) >= 0 && (initial_square + direction.first*8 + direction.second) < 64) {
                if ((initial_square % 8 == 0 && direction.second != -1) || (initial_square % 8 == 7 && direction.second != 1)) {
                    player ? (white_attacks |= (1ull << (initial_square + direction.first*8 + direction.second))) : (black_attacks |= (1ull << (initial_square + direction.first*8 + direction.second)));
                    if (opponent_king_bitboard & (1ull << (initial_square + direction.first*8 + direction.second))) is_invalid = true;
                } else {
                    player ? (white_attacks |= (1ull << (initial_square + direction.first*8 + direction.second))) : (black_attacks |= (1ull << (initial_square + direction.first*8 + direction.second)));
                    if (opponent_king_bitboard & (1ull << (initial_square + direction.first*8 + direction.second))) is_invalid = true;
                }
            }
        }
    }
}

bool Move::is_check() {
    return player ? is_white_king_check : is_black_king_check;
}

void Move::generate_pseudo_legal_moves() {
    pseudo_legal_moves.clear();
    generate_pseudo_pawn_moves();
    generate_pseudo_knight_moves();
    generate_pseudo_king_moves();
    generate_pseudo_bishop_moves();
    generate_pseudo_rook_moves();
    generate_pseudo_queen_moves();
}

void Move::generate_pseudo_pawn_moves() {
    uint64_t current_pawn_bitboard = bitboard[player ? 'P' : 'p'];
    uint64_t opposition_bitboards = !player ? white_bitboards : black_bitboards;
    int direction = player ? +1 : -1;
    std::pair<int, int> promotion_range = player ? std::make_pair(48, 56) : std::make_pair(8, 16);
    std::pair<int, int> start_range = player ? std::make_pair(8, 16) : std::make_pair(48, 56);
    while (current_pawn_bitboard) {
        int initial_square = __builtin_ctzll(current_pawn_bitboard);
        current_pawn_bitboard &= ~(1ull << initial_square);
        // capture moves and en_passant
        if (initial_square % 8 != 0 && ((opposition_bitboards & (1ull << (initial_square + direction*8 - 1))) || ((initial_square + direction*8 - 1) == en_passant))) {
            if (initial_square >= promotion_range.first && initial_square < promotion_range.second) { // promotion moves
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction*8 - 1, 'q'));
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction*8 - 1, 'n'));
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction*8 - 1, 'b'));
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction*8 - 1, 'r'));
            } else {
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction*8 - 1));
            }
        }
        if (initial_square % 8 != 7 && ((opposition_bitboards & (1ull << (initial_square + direction*8 + 1))) || ((initial_square + direction*8 + 1) == en_passant))) {
            if (initial_square >= promotion_range.first && initial_square < promotion_range.second) { // promotion moves
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction*8 + 1, 'q'));
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction*8 + 1, 'n'));
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction*8 + 1, 'b'));
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction*8 + 1, 'r'));
            } else {
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction*8 + 1));
            }
        }

        if (!(merged_bitboards & (1ull << (initial_square + direction*8)))) {
            // one step move
            if (initial_square >= promotion_range.first && initial_square < promotion_range.second) { // promotion moves
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction*8, 'q'));
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction*8, 'n'));
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction*8, 'b'));
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction*8, 'r'));
            } else {
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction*8));
            }
            // two step moves
            if (initial_square >= start_range.first && initial_square < start_range.second && !(merged_bitboards & (1ull << (initial_square + direction*16)))) {
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction*16));
            }
        }
    }
}

void Move::generate_pseudo_knight_moves() {
    uint64_t current_knight_bitboard = bitboard[player ? 'N' : 'n'];
    uint64_t player_bitboards = player ? white_bitboards : black_bitboards;
    while (current_knight_bitboard) {
        int initial_square = __builtin_ctzll(current_knight_bitboard);
        current_knight_bitboard &= ~(1ull << initial_square);
        // Top Left Moves
        if (initial_square >= 16 && initial_square % 8 != 0 && !(player_bitboards & (1ull << (initial_square - 17)))) {
            pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square - 17));
        }
        // Top Right Moves
        if (initial_square >= 16 && initial_square % 8 != 7 && !(player_bitboards & (1ull << (initial_square - 15)))) {
            pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square - 15));
        }

        // Bottom Left Moves
        if (initial_square < 48 && initial_square % 8 != 0 && !(player_bitboards & (1ull << (initial_square + 15)))) {
            pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + 15));
        }
        // To Right Moves
        if (initial_square < 48 && initial_square % 8 != 7 && !(player_bitboards & (1ull << (initial_square + 17)))) {
            pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + 17));
        }

        // Left Top Moves
        if (initial_square >= 8 && initial_square % 8 > 1 && !(player_bitboards & (1ull << (initial_square - 10)))) {
            pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square - 10));
        }
        // Left Bottom Moves
        if (initial_square < 56 && initial_square % 8 > 1 && !(player_bitboards & (1ull << (initial_square + 6)))) {
            pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + 6));
        }

        // Right Top Moves
        if (initial_square >= 8 && initial_square % 8 < 6 && !(player_bitboards & (1ull << (initial_square - 6)))) {
            pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square - 6));
        }
        // Left Bottom Moves
        if (initial_square < 56 && initial_square % 8 < 6 && !(player_bitboards & (1ull << (initial_square + 10)))) {
            pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + 10));
        }
    }
}

void Move::generate_pseudo_king_moves() {
    uint64_t current_king_bitboard = bitboard[player ? 'K' : 'k'];
    uint64_t player_bitboards = player ? white_bitboards : black_bitboards;
    std::pair<int, int> castling = player ? std::make_pair(3, 2) : std::make_pair(1, 0); 
    while (current_king_bitboard) {
        int initial_square = __builtin_ctzll(current_king_bitboard);
        current_king_bitboard &= ~(1ull << initial_square);
        std::vector<std::pair<int, int>> directions{std::make_pair(1, 1), std::make_pair(1, -1), std::make_pair(-1, 1), std::make_pair(-1, -1), std::make_pair(1, 0), std::make_pair(-1, 0), std::make_pair(0, 1), std::make_pair(0, -1)};
        for (auto direction: directions) {
            if ((initial_square + direction.first*8 + direction.second) >= 0 && (initial_square + direction.first*8 + direction.second) < 64 && !(player_bitboards & (1ull << (initial_square + direction.first*8 + direction.second)))) {
                if (direction.second == -1 && initial_square % 8 != 0) {
                    pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction.first*8 + direction.second));
                }
                if (direction.second == 1 && initial_square % 8 != 7) {
                    pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction.first*8 + direction.second));
                }
                if (direction.second == 0) {
                    pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + direction.first*8 + direction.second));
                }
            }
        }
        if (castling_rights.test(castling.first)) {
            if (!(merged_bitboards & (1ull << (player ? 5 : 61))) && !(merged_bitboards & (1ull << (player ? 6 : 62)))) {
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square + 2));
            } 
        }
        if (castling_rights.test(castling.second)) {
            if (!(merged_bitboards & (1ull << (player ? 1 : 57))) && !(merged_bitboards & (1ull << (player ? 2 : 58))) && !(merged_bitboards & (1ull << (player ? 3 : 59)))) {
                pseudo_legal_moves.push_back(convert_to_uci(initial_square, initial_square - 2));
            }
        }

    }
}

void Move::generate_pseudo_bishop_moves() {
    uint64_t current_bishop_bitboard = bitboard[player ? 'B' : 'b'];
    uint64_t player_bitboards = player ? white_bitboards : black_bitboards;
    while (current_bishop_bitboard) {
        int initial_square = __builtin_ctzll(current_bishop_bitboard);
        current_bishop_bitboard &= ~(1ull << initial_square);

        uint64_t magic_board = (merged_bitboards & BMask[initial_square]) * BMagic[initial_square];
        uint64_t key = magic_board >> (64 - BBits[initial_square]);
        uint64_t final = BTable[initial_square*512 + key];

        final = final & ~(player_bitboards);

        while (final) {
            int square = __builtin_ctzll(final);
            final &= ~(1ull << square);
            pseudo_legal_moves.push_back(convert_to_uci(initial_square, square));
        }
    }
}

void Move::generate_pseudo_rook_moves() {
    uint64_t current_rook_bitboard = bitboard[player ? 'R' : 'r'];
    uint64_t player_bitboards = player ? white_bitboards : black_bitboards;
    while (current_rook_bitboard) {
        int initial_square = __builtin_ctzll(current_rook_bitboard);
        current_rook_bitboard &= ~(1ull << initial_square);

        uint64_t magic_board = (merged_bitboards & RMask[initial_square]) * RMagic[initial_square];
        uint64_t key = magic_board >> (64 - RBits[initial_square]);
        uint64_t final = RTable[initial_square][key];

        final = final & ~(player_bitboards);

        while (final) {
            int square = __builtin_ctzll(final);
            final &= ~(1ull << square);
            pseudo_legal_moves.push_back(convert_to_uci(initial_square, square));
        }
    }
}

void Move::generate_pseudo_queen_moves() {
    uint64_t current_queen_bitboard = bitboard[player ? 'Q' : 'q'];
    uint64_t player_bitboards = player ? white_bitboards : black_bitboards;
    while (current_queen_bitboard) {
        int initial_square = __builtin_ctzll(current_queen_bitboard);
        current_queen_bitboard &= ~(1ull << initial_square);

        uint64_t magic_boardR = (merged_bitboards & RMask[initial_square]) * RMagic[initial_square];
        uint64_t magic_boardB = (merged_bitboards & BMask[initial_square]) * BMagic[initial_square];
        uint64_t keyR = magic_boardR >> (64 - RBits[initial_square]);
        uint64_t keyB = magic_boardB >> (64 - BBits[initial_square]);
        uint64_t finalR = RTable[initial_square][keyR];
        uint64_t finalB = BTable[initial_square*512 + keyB];
        uint64_t final = (finalR | finalB);

        final &= ~(player_bitboards);

        while (final) {
            int square = __builtin_ctzll(final);
            final &= ~(1ull << square);
            pseudo_legal_moves.push_back(convert_to_uci(initial_square, square));
        }
    }
}

#endif