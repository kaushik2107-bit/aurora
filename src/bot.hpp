#ifndef BOT_H
#define BOT_H
#include "analysis.hpp"

class BOT {
public:
    const bool useOpeningBook = false;
    const int maxBookPly = 16;
    const bool useMaxThinkTime = false;
	const int maxThinkTimeMs = 2500;
    Analysis analysis;

    void notifyNewGame() {
        analysis.notifyNewGame();
    }

    void setPosition(std::string newFen) {
        analysis.notifyNewGame(newFen);
    }

    void makeMove(std::string move) {
        analysis.generate_legal_moves();
        analysis.push(move);
        analysis.generate_legal_moves();
    }

    int chooseThinkTime(int timeRemainingWhiteMs, int timeRemainingBlackMs, int incrementWhiteMs, int incrementBlackMs) {
        int myTimeRemainingMs = analysis.get_player() == "White" ? timeRemainingWhiteMs : timeRemainingBlackMs;
        int myIncrementMs = analysis.get_player() == "White" ? incrementWhiteMs : incrementBlackMs;

        double thinkTimeMs = myTimeRemainingMs / 40.0;
        if (useMaxThinkTime)
		{
			thinkTimeMs = std::min((double)maxThinkTimeMs, thinkTimeMs);
		}

        if (myTimeRemainingMs > myIncrementMs * 2)
		{
			thinkTimeMs += myIncrementMs * 0.8;
		}
        double minThinkTime = std::min(50.0, myTimeRemainingMs * 0.25);
        return (int)std::ceil(std::max(minThinkTime, thinkTimeMs));
    }

    void thinkTimed(int timeMs) {
        startSearch(timeMs);
    }

    void startSearch(int timeMs) {
        std::vector<std::string> move; double evaluation; int nodeCount;
        std::tie(move, evaluation, nodeCount) = analysis.iterative_deepening(40, timeMs);
        // for (auto x: move) std::cout << x << " ";
        // std::cout << std::endl;
        std::cout << "bestmove " << move.front() << std::endl;
    }

    void print_board() {
        analysis.print_board();
    }

    void set_debug(bool debug_state) {
        analysis.debug = debug_state;
    }
};


#endif