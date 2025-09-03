#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << R"(Usage: run.cpp [command]
Commands:
  versions              Print installed language versions
  shell                 Start the shell
  parity                Build and run Primitive Types Parity
)" << std::endl;
        return 0;
    }
    
    std::string command = argv[1];
    
    if (command == "versions") {
        if (system("clang++ -o versions versions.cpp") == 0) {
            execl("./versions", "./versions", nullptr);
            perror("execl failed");
        }
        return 1;
    }
    else if (command == "shell") {
        if (chdir("/src") != 0) {
            perror("chdir failed");
            return 1;
        }
        execl("/bin/bash", "bash", nullptr);
        // If execl fails, it returns here
        perror("execl failed");
        return 1;
    }
    else if (command == "parity") {
        if (chdir("/src/primitive_types/parity") != 0) {
            perror("chdir failed");
            return 1;
        }
        execl("/usr/bin/make", "make", "run", nullptr);
        // If execl fails, it returns here
        perror("execl failed");
        return 1;
    }
    else {
        std::cout << R"(Usage: run.cpp [command]
Commands:
  versions              Print installed language versions
  shell                 Start the shell
  parity                Build and run Primitive Types Parity
)" << std::endl;
        return 1;
    }
    
    return 0;
}