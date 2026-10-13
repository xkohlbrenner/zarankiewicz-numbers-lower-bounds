#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <.txt>\n";
        return 1;
    }

    std::ifstream in(argv[1]);

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::string cmd = "./build/main " + line;
        std::cout << "Executing: " << line << std::endl;
        std::cout << "-----------------------------------------" << std::endl;
        std::system(cmd.c_str());
        std::cout << std::endl;
        std::cout << std::endl;
    }
    return 0;
}