#include <bits/stdc++.h>
#include "engineUCI.hpp"

int main() {
    EngineUCI eng;
    std::string message;
    std::fstream file("D:\\Misc_Projects\\lichess-bot\\engines\\log.txt");
    if (!file.is_open()) {
        std::cerr << "Error opening the file." << std::endl;
        return 1;
    }
    while(std::getline(std::cin, message)) {
        file << message << std::endl;
        if (message == "quit") break;
        eng.receiveCommand(message);
    }

    file.close();
}