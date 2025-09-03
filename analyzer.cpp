#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <cstdlib>
#include <filesystem>

const char* RESULTS_LOG = "target/results.log";

struct Row {
    std::string name;
    std::string secs;
    std::string base;
    std::string mb;
    std::string joules;
};

struct FinalRow {
    std::string name;
    std::string secs;
    std::string mb;
    std::string joules;
};

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

double median(std::vector<double> array) {
    if (array.empty()) return 0.0;
    
    std::sort(array.begin(), array.end());
    size_t len = array.size();
    return (array[(len - 1) / 2] + array[len / 2]) / 2.0;
}

std::string sd(const std::vector<double>& list, double scale = 1.0, int precision = 2) {
    std::vector<double> values = list;
    double list_median = median(values);
    
    std::vector<double> deviations;
    for (double x : list) {
        deviations.push_back(std::abs(x - list_median));
    }
    
    double mad = median(deviations);
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) 
        << (list_median / scale) << "<sub>±" 
        << std::setfill('0') << std::setw(precision + 3) << std::setprecision(precision)
        << (mad / scale) << "</sub>";
    return oss.str();
}

std::string pad(int n, const std::string& str, char padchar = ' ') {
    std::string trimmed = trim(str);
    if (trimmed.length() >= static_cast<size_t>(n)) return trimmed;
    return std::string(n - trimmed.length(), padchar) + trimmed;
}

int main(int argc, char* argv[]) {
    int ATTEMPTS = 10;
    const char* attempts_env = std::getenv("ATTEMPTS");
    if (attempts_env) {
        ATTEMPTS = std::stoi(attempts_env);
    }
    
    if (argc > 1) {
        std::filesystem::remove(RESULTS_LOG);
        
        for (int n = 0; n < ATTEMPTS; n++) {
            std::cout << "--- Iteration " << (n + 1) << std::endl;
            
            std::string command;
            for (int i = 1; i < argc; i++) {
                if (i > 1) command += " ";
                command += argv[i];
            }
            
            setenv("QUIET", "1", 1);
            int result = system(command.c_str());
            if (result != 0) {
                return 1;
            }
        }
    }
    
    std::map<std::string, std::string> name_repl = {
        {"ruby", "Ruby"},
        {"jruby", "Ruby/jruby"},
        {"gccgo", "Go/gccgo"},
        {"gc", "Go"},
        {"CPython", "Python"},
        {"PyPy", "Python/pypy"},
        {"Digital Mars D", "D/dmd"},
        {"GNU D", "D/gdc"},
        {"LDC", "D/ldc2"}
    };
    
    std::vector<Row> lines;
    std::ifstream file(RESULTS_LOG);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open " << RESULTS_LOG << std::endl;
        return 1;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> values = split(line, '\t');
        if (values.size() >= 5) {
            Row row;
            std::string name = values[0];
            row.name = name_repl.count(name) ? name_repl[name] : name;
            row.secs = values[1];
            row.base = values[2];
            row.mb = values[3];
            row.joules = trim(values[4]);
            lines.push_back(row);
        }
    }
    file.close();
    
    std::vector<std::string> keys;
    for (const auto& row : lines) {
        if (std::find(keys.begin(), keys.end(), row.name) == keys.end()) {
            keys.push_back(row.name);
        }
    }
    
    std::vector<FinalRow> results;
    for (const std::string& k : keys) {
        std::vector<Row> rows;
        for (const auto& line : lines) {
            if (line.name == k) {
                rows.push_back(line);
            }
        }
        
        if (static_cast<int>(rows.size()) != ATTEMPTS) {
            std::cerr << "Integrity check failed (" << k << ")" << std::endl;
            return 1;
        }
        
        std::vector<double> secs_vals, base_vals, mb_vals, joules_vals;
        for (const auto& row : rows) {
            secs_vals.push_back(std::stod(row.secs));
            base_vals.push_back(std::stod(row.base));
            mb_vals.push_back(std::stod(row.mb) - std::stod(row.base));
            joules_vals.push_back(std::stod(row.joules));
        }
        
        std::string secs = sd(secs_vals, 1e9, 3);
        std::string base = sd(base_vals, 1048576.0);
        std::string mb = sd(mb_vals, 1048576.0);
        std::string joules = sd(joules_vals);
        
        FinalRow final_row;
        final_row.name = k;
        final_row.secs = secs;
        final_row.mb = base + " + " + mb;
        final_row.joules = joules;
        results.push_back(final_row);
    }
    
    // Sort results by secs and mb
    std::sort(results.begin(), results.end(), [](const FinalRow& a, const FinalRow& b) {
        double a_secs = std::stod(a.secs.substr(0, a.secs.find('<')));
        double b_secs = std::stod(b.secs.substr(0, b.secs.find('<')));
        if (a_secs != b_secs) return a_secs < b_secs;
        
        // If secs are equal, compare mb (simplified)
        return a.mb < b.mb;
    });
    
    // Calculate max lengths for padding
    size_t max_name_len = 0, max_secs_len = 0, max_mb_len = 0, max_joules_len = 0;
    for (const auto& row : results) {
        max_name_len = std::max(max_name_len, row.name.length());
        max_secs_len = std::max(max_secs_len, row.secs.length());
        max_mb_len = std::max(max_mb_len, row.mb.length());
        max_joules_len = std::max(max_joules_len, row.joules.length());
    }
    
    // Ensure minimum widths for headers
    max_name_len = std::max(max_name_len, std::string("Language").length());
    max_secs_len = std::max(max_secs_len, std::string("Time, s").length());
    max_mb_len = std::max(max_mb_len, std::string("Memory, MiB").length());
    max_joules_len = std::max(max_joules_len, std::string("Energy, J").length());
    
    // Create table
    std::vector<std::string> table;
    
    // Header row
    std::string header = "| " + pad(max_name_len, "Language") + " | " +
                        pad(max_secs_len, "Time, s") + " | " +
                        pad(max_mb_len, "Memory, MiB") + " | " +
                        pad(max_joules_len, "Energy, J") + " |";
    table.push_back(header);
    
    // Separator row
    std::string separator = "| :" + std::string(max_name_len - 1, '-') + " | " +
                           std::string(max_secs_len - 1, '-') + ": | " +
                           std::string(max_mb_len - 1, '-') + ": | " +
                           std::string(max_joules_len - 1, '-') + ": |";
    table.push_back(separator);
    
    // Data rows
    for (const auto& row : results) {
        std::string data_row = "| " + pad(max_name_len, row.name) + " | " +
                              pad(max_secs_len, row.secs) + " | " +
                              pad(max_mb_len, row.mb) + " | " +
                              pad(max_joules_len, row.joules) + " |";
        table.push_back(data_row);
    }
    
    // Print table
    for (const auto& row : table) {
        std::cout << row << std::endl;
    }
    
    return 0;
}