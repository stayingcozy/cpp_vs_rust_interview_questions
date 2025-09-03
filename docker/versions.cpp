#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <regex>
#include <sstream>
#include <cstdio>

namespace fs = std::filesystem;

const std::string BUILD_DIR = "build";

// Helper function to execute shell commands and capture output
std::string exec(const std::string& cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";
    
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    
    // Remove trailing newline
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

// Create file with content and return full path
std::string cat(const std::string& filename, const std::string& content = "") {
    std::string fullPath = BUILD_DIR + "/" + filename;
    
    if (!content.empty()) {
        std::ofstream file(fullPath);
        if (file.is_open()) {
            file << content << std::endl;
            file.close();
        }
    }
    
    return fullPath;
}

// Padding functions
std::string pad(int num, const std::string& str, char padChar = ' ') {
    std::string trimmed = str;
    // Simple trim
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    if (trimmed.length() >= num) return trimmed;
    
    std::string result = trimmed;
    while (result.length() < num) {
        result += padChar;
    }
    return result;
}

std::string lpad(const std::string& str, char padChar = ' ') {
    return pad(16, str, padChar);
}

std::string rpad(const std::string& str, char padChar = ' ') {
    return pad(31, str, padChar);
}

int main() {
    // Create build directory
    fs::create_directories(BUILD_DIR);
    
    // Language version detectors
    std::map<std::string, std::function<std::string()>> langs = {
        {"Rust", []() -> std::string {
            std::string output = exec("rustc --version");
            std::istringstream iss(output);
            std::string word;
            std::vector<std::string> words;
            while (iss >> word) {
                words.push_back(word);
            }
            return words.size() > 1 ? words[1] : "unknown";
        }},
        
        {"clang/clang++", []() -> std::string {
            std::string prog = R"(#include <stdio.h>
int main(void) {
    printf(__clang_version__);
    return 0;
})";
            
            std::string cmd = "clang++ -o " + cat("clang") + " " + cat("clang.cpp", prog) + " && " + cat("clang");
            return exec(cmd);
        }}
    };
    
    // Create table
    std::vector<std::string> table = {
        "| " + lpad("Language") + " | " + rpad("Version") + " |",
        "| " + lpad("", '-') + " | " + rpad("", '-') + " |"
    };
    
    // Sort languages by name and process each
    std::vector<std::pair<std::string, std::function<std::string()>>> sortedLangs(langs.begin(), langs.end());
    std::sort(sortedLangs.begin(), sortedLangs.end(), 
              [](const auto& a, const auto& b) { return a.first < b.first; });
    
    for (const auto& [name, versionFunc] : sortedLangs) {
        std::cerr << "Fetching " << name << " version..." << std::endl;
        
        std::string version;
        try {
            version = versionFunc();
        } catch (...) {
            version = "error";
        }
        
        if (version.empty()) {
            version = "not found";
        }
        
        table.push_back("| " + lpad(name) + " | " + rpad(version) + " |");
    }
    
    // Output table
    for (const std::string& line : table) {
        std::cout << line << std::endl;
    }
    
    std::cerr << std::endl;
    
    // Cleanup
    if (fs::exists(BUILD_DIR)) {
        fs::remove_all(BUILD_DIR);
    }
    
    return 0;
}