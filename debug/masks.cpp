#include <bits/stdc++.h>
#include "../constants/constants.h"

void printNumberAsBoard(uint64_t number) {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            uint64_t bit = (number >> ((7 - row) * 8 + col)) & 1;
            std::cout << bit;
        }
        std::cout << std::endl;
    }
}

uint64_t generateBishopMask(int square) {
    std::vector<std::pair<int, int>> directions{
        {1, 0}, {0, -1}, {-1, 0}, {0, 1}
    };
    uint64_t attacks = 0ull;
    for (auto direction: directions) {
        int current_square = square;
        while (((current_square + 8*direction.first + direction.second) % 8 != 0 || current_square % 8 == 0) && ((current_square + 8*direction.first + direction.second) % 8 != 7 || current_square % 8 == 7) && ((current_square + 8*direction.first + direction.second) >= 8 || current_square < 8) && ((current_square + 8*direction.first + direction.second) < 56 || current_square >= 56)) {
            attacks |= (1ull << (current_square + 8*direction.first + direction.second));
            current_square += 8*direction.first + direction.second;
        }
    }
    return attacks;
}

std::vector<uint64_t> generate_possible_bitboards(int square, uint64_t board, int direction) {
    if (direction != 0 && (square % 8 == 0 || square % 8 == 7 || square < 8 || square >= 56)) return {board};
    std::vector<uint64_t> bitboards;
    if (direction == 0) {
        std::vector<uint64_t> bitboard1 = generate_possible_bitboards(square + 7, board, 1);
        std::vector<uint64_t> bitboard2 = generate_possible_bitboards(square + 9, board, 2);
        std::vector<uint64_t> bitboard3 = generate_possible_bitboards(square - 7, board, 3);
        std::vector<uint64_t> bitboard4 = generate_possible_bitboards(square - 9, board, 4);
        for (const auto& elem1 : bitboard1) {
            for (const auto& elem2 : bitboard2) {
                for (const auto& elem3 : bitboard3) {
                    for (const auto& elem4 : bitboard4) {
                        uint64_t combined = elem1 | elem2 | elem3 | elem4;
                        bitboards.push_back(combined);
                    }
                }
            }
        }
    } else {
        if (direction == 1) {
            std::vector<uint64_t> take = generate_possible_bitboards(square + 7, (board | (1ull << (square))), 1);
            std::vector<uint64_t> notTake = generate_possible_bitboards(square + 7, board, 1);
            bitboards.insert(bitboards.end(), take.begin(), take.end());
            bitboards.insert(bitboards.end(), notTake.begin(), notTake.end());
        } else if (direction == 2) {
            std::vector<uint64_t> take = generate_possible_bitboards(square + 9, (board | (1ull << (square))), 2);
            std::vector<uint64_t> notTake = generate_possible_bitboards(square + 9, board, 2);
            bitboards.insert(bitboards.end(), take.begin(), take.end());
            bitboards.insert(bitboards.end(), notTake.begin(), notTake.end());
        } else if (direction == 3) {
            std::vector<uint64_t> take = generate_possible_bitboards(square - 7, (board | (1ull << (square))), 3);
            std::vector<uint64_t> notTake = generate_possible_bitboards(square - 7, board, 3);
            bitboards.insert(bitboards.end(), take.begin(), take.end());
            bitboards.insert(bitboards.end(), notTake.begin(), notTake.end());
        } else if (direction == 4) {
            std::vector<uint64_t> take = generate_possible_bitboards(square - 9, (board | (1ull << (square))), 4);
            std::vector<uint64_t> notTake = generate_possible_bitboards(square - 9, board, 4);
            bitboards.insert(bitboards.end(), take.begin(), take.end());
            bitboards.insert(bitboards.end(), notTake.begin(), notTake.end());
        }
    }

    return bitboards;
}

uint64_t generate_possible_moves(int square, uint64_t boards) {
    uint64_t ans = 0ull;

    if (square % 8 == 0 || square % 8 == 7 || square < 8 || square >= 56) {
        int current_square = square + 7;
        if (current_square % 8 == 0 && current_square < 64) ans |= (1ull << current_square);
        while(current_square < 64 && square % 8 != 0 && current_square % 8 != 0) {
            ans |= (1ull << current_square);
            if (boards & (1ull << current_square)) break;
            current_square += 7;
            if (current_square % 8 == 0 && current_square < 64) ans |= (1ull << current_square);
        }

        current_square = square + 9;
        if (current_square % 8 == 7 && current_square < 64) ans |= (1ull << current_square);
        while(current_square < 64 && square % 8 != 7 && current_square % 8 != 7) {
            ans |= (1ull << current_square);
            if (boards & (1ull << current_square)) break;
            current_square += 9;
            if (current_square % 8 == 7 && current_square < 64) ans |= (1ull << current_square);
        }

        current_square = square - 7;
        if (current_square % 8 == 7 && current_square >= 0) ans |= (1ull << current_square);
        while(current_square >= 0 && square % 8 != 7 && current_square % 8 != 7) {
            ans |= (1ull << current_square);
            if (boards & (1ull << current_square)) break;
            current_square -= 7;
            if (current_square % 8 == 7 && current_square >= 0) ans |= (1ull << current_square);
        }

        current_square = square - 9;
        if (current_square % 8 == 0 && current_square >= 0) ans |= (1ull << current_square);
        while(current_square >= 0 && square % 8 != 0 && current_square % 8 != 0) {
            ans |= (1ull << current_square);
            if (boards & (1ull << current_square)) break;
            current_square -= 9;
            if (current_square % 8 == 0 && current_square >= 0) ans |= (1ull << current_square);
        }
    }

    int current_square = square;
    // if (current_square < 8 || current_square % 8 == 0) current_square += 7;
    while (current_square % 8 != 0 && current_square % 8 != 7 && current_square >= 8 && current_square < 56) {
        if (current_square % 8 == 0 || current_square >= 56) ans |= (1ull << (current_square + 7));
        current_square += 7;
        ans |= (1ull << current_square);
        if (boards & (1ull << current_square)) break;
    }

    current_square = square;
    // if (current_square < 8 || current_square % 8 == 7) current_square += 9;
    while (current_square % 8 != 0 && current_square % 8 != 7 && current_square >= 8 && current_square < 56) {
        if (current_square % 8 == 7 || current_square >= 56) ans |= (1ull << (current_square + 9));
        current_square += 9;
        ans |= (1ull << current_square);
        if (boards & (1ull << current_square)) break;
    }

    current_square = square;
    // if (current_square >= 56 || current_square % 8 == 7) current_square -= 7;
    while (current_square % 8 != 0 && current_square % 8 != 7 && current_square >= 8 && current_square < 56) {
        if (current_square % 8 == 7 || current_square  < 8) ans |= (1ull << (current_square - 7));
        current_square -= 7;
        ans |= (1ull << current_square);
        if (boards & (1ull << current_square)) break;
    }

    current_square = square;
    // if (current_square >= 56 || current_square % 8 == 0) current_square -= 9;
    while (current_square % 8 != 0 && current_square % 8 != 7 && current_square >= 8 && current_square < 56) {
        if (current_square % 8 == 0 || current_square - 9 < 8) ans |= (1ull << (current_square - 9));
        current_square -= 9;
        ans |= (1ull << current_square);
        if (boards & (1ull << current_square)) break;
    }

    return ans;
}

void printBishopTableToFile(const uint64_t bishop_table[], const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cout << "Failed to open file: " << filename << std::endl;
        return;
    }

    file << "const uint64_t BTable[32768] = {\n";
    for (int i = 0; i < 32768; i++) {
        file << "  0x" << std::hex << bishop_table[i] << "ULL,\n";
    }
    file << "};\n";

    file.close();
    std::cout << "Data written to file: " << filename << std::endl;
}

void printRookTableToFile(const uint64_t rook_table[], const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cout << "Failed to open file: " << filename << std::endl;
        return;
    }

    file << "const uint64_t BTable[64][4096] = {\n";
    for (int i = 0; i < 64; i++) {
        file << "{";
        for (int j=0; j<4096; j++) {
            file << "  0x" << std::hex << rook_table[i*4096 + j] << "ULL,\n";
        }
        file << "},";
    }
    file << "};\n";

    file.close();
    std::cout << "Data written to file: " << filename << std::endl;
}

std::vector<uint64_t> generate_possible_rook_bitboards(int square, uint64_t board, int direction) {
    if (direction == 1 && square >= 56) return {board}; 
    if (direction == 2 && square % 8 == 7) return {board}; 
    if (direction == 3 && square < 8) return {board}; 
    if (direction == 4 && square % 8 == 0) return {board}; 
    std::vector<uint64_t> bitboards;
    
    if (direction == 0) {
        std::vector<uint64_t> bitboards1{0};
        std::vector<uint64_t> bitboards2{0};
        std::vector<uint64_t> bitboards3{0};
        std::vector<uint64_t> bitboards4{0};
        if (square < 56) {
            bitboards1 = generate_possible_rook_bitboards(square + 8, board, 1);
        }
        if (square % 8 != 7) {
            bitboards2 = generate_possible_rook_bitboards(square + 1, board, 2);
        }
        if (square >= 8) {
            bitboards3 = generate_possible_rook_bitboards(square - 8, board, 3);
        }
        if (square % 8 != 0) {
            bitboards4 = generate_possible_rook_bitboards(square - 1, board, 4);
        }
        for (const auto& elem1 : bitboards1) {
            for (const auto& elem2 : bitboards2) {
                for (const auto& elem3 : bitboards3) {
                    for (const auto& elem4 : bitboards4) {
                        uint64_t combined = elem1 | elem2 | elem3 | elem4;
                        bitboards.push_back(combined);
                    }
                }
            }
        }
    } else {
        if (direction == 1) {
            std::vector<uint64_t> take = generate_possible_rook_bitboards(square + 8, (board | (1ull << square)), 1);
            std::vector<uint64_t> notTake = generate_possible_rook_bitboards(square + 8 , board, 1);
            bitboards.insert(bitboards.end(), take.begin(), take.end());
            bitboards.insert(bitboards.end(), notTake.begin(), notTake.end());
        } else if (direction == 2) {
            std::vector<uint64_t> take = generate_possible_rook_bitboards(square + 1, (board | (1ull << (square))), 2);
            std::vector<uint64_t> notTake = generate_possible_rook_bitboards(square + 1 , board, 2);
            bitboards.insert(bitboards.end(), take.begin(), take.end());
            bitboards.insert(bitboards.end(), notTake.begin(), notTake.end());
        } else if (direction == 3) {
            std::vector<uint64_t> take = generate_possible_rook_bitboards(square - 8, (board | (1ull << (square))), 3);
            std::vector<uint64_t> notTake = generate_possible_rook_bitboards(square - 8 , board, 3);
            bitboards.insert(bitboards.end(), take.begin(), take.end());
            bitboards.insert(bitboards.end(), notTake.begin(), notTake.end());
        } else if (direction == 4) {
            std::vector<uint64_t> take = generate_possible_rook_bitboards(square - 1, (board | (1ull << (square))), 4);
            std::vector<uint64_t> notTake = generate_possible_rook_bitboards(square - 1 , board, 4);
            bitboards.insert(bitboards.end(), take.begin(), take.end());
            bitboards.insert(bitboards.end(), notTake.begin(), notTake.end());
        }
    }
    return bitboards;
}

uint64_t generate_possible_rook_moves(int square, uint64_t boards) {
    uint64_t ans = 0;
    int current_square = square + 8;
    while (square < 56 && current_square < 64) {
        ans |= (1ull << current_square);
        if (boards & (1ull << current_square)) break;
        current_square += 8;
    }

    current_square = square + 1;
    if (current_square % 8 == 7) ans |= (1ull << current_square);
    while (square % 8 != 7 && current_square % 8 != 7) {
        ans |= (1ull << current_square);
        if (boards & (1ull << current_square)) break;
        current_square += 1;
        if (current_square % 8 == 7) ans |= (1ull << current_square);
    }

    current_square = square - 8;
    while (square >= 8 && current_square >= 0) {
        ans |= (1ull << current_square);
        if (boards & (1ull << current_square)) break;
        current_square -= 8;
    }

    current_square = square - 1;
    if (current_square % 8 == 0) ans |= (1ull << current_square);
    while (square % 8 != 0 && current_square % 8 != 0) {
        ans |= (1ull << current_square);
        if (boards & (1ull << current_square)) break;
        current_square -= 1;
        if (current_square % 8 == 0) ans |= (1ull << current_square);
    }

    return ans;
}

uint64_t get_bishop_mask(int position) {
    uint64_t mask = 0;
    int row = position / 8;
    int col = position % 8;

    int r = row, c = col;
    while (r > 0 && c > 0) {
        r--; c--;
        mask |= 1ULL << (r * 8 + c);
    }

    r = row; c = col;
    while (r > 0 && c < 7) {
        r--; c++;
        mask |= 1ULL << (r * 8 + c);
    }

    r = row; c = col;
    while (r < 7 && c > 0) {
        r++; c--;
        mask |= 1ULL << (r * 8 + c);
    }

    r = row; c = col;
    while (r < 7 && c < 7) {
        r++; c++;
        mask |= 1ULL << (r * 8 + c);
    }

    return mask;
}

uint64_t get_rook_mask(int position) {
    uint64_t mask = 0;
    int row = position / 8;
    int col = position % 8;

    // Top
    for (int r = row - 1; r >= 0; r--) {
        mask |= 1ULL << (r * 8 + col);
    }

    // Bottom
    for (int r = row + 1; r < 8; r++) {
        mask |= 1ULL << (r * 8 + col);
    }

    // Left
    for (int c = col - 1; c >= 0; c--) {
        mask |= 1ULL << (row * 8 + c);
    }

    // Right
    for (int c = col + 1; c < 8; c++) {
        mask |= 1ULL << (row * 8 + c);
    }

    return mask;
}

std::vector<uint64_t> get_bishop_mask_divider(int position) {
    std::vector<uint64_t> masks;
    int row = position / 8;
    int col = position % 8;
    uint64_t mask = 0;
    
    int r = row, c = col;
    while (r < 7 && c > 0) {
        r++; c--;
        mask |= 1ULL << (r * 8 + c);
    }
    masks.push_back(mask);
    mask = 0;

    r = row; c = col;
    while (r < 7 && c < 7) {
        r++; c++;
        mask |= 1ULL << (r * 8 + c);
    }
    masks.push_back(mask);
    mask = 0;

    r = row; c = col;
    while (r > 0 && c < 7) {
        r--; c++;
        mask |= 1ULL << (r * 8 + c);
    }
    masks.push_back(mask);
    mask = 0;

    r = row; c = col;
    while (r > 0 && c > 0) {
        r--; c--;
        mask |= 1ULL << (r * 8 + c);
    }
    masks.push_back(mask);


    return masks;
}

std::vector<uint64_t> get_rook_mask_divider(int position) {
    std::vector<uint64_t> masks;
    uint64_t mask = 0;
    int row = position / 8;
    int col = position % 8;

    // Top
    for (int r = row + 1; r < 8; r++) {
        mask |= 1ULL << (r * 8 + col);
    }
    masks.push_back(mask);
    mask = 0;

    // Right
    for (int c = col + 1; c < 8; c++) {
        mask |= 1ULL << (row * 8 + c);
    }
    masks.push_back(mask);
    mask = 0;

    // Bottom
    for (int r = row - 1; r >= 0; r--) {
        mask |= 1ULL << (r * 8 + col);
    }
    masks.push_back(mask);
    mask = 0;

    // Left
    for (int c = col - 1; c >= 0; c--) {
        mask |= 1ULL << (row * 8 + c);
    }
    masks.push_back(mask);
    mask = 0;

    return masks;
}

uint64_t create_bishop_points(int start, int end) {
    uint64_t bitboard = 0;
    int startRow = start / 8;
    int startCol = start % 8;
    int endRow = end / 8;
    int endCol = end % 8;

    // Check if start and end positions are on the same diagonal
    if (abs(startRow - endRow) == abs(startCol - endCol)) {
        int rowDelta = (startRow < endRow) ? 1 : -1;
        int colDelta = (startCol < endCol) ? 1 : -1;

        int row = startRow + rowDelta;
        int col = startCol + colDelta;

        while (row != endRow && col != endCol) {
            bitboard |= 1ULL << (row * 8 + col);
            row += rowDelta;
            col += colDelta;
        }

        bitboard |= 1ull << (row * 8 + col);
    } else if (startRow == endRow || startCol == endCol) {
        int rowDelta = (startRow == endRow) ? 0 : ((startRow < endRow) ? 1 : -1);
        int colDelta = (startCol == endCol) ? 0 : ((startCol < endCol) ? 1 : -1);

        int row = startRow + rowDelta;
        int col = startCol + colDelta;

        while (row != endRow || col != endCol) {
            bitboard |= 1ULL << (row * 8 + col);
            row += rowDelta;
            col += colDelta;
        }

        bitboard |= 1ull << (row * 8 + col);
    }

    return bitboard;
}

int main() {
    std::cout << "uint64_t two_points[64][64] = {";
    for (int i=0; i<64; i++) {
        std::cout << "{";
        for (int j=0; j<64; j++) {
            if (i != j) {
                std::cout << "0x" << std::hex << create_bishop_points(i, j) << "ull,";
            } else {
                std::cout << "0x0ull,";
            }
        }
        std::cout << "},";
    }
    std::cout << "};";

    // uint64_t* rook_table = new uint64_t[262144];
    // // uint64_t bishop_table[32768];
    // for (int i=0; i<64; i++) {
    //     std::vector<uint64_t> possible_bitboards =  generate_possible_rook_bitboards(i, 0ull, 0);
    //     sort(possible_bitboards.begin(), possible_bitboards.end());
    //     possible_bitboards.erase(unique(possible_bitboards.begin(), possible_bitboards.end()), possible_bitboards.end());
    //     // std::cout << possible_bitboards.size() << std::endl;
    //     for (auto bitboards: possible_bitboards) {
    //         // uint64_t bitboards = 0x0000000011000100;
    //         uint64_t magic_board = bitboards * RMagic[i];
    //         uint64_t key = magic_board >> (64 - RBits[i]);
    //         uint64_t moves_board = generate_possible_rook_moves(i, bitboards);
    //         rook_table[i*4096 + key] = moves_board;
    //     }
    // }

    // std::vector<uint64_t> rook_mask_edges_divider;
    // for (int i=0; i<64; i++) {
    //     std::vector<uint64_t> tmp = get_rook_mask_divider(i);
    //     rook_mask_edges_divider.insert(rook_mask_edges_divider.end(), tmp.begin(), tmp.end()); 
    // }

    // std::cout << "std::vector<uint64_t> rook_mask_edges_divider = {\n";
    // for (int i=0; i<256; i++) {
        // std::cout << "0x" << std::hex << rook_mask_edges_divider[i] << "ull,";
    // }
    // std::cout << "};" << std::endl;

    // std::string filename = "rook_table.txt";
    // printRookTableToFile(rook_table, filename);


}