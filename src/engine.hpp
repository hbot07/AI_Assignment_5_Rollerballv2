#pragma once

#include "engine_base.hpp"
#include <atomic>

class Engine : public AbstractEngine {

    // add extra items here. 
    // Note that your engine will always be instantiated with the default 
    // constructor.
    int num_moves = 0;
    float move_time = 2500;
    int minimax(const Board &b, int depth, bool isMaximizingPlayer, int alpha, int beta,auto start_time);
    public:
    void find_best_move(const Board& b) override;

};
