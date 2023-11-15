#include <algorithm>
#include <random>
#include <iostream>
#include <thread>
#include <chrono>
#include "board.hpp"
#include "engine.hpp"
#include "butils.hpp"


// Define constant for positive and negative infinity
const int INF = 1e9;
const int NEG_INF = -INF;

// Define a helper function to evaluate the board

// NOTE: This is a dummy function and should be replaced with an appropriate evaluation function for your game.
// Piece values
struct Boardscores{
short PAWN_VALUE   = 70;
short ROOK_VALUE   = 200;
short KING_VALUE   = 900; 
short BISHOP_VALUE = 100;
short KNIGHT_VALUE = 80;
float PAWN_SQUARE_TABLE[64] = {
        0, 1, 2, 3, 3.05624, 0, 0, 0,
         0, 1, 1.994, 3.606, 4.8797, 1.993009, 5, 5,
          3, 5.09532, 2, 3, 3, 1.89419, 1, 1,
           1, 6.10998,1, 2, 2, 0.994165, 0, 0,
            5.20455, 3.06924, 0, 0, 0, 0, 0, 0,
             1.26494, 3, 4, 0, -2, 0.997, 0.992, 1, 
             0.999, 3.54058, 1.79473, 0, -4.032, 2, 2, 1, 
             0, 0,  0, 0, 0, 0, 0, 0 };

float ROOK_SQUARE_TABLE[64] = {
        23.6916, 16.8375, 10.541, 25.5748, 25, 30, 30, 0,
         20.4927, 4.00969, 7.02004, 6.86752, 10.22, 11, 8.03017, 5,
          18, 12.264, 10, 10, 10, 8, 7.864, 5, 
          16, 11, 10, 10, 10, 5, 4.83633, 5, 
          16, 15, 10, 10, 10, 5.98273, 5, 5, 
          20.198, 17, 10, 13, 0, 11, 13.513,5,
           17.4375, 8.112, 2, 3.08458, 0, 7.48674, 5.4217, 5,
            0, 0, 0, 5, 5, 0, 0, 0 };

float KING_SQUARE_TABLE[64] = {
        -30, -20, 0, 0, 0, -5, -30, 20, -20, 20, 15, 0, 7.89689, 0, -10, 20, -20, 15, -20, -20, -20, 8.5275, -20, -10, -30, -10, -20, -20, -20, -17.78, -30, -10, -20, 21.8958, -30, -40, 5, 7, -29.1658, -20, -18.8343, -6.46632, 31.8329, 0, -9.9, -20.34, -31.24, -30, -50, -20, -20, -29.37, -20, -31.26, -20, -30, 0, 0, 0, 0, 0, 0, 0, 0 };

float BISHOP_SQUARE_TABLE[64] = {
        7, 10, 10, 31.1065, 10, 6, 7, 0, 
        11.03, 10, 20, 10, 22.5755, 3, 6, 5,
         10, 19.7974, 10, 10, 10, 17.4259, 4, 5, 
         25.1436, 10, 10, 10, 10, 3, 19.92, 5, 
         10, 17, 10, 10, 10, 15, 4, 5,
          10, 10, 15, 6, 4, 15, 3, 4,
           7, 10, 10, 12, 10, 2.9225, 7, 5, 
           0, 0, 0, 5, 5, 0, 0, 0 };

float KNIGHT_SQUARE_TABLE[64] = {
        0, 1, 2, 3, 3.05624, 0, 0, 0, 0, 1, 1.994, 3.606, 4.8797, 1.993009, 5, 5, 0.975066, 5.09532, 2, 3, 3, 1.89419, 1, 1, 1, 6.10998, 1, 2, 2, 0.994165, 0, 0, 1.20455, 3.06924, 0, 0, 0, 0, 0, 0, 1.26494, 8.44805, 4, 0, -2, 0.997, 0.992, 1, 0.999, 3.54058, 1.79473, 0, -4.032, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0 };

};
const short PAWN_VALUE   = 15;
const short ROOK_VALUE   = 100;
const short KING_VALUE   = 900; // This might be arbitrary since the game is over if the King is captured, but for evaluation purposes, it has a high value.
const short BISHOP_VALUE = 55;

// Piece square tables
// Note: These are placeholders. For an optimal evaluation, the values inside should be tuned based on the specifics of the game.
// Placeholder values for piece square tables.

constexpr U8 cw_90[64] = {
        48, 40, 32, 24, 16, 8,  0,  7,
        49, 41, 33, 25, 17, 9,  1,  15,
        50, 42, 18, 19, 20, 10, 2,  23,
        51, 43, 26, 27, 28, 11, 3,  31,
        52, 44, 34, 35, 36, 12, 4,  39,
        53, 45, 37, 29, 21, 13, 5,  47,
        54, 46, 38, 30, 22, 14, 6,  55,
        56, 57, 58, 59, 60, 61, 62, 63
};

constexpr U8 acw_90[64] = {
        6, 14, 22, 30, 38, 46, 54, 7,
        5, 13, 21, 29, 37, 45, 53, 15,
        4, 12, 18, 19, 20, 44, 52, 23,
        3, 11, 26, 27, 28, 43, 51, 31,
        2, 10, 34, 35, 36, 42, 50, 39,
        1,  9, 17, 25, 33, 41, 49, 47,
        0,  8, 16, 24, 32, 40, 48, 55,
        56, 57, 58, 59, 60, 61, 62, 63
};

constexpr U8 cw_180[64] = {
        54, 53, 52, 51, 50, 49, 48, 7,
        46, 45, 44, 43, 42, 41, 40, 15,
        38, 37, 18, 19, 20, 33, 32, 23,
        30, 29, 26, 27, 28, 25, 24, 31,
        22, 21, 34, 35, 36, 17, 16, 39,
        14, 13, 12, 11, 10,  9,  8, 47,
        6,  5,  4,  3,  2,  1,  0, 55,
        56, 57, 58, 59, 60, 61, 62, 63
};

constexpr U8 id[64] = {
        0,  1,  2,  3,  4,  5,  6,  7,
        8,  9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 42, 43, 44, 45, 46, 47,
        48, 49, 50, 51, 52, 53, 54, 55,
        56, 57, 58, 59, 60, 61, 62, 63
};


// put these into arrays: PAWN_SQUARE_TABLE:
//0, 1, 21.88, 3, 0.0219487, 0, 0, 0, 0, 8.83, 2, 3, 3.172, 0.000280212, 5, 5, 1, 4.1105, 2, 3, 3, 2.3736, 1, 1, 1, 3.8024, 1, 2, 2, 0.82161, 0, 0, 1, 6.363, 0, 0, 0, 0, 0, 0, 0.899983, 2894.02, 11.92, 0, -2, 1.37003, 1, 1, 0.893952, 4.10546, 13.8613, 0, -4.08, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0,
//ROOK_SQUARE_TABLE:
//10.193, 22.209, 0.00122425, 23.0472, 25, 30, 30, 0, 1478.75, 0.278906, 0.0420364, 8.06672, 10, 11.1489, 8.25157, 5, 24, 12, 10, 10, 10, 8, 8, 5, 16, 11, 10, 10, 10, 5, 2.5838, 5, 16, 15, 10, 10, 10, 4.085, 13.3889, 5, 20, 18.7, 10, 13, 0, 78394.9, 18.7828, 5, 15, 6.14141, 2.16234, 0.0715176, 0, 5.06154, 5.05, 5, 0, 0, 0, 5, 5, 0, 0, 0,
//KING_SQUARE_TABLE:
//-30, -20, 0, 0, 0, -5, -30, 20, -20, 20, 15, 0, 8.68582, 0, -10, 20, -20, 15, -20, -20, -20, 0.272598, -20, -10, -30, -10, -20, -20, -20, -20, -30, -10, -20, 12.5994, -30, -40, 5, 7, -30, -20, -13.6, -5, 18.5737, 0, -12.9942, -12.1921, -40, -30, -30.885, -22.7654, -21.424, -23.2213, -20, -59.4, -20, -30, 0, 0, 0, 0, 0, 0, 0, 0,
//BISHOP_SQUARE_TABLE:
//7, 10, 10, 0.849523, 10, 6, 7, 0, 10, 10, 20, 10, 20, 3, 6, 5, 10, 8.54096, 10, 10, 10, 15, 4, 5, 23.3796, 10, 10, 10, 10, 3, 24.4876, 5, 10, 15, 10, 10, 10, 17.004, 4, 5, 10, 10, 15, 6, -174.165, 15, 3, 4, 7, 10, 10, -13105, 10, 3, 7, 5, 0, 0, 0, 5, 5, 0, 0, 0, %


Boardscores scores_7_3;
Boardscores scores_8_4;
Boardscores scores_8_2;


// float PAWN_SQUARE_TABLE_7_3[64] = {
//         0, 1, 2, 3, 3.05624, 0, 0, 0, 0, 1, 1.994, 3.606, 4.8797, 1.993009, 5, 5, 0.975066, 5.09532, 2, 3, 3, 1.89419, 1, 1, 1, 6.10998, 1, 2, 2, 0.994165, 0, 0, 1.20455, 3.06924, 0, 0, 0, 0, 0, 0, 1.26494, 8.44805, 4, 0, -2, 0.997, 0.992, 1, 0.999, 3.54058, 1.79473, 0, -4.032, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0 };

// float ROOK_SQUARE_TABLE_7_3[64] = {
//         25.6916, 16.8375, 10.541, 25.5748, 25, 30, 30, 0, 20.4927, 4.00969, 7.02004, 6.86752, 10.22, 11, 8.03017, 5, 20.4218, 12.264, 10, 10, 10, 8, 7.864, 5, 16, 11, 10, 10, 10, 5, 4.83633, 5, 16, 15, 10, 10, 10, 5.98273, 5, 5, 20.198, 17, 10, 13, 0, 20.0363, 13.513, 5, 15.4375, 8.112, 2, 3.08458, 0, 7.48674, 5.4217, 5, 0, 0, 0, 5, 5, 0, 0, 0 };

// float KING_SQUARE_TABLE_7_3[64] = {
//         -30, -20, 0, 0, 0, -5, -30, 20, -20, 20, 15, 0, 7.89689, 0, -10, 20, -20, 15, -20, -20, -20, 8.5275, -20, -10, -30, -10, -20, -20, -20, -17.78, -30, -10, -20, 21.8958, -30, -40, 5, 7, -29.1658, -20, -18.8343, -6.46632, 31.8329, 0, -9.9, -20.34, -31.24, -30, -50, -20, -20, -29.37, -20, -31.26, -20, -30, 0, 0, 0, 0, 0, 0, 0, 0 };

// float BISHOP_SQUARE_TABLE_7_3[64] = {
//         7, 10, 10, 31.1065, 10, 6, 7, 0, 11.03, 10, 20, 10, 22.5755, 3, 6, 5, 10, 19.7974, 10, 10, 10, 17.4259, 4, 5, 25.1436, 10, 10, 10, 10, 3, 19.92, 5, 10, 15, 10, 10, 10, 15, 4, 5, 10, 10, 15, 6, 16.3202, 15, 3, 4, 7, 10, 10, 31.2556, 10, 2.9225, 7, 5, 0, 0, 0, 5, 5, 0, 0, 0 };

// float PAWN_SQUARE_TABLE_8_4[64] = {
//         0, 1, 2, 3, 3.05624, 0, 0, 0, 0, 1, 1.994, 3.606, 4.8797, 1.993009, 5, 5, 0.975066, 5.09532, 2, 3, 3, 1.89419, 1, 1, 1, 6.10998, 1, 2, 2, 0.994165, 0, 0, 1.20455, 3.06924, 0, 0, 0, 0, 0, 0, 1.26494, 8.44805, 4, 0, -2, 0.997, 0.992, 1, 0.999, 3.54058, 1.79473, 0, -4.032, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0 };

// float ROOK_SQUARE_TABLE_8_4[64] = {
//         25.6916, 16.8375, 10.541, 25.5748, 25, 30, 30, 0, 20.4927, 4.00969, 7.02004, 6.86752, 10.22, 11, 8.03017, 5, 20.4218, 12.264, 10, 10, 10, 8, 7.864, 5, 16, 11, 10, 10, 10, 5, 4.83633, 5, 16, 15, 10, 10, 10, 5.98273, 5, 5, 20.198, 17, 10, 13, 0, 20.0363, 13.513, 5, 15.4375, 8.112, 2, 3.08458, 0, 7.48674, 5.4217, 5, 0, 0, 0, 5, 5, 0, 0, 0 };

// float KING_SQUARE_TABLE_8_4[64] = {
//         -30, -20, 0, 0, 0, -5, -30, 20, -20, 20, 15, 0, 7.89689, 0, -10, 20, -20, 15, -20, -20, -20, 8.5275, -20, -10, -30, -10, -20, -20, -20, -17.78, -30, -10, -20, 21.8958, -30, -40, 5, 7, -29.1658, -20, -18.8343, -6.46632, 31.8329, 0, -9.9, -20.34, -31.24, -30, -50, -20, -20, -29.37, -20, -31.26, -20, -30, 0, 0, 0, 0, 0, 0, 0, 0 };

// float BISHOP_SQUARE_TABLE_8_4[64] = {
//         7, 10, 10, 31.1065, 10, 6, 7, 0, 11.03, 10, 20, 10, 22.5755, 3, 6, 5, 10, 19.7974, 10, 10, 10, 17.4259, 4, 5, 25.1436, 10, 10, 10, 10, 3, 19.92, 5, 10, 15, 10, 10, 10, 15, 4, 5, 10, 10, 15, 6, 16.3202, 15, 3, 4, 7, 10, 10, 31.2556, 10, 2.9225, 7, 5, 0, 0, 0, 5, 5, 0, 0, 0 };

int evaluate(const Board &b) {
    int score = 0;

    // Material count
    if (b.data.board_type == 1)
 {   for (int i = 0; i < 7; i++) {
        for (int j = 0;j < 7;j++){
            if(i >=2 && i <= 4 && j >= 2 &&j<= 4){
                continue;
            }
            U8 piece = b.data.board_0[8*i + j];
            switch (piece & ~PlayerColor::WHITE & ~PlayerColor::BLACK) {
                case PieceType::PAWN:
                    score += (piece & PlayerColor::WHITE) ? scores_7_3.PAWN_VALUE + scores_7_3.PAWN_SQUARE_TABLE[8*(6-i) + j] : -scores_7_3.PAWN_VALUE - scores_7_3.PAWN_SQUARE_TABLE[8*i +6- j];
                    break;
                case PieceType::ROOK:
                    score += (piece & PlayerColor::WHITE) ? scores_7_3.ROOK_VALUE + scores_7_3.ROOK_SQUARE_TABLE[8*(6-i) + j] : -scores_7_3.ROOK_VALUE - scores_7_3.ROOK_SQUARE_TABLE[8*i +6-j];
                    break;
                case PieceType::KING:
                    score += (piece & PlayerColor::WHITE) ? scores_7_3.KING_VALUE + scores_7_3.KING_SQUARE_TABLE[8*(6-i) + j] : -scores_7_3.KING_VALUE - scores_7_3.KING_SQUARE_TABLE[8*i +6-j];
                    break;
                case PieceType::BISHOP:
                    score += (piece & PlayerColor::WHITE) ? scores_7_3.BISHOP_VALUE + scores_7_3.BISHOP_SQUARE_TABLE[8*(6-i) + j] : -scores_7_3.BISHOP_VALUE - scores_7_3.BISHOP_SQUARE_TABLE[8*i +6-j];
                    break;
                case PieceType::KNIGHT:
                    score += (piece & PlayerColor::WHITE) ? scores_7_3.KNIGHT_VALUE + scores_7_3.KNIGHT_SQUARE_TABLE[8*(6-i) + j] : -scores_7_3.KNIGHT_VALUE - scores_7_3.KNIGHT_SQUARE_TABLE[8*i +6-j];
                    break;
            }}
    }

    // Check and Checkmate
     if (b.in_check()) {
         score += (b.data.player_to_play == PlayerColor::WHITE) ? -300 : 300;  // If white is in check, subtract value. If black is in check, add value.
     }
}
else{
if(b.data.board_type == 2){
for (int i = 0; i < 8; i++) {
        for (int j = 0;j < 8;j++){
            if(i >=2 && i <= 5 && j >= 2 &&j<= 5){
                continue;
            }
            U8 piece = b.data.board_0[8*i + j];
            switch (piece & ~PlayerColor::WHITE & ~PlayerColor::BLACK) {
                case PieceType::PAWN:
                    score += (piece & PlayerColor::WHITE) ? scores_8_4.PAWN_VALUE + scores_8_4.PAWN_SQUARE_TABLE[8*(7-i) + j] : -scores_8_4.PAWN_VALUE - scores_8_4.PAWN_SQUARE_TABLE[8*i +7- j];
                    break;
                case PieceType::ROOK:
                    score += (piece & PlayerColor::WHITE) ? scores_8_4.ROOK_VALUE + scores_8_4.ROOK_SQUARE_TABLE[8*(7-i) + j] : -scores_8_4.ROOK_VALUE - scores_8_4.ROOK_SQUARE_TABLE[8*i +7-j];
                    break;
                case PieceType::KING:
                    score += (piece & PlayerColor::WHITE) ? scores_8_4.KING_VALUE + scores_8_4.KING_SQUARE_TABLE[8*(7-i) + j] : -scores_8_4.KING_VALUE - scores_8_4.KING_SQUARE_TABLE[8*i +7-j];
                    break;
                case PieceType::BISHOP:
                    score += (piece & PlayerColor::WHITE) ? scores_8_4.BISHOP_VALUE + scores_8_4.BISHOP_SQUARE_TABLE[8*(7-i) + j] : -scores_8_4.BISHOP_VALUE - scores_8_4.BISHOP_SQUARE_TABLE[8*i +7-j];
                    break;
            }}
    }

    // Check and Checkmate
     if (b.in_check()) {
         score += (b.data.player_to_play == PlayerColor::WHITE) ? -200 : 200;  // If white is in check, subtract value. If black is in check, add value.
     }
}
else{
for (int i = 0; i < 8; i++) {
        for (int j = 0;j < 8;j++){
            if(i >=3 && i <= 4 && j >= 3 &&j<= 4){
                continue;
            }
            U8 piece = b.data.board_0[8*i + j];
            switch (piece & ~PlayerColor::WHITE & ~PlayerColor::BLACK) {
                case PieceType::PAWN:
                    score += (piece & PlayerColor::WHITE) ? scores_8_2.PAWN_VALUE + scores_8_2.PAWN_SQUARE_TABLE[8*(7-i) + j] : -scores_8_2.PAWN_VALUE - scores_8_2.PAWN_SQUARE_TABLE[8*i +7- j];
                    break;
                case PieceType::ROOK:
                    score += (piece & PlayerColor::WHITE) ? scores_8_2.ROOK_VALUE + scores_8_2.ROOK_SQUARE_TABLE[8*(7-i) + j] : -scores_8_2.ROOK_VALUE - scores_8_2.ROOK_SQUARE_TABLE[8*i +7-j];
                    break;
                case PieceType::KING:
                    score += (piece & PlayerColor::WHITE) ? scores_8_2.KING_VALUE + scores_8_2.KING_SQUARE_TABLE[8*(7-i) + j] : -scores_8_2.KING_VALUE - scores_8_2.KING_SQUARE_TABLE[8*i +7-j];
                    break;
                case PieceType::BISHOP:
                    score += (piece & PlayerColor::WHITE) ? scores_8_2.BISHOP_VALUE + scores_8_2.BISHOP_SQUARE_TABLE[8*(7-i) + j] : -scores_8_2.BISHOP_VALUE - scores_8_2.BISHOP_SQUARE_TABLE[8*i +7-j];
                    break;
            }}
    }

    // Check and Checkmate
     if (b.in_check()) {
         score += (b.data.player_to_play == PlayerColor::WHITE) ? -200 : 200;  // If white is in check, subtract value. If black is in check, add value.
     }
}


}


    // Note: Check for checkmate can be added as well but you haven't provided a function to determine checkmate.
    // For instance:
    // if (b.is_checkmate()) {
    //     score += (b.data.player_to_play == PlayerColor::WHITE) ? -1000 : 1000;
    // }

    return score;
}

int ply_counter = 0;
int depth_0 = 0;
int search_false = 0;
bool is_white = false;

int Engine::minimax(const Board &b, int depth, bool isMaximizingPlayer, int alpha, int beta,std::chrono::time_point<std::chrono::high_resolution_clock> start_time) {
    Board newBoard(b);
    int check_score = 0 ;
    if (depth == 0) {

        depth_0++;
        if (newBoard.in_check()) {
            check_score = (newBoard.data.player_to_play == PlayerColor::BLACK) ? 150:-150;  // If white is in check, subtract value. If black is in check, add value.
            //      std::cout << "score from evaluater at leaf" << evaluate(b) << "\n";
            //    std::cout << "score from adder at leaf" << score << "\n";
            return (is_white)? evaluate(b) + check_score - num_moves: evaluate(b) + check_score + num_moves;
        }
//        std::cout << ply_counter << " at depth 0\n"<< std::endl;
        //    std::cout << "score from evaluater at leaf" << evaluate(b) << "\n";
        //    std::cout << "score from adder at leaf" << score << "\n";
        return (is_white)? evaluate(b) - num_moves: evaluate(b) + num_moves;
    }
     auto end = std::chrono::high_resolution_clock::now();
        std::chrono::milliseconds timeTemp = std::chrono::duration_cast< std::chrono::milliseconds >( end - start_time );
        float runTime = timeTemp.count();
    if (runTime > move_time ) {
         search_false++;
          std::cout << "time up" << "\n";
//        std::cout << search_false << " search is false\n" << std::endl;
        if (newBoard.in_check()) {
            check_score = (newBoard.data.player_to_play == PlayerColor::BLACK) ? 150:-150;  // If white is in check, subtract value. If black is in check, add value.
            //      score += check_score;
            //      std::cout << "score from evaluater at leaf" << evaluate(b) << "\n";
            //    std::cout << "score from adder at leaf" << score << "\n";
            (is_white)? evaluate(b) + check_score - num_moves: evaluate(b) + check_score + num_moves;
        }

        //     std::cout << "score from evaluater at leaf" << evaluate(b) << "\n";
        //    std::cout << "score from adder at leaf" << score << "\n";
        return (is_white)? evaluate(b) - num_moves: evaluate(b) + num_moves;
    }
    //  Board newBoard = *b.copy();

    auto moves = newBoard.get_legal_moves();
    std::vector<std::pair<int, U16>> movesorted;
    //std::vector<U16> moveset;
    //for(auto m:moves){
    //  moveset.push_back(m);
    //}
    //std::srand(time(NULL));
    //std::random_shuffle(std::begin(moveset), std::end(moveset));
    if (isMaximizingPlayer) {

        for (auto m:moves){
            //newBoard.do_move(m);
            movesorted.push_back(std::make_pair(-1,m));
            //undo_move(m,newBoard);
        }
        // std::sort(movesorted.begin(),movesorted.end());
        int maxEval = -2000;
        for (long unsigned int i = 0;i < movesorted.size();i++) {
            //newBoard = Board(b);
            newBoard.do_move_(movesorted[i].second);
            num_moves += 1;
            // std::cout << "score from evaluater at internal" << evaluate(b) << "\n";
            // std::cout << "score from adder at internal" << score << "\n";
            int eval = minimax(newBoard, depth - 1, false, alpha, beta,start_time);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            // undo_move(movesorted[i].second,newBoard);
            num_moves -= 1;
            if (beta <= alpha) {
                break;  // beta cutoff
            }
        }
        if (maxEval == -2000){
            maxEval += (is_white)? -3*num_moves: 3 *num_moves;
            if (newBoard.in_check()) {
                maxEval += (newBoard.data.player_to_play == PlayerColor::BLACK) ? 150:-150;

            }
            else{
                maxEval += 300;
            }
        }
        return maxEval;
    } else {
        for (auto m:moves){
            //newBoard.do_move(m);
            movesorted.push_back(std::make_pair(2,m));
            //undo_move(m,newBoard);
        }
        //std::sort(movesorted.begin(),movesorted.end());
        int minEval = 2000;
        for (long unsigned int i = 0;i < movesorted.size();i++) {
            // newBoard = Board(b);
            newBoard.do_move_(movesorted[i].second);
            num_moves += 1;
            int eval = minimax(newBoard, depth - 1, true, alpha, beta,start_time);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            // undo_move(movesorted[i].second,newBoard);
            num_moves -= 1;
            if (beta <= alpha) {
                break;  // alpha cutoff
            }
        }
        if (minEval == 2000){
            minEval += (is_white)? -3*num_moves: 3 *num_moves;
            if (newBoard.in_check()) {
                minEval += (newBoard.data.player_to_play == PlayerColor::BLACK) ? 150:-150;

            }
            else{
                minEval -= 300;
            }
        }
        return minEval;
    }
}





void Engine::find_best_move(const Board& b) {

auto start = std::chrono::high_resolution_clock::now();

    ply_counter = 0;
    depth_0 = 0;
    search_false = 0;

    auto moves = b.get_legal_moves();

    if (moves.size() == 0) {
        this->best_move = 0;
        return;
    }
    std::vector<U16> moveset;
    for(auto m:moves){
        moveset.push_back(m);
    }
    std::srand(time(NULL));
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(std::begin(moveset), std::end(moveset), g);
    Board newBoard(b);
    int initial_score = evaluate(newBoard);
    float runTime = 1;

        if(time_left.count() <= 4000){
            move_time = time_left.count() / 2;
            std::cout << "about to time_out: " << move_time << std::endl; 
        }
        else{
            move_time = std::max(float(1800),move_time-70);
        }

    while(runTime < move_time) {
        int bestValue;
        //num_moves = 0;
        // Minimax initialization
        is_white = false;
        if (b.data.player_to_play == WHITE) {
            is_white = true;
            bestValue = NEG_INF;
        } else { bestValue = INF; }
        U16 bestMove = 0;
        //newBoard = Board(b);
        std::vector<std::pair<int, U16>> movesorted;
        for (auto m: moves) {
            //newBoard.do_move(m);
            movesorted.push_back(std::make_pair(-1 *(initial_score), m));
            //std::cout << "score" << evaluate(newBoard) << "\n";
            //undo_move(m,newBoard);
        }
        // std::sort(movesorted.begin(), movesorted.end());

        for (long unsigned int i = 0; i < movesorted.size(); i++) {
            newBoard = Board(b);
            newBoard.do_move_(movesorted[i].second);
            num_moves += 1;

            if (is_white) {
                int moveValue = minimax(newBoard, ply_counter, false, NEG_INF,
                                        INF,start);  // assuming depth 3 for now, you can change as needed
                if (moveValue > bestValue) {
                    bestValue = moveValue;
                    bestMove = movesorted[i].second;
                }
            } else {
                int moveValue = minimax(newBoard, ply_counter, true, NEG_INF,
                                        INF,start);  // assuming depth 3 for now, you can change as needed
                if (moveValue < bestValue) {
                    bestValue = moveValue;
                    bestMove = movesorted[i].second;
                }
            }
            // newBoard.undo_last_move_without_flip_(movesorted[i].second);
            // newBoard.flip_player_();
            num_moves -= 1;
        }
        if(search_false == 0){

            this->best_move = bestMove;
            std::cout << "Best move: " << this->best_move  <<  "value="<< bestValue<< std::endl;
        }

        ply_counter++;
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::milliseconds timeTemp = std::chrono::duration_cast< std::chrono::milliseconds >( end - start );
            runTime = timeTemp.count();
    }
      num_moves += 1;
      
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Time taken by function: " << duration.count() << " ms" << std::endl;
    std::cout << "Number of ply: " << ply_counter << std::endl;
    std::cout << "Number of depth 0: " << depth_0 << std::endl;
    std::cout << "Number of search false: " << search_false << std::endl;
    std::cout << "number of moves: " << num_moves << std::endl;
}
