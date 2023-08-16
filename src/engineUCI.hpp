#ifndef ENGINE_UCI_H
#define ENGINE_UCI_H
#include <iostream>
#include <algorithm>
#include <iomanip>
#include "bot.hpp"
#include <fstream>

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

void print_string_in_hex(const std::string& str) {
    for (unsigned char c : str) {
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    }
    std::cout << std::endl;
}

bool containsSubstring(const std::string &mainString, const std::string &subString) {
    return mainString.find(subString) != std::string::npos;
}

std::string TryGetLabelledValue(const std::string &text, const std::string &label, const std::vector<std::string> &allLabels, const std::string &defaultValue = "") {
    std::string modifiedText = text;
    // modifiedText.erase(std::remove_if(modifiedText.begin(), modifiedText.end(), ::isspace), modifiedText.end());

    if (modifiedText.find(label) != std::string::npos) {
        size_t valueStart = modifiedText.find(label) + label.length();
        size_t valueEnd = modifiedText.length();

        for (const std::string &otherID : allLabels) {
            if (otherID != label && modifiedText.find(otherID) != std::string::npos) {
                size_t otherIDStartIndex = modifiedText.find(otherID);
                if (otherIDStartIndex > valueStart && otherIDStartIndex < valueEnd) {
                    valueEnd = otherIDStartIndex;
                }
            }
        }

        return modifiedText.substr(valueStart, valueEnd - valueStart);
    }

    trim(defaultValue);
    return defaultValue;
}

static int TryGetLabelledValueInt(const std::string &text, const std::string &label, const std::vector<std::string> &allLabels, int defaultValue = 0) {
    std::string valueString = TryGetLabelledValue(text, label, allLabels, std::to_string(defaultValue));
    std::istringstream iss(valueString);
    int result;

    if (iss >> result) {
        return result;
    }

    return defaultValue;
}


class EngineUCI {
public:
    BOT bot;
    std::vector<std::string> positionLabels{ "position", "fen", "moves" };
    std::vector<std::string> goLabels{ "go", "movetime", "wtime", "btime", "winc", "binc", "movestogo" };
    std::fstream file;

    EngineUCI() {
        file.open("./log.txt");
    }

    void receiveCommand(std::string message) {
        message = trim(message);
        std::vector<std::string> messageArray;
        std::istringstream iss(message);
        for (std::string word; iss >> word; ) {
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            messageArray.push_back(word);
        }
        
        std::string msgType = messageArray[0];
        if (msgType == "uci") {
            file << "uciok" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (msgType == "isready") {
            file << "readyok" << std::endl;
            std::cout << "readyok" << std::endl;
        } else if (msgType == "ucinewgame") {
            bot.notifyNewGame();
        } else if (msgType == "position") {
            processPositionCommand(message);
        } else if (msgType == "go") {
            processGoCommand(message);
        } else if (msgType == "d") {
            bot.print_board();
        }
    }

    void processGoCommand(std::string message) {
        if (containsSubstring(message, "movetime")) {
            int moveTimeMs = TryGetLabelledValueInt(message, "movetime", goLabels, 0);
            bot.thinkTimed(moveTimeMs);
        } else {
            int timeRemainingWhiteMs = TryGetLabelledValueInt(message, "wtime", goLabels, 0);
			int timeRemainingBlackMs = TryGetLabelledValueInt(message, "btime", goLabels, 0);
			int incrementWhiteMs = TryGetLabelledValueInt(message, "winc", goLabels, 0);
			int incrementBlackMs = TryGetLabelledValueInt(message, "binc", goLabels, 0);

            int thinkTime = bot.chooseThinkTime(timeRemainingWhiteMs, timeRemainingBlackMs, incrementWhiteMs, incrementBlackMs);
            bot.thinkTimed(thinkTime);
        }
    }

    void processPositionCommand(std::string message) {
        if (containsSubstring(message, "startpos")) {
            bot.notifyNewGame();
        } else if (containsSubstring(message, "fen")) {
            std::string customFen = TryGetLabelledValue(message, "fen", positionLabels);
            customFen = trim(customFen);
            bot.setPosition(customFen);
        } else {
            file << "Invalid position command (expected 'startpos' or 'fen')" << std::endl;
            std::cout << "Invalid position command (expected 'startpos' or 'fen')" << std::endl;
        }

        std::string allMoves = TryGetLabelledValue(message, "moves", positionLabels);
        if (allMoves.size()) {
            std::vector<std::string> movesArray;
            std::istringstream iss(allMoves);
            for (std::string word; iss >> word; ) {
                std::transform(word.begin(), word.end(), word.begin(), ::tolower);
                movesArray.push_back(word);
            }
            for (auto move: movesArray) {
                bot.makeMove(move);
            }
        }
    }
};

#endif