#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <chrono>
#include <filesystem>
#include <iomanip>

long PAGE_SIZE;
bool HAS_MEM;
const char* RESULTS_LOG = "target/results.log";

long read_mem(pid_t pid) {
    if (HAS_MEM) {
        std::string path = "/proc/" + std::to_string(pid) + "/statm";
        std::ifstream file(path);
        if (file.is_open()) {
            std::string line;
            std::getline(file, line);
            std::istringstream iss(line);
            std::string vsize, rss;
            iss >> vsize >> rss;
            return PAGE_SIZE * std::stoll(rss);
        }
    }
    return 0;
}

class EnergyStats {
private:
    static const char* PATH;
    long long acc_e;
    long long e;
    long long max_e;

public:
    bool has_energy_metrics;
    
    EnergyStats() : acc_e(0), e(0), has_energy_metrics(false) {
        std::ifstream file(PATH);
        if (file.is_open()) {
            has_energy_metrics = true;
            file >> max_e;
        }
    }
    
    void update() {
        if (!has_energy_metrics) return;
        
        std::ifstream file(PATH);
        if (!file.is_open()) return;
        
        long long new_e;
        file >> new_e;
        
        if (e == 0) {
            acc_e = 0;
        } else if (new_e > e) {
            acc_e += new_e - e;
        } else if (new_e < e) {
            acc_e += max_e - e + new_e;
        }
        e = new_e;
    }
    
    double val() {
        return 0.000001 * acc_e;
    }
};

const char* EnergyStats::PATH = "/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj";

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int main(int argc, char* argv[]) {
    // Get page size
    PAGE_SIZE = sysconf(_SC_PAGESIZE);
    
    // Check if /proc/self/statm exists
    HAS_MEM = std::filesystem::exists("/proc/self/statm");
    
    EnergyStats energy_stats;
    
    // Create TCP server
    int server_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Allow IPv4 and IPv6
    int v6only = 0;
    setsockopt(server_fd, IPPROTO_IPV6, IPV6_V6ONLY, &v6only, sizeof(v6only));
    
    struct sockaddr_in6 address;
    address.sin6_family = AF_INET6;
    address.sin6_addr = in6addr_any;
    address.sin6_port = htons(9001);
    
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }
    
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }
    
    // Spawn process
    std::vector<char*> args;
    for (int i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }
    args.push_back(nullptr);
    
    pid_t spawned_pid = fork();
    if (spawned_pid == 0) {
        execvp(args[0], args.data());
        std::cerr << "execvp failed" << std::endl;
        return 1;
    }
    
    // Set server socket to non-blocking for initial accept
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);
    
    int client_fd;
    while (true) {
        client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd >= 0) break;
        
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            std::cerr << "Accept failed" << std::endl;
            return 1;
        }
        
        int status;
        if (waitpid(spawned_pid, &status, WNOHANG) != 0) {
            return 1;
        }
        
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        struct timeval tv = {1, 0};
        select(server_fd + 1, &readfds, nullptr, nullptr, &tv);
    }
    
    // Read test data
    char buffer[1024];
    int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    buffer[bytes_read] = '\0';
    
    std::string test_data(buffer);
    // Remove newline
    if (!test_data.empty() && test_data.back() == '\n') {
        test_data.pop_back();
    }
    
    std::vector<std::string> parts = split(test_data, '\t');
    std::string test_name = parts[0];
    pid_t pid = std::stoi(parts[1]);
    
    std::cout << "\"" << test_name << "\"" << std::endl;
    
    close(client_fd);
    
    // Start timing
    auto start_time = std::chrono::steady_clock::now();
    long mem = read_mem(pid);
    long base_mem = mem;
    energy_stats.update();
    
    // Set server back to blocking
    fcntl(server_fd, F_SETFL, flags);
    
    // Monitor until second connection
    while (true) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        struct timeval tv = {0, 10000}; // 0.01 seconds
        
        int result = select(server_fd + 1, &readfds, nullptr, nullptr, &tv);
        if (result > 0) break;
        
        long m = read_mem(pid);
        if (m > mem) mem = m;
        energy_stats.update();
    }
    
    // Accept second connection
    client_fd = accept(server_fd, nullptr, nullptr);
    recv(client_fd, buffer, sizeof(buffer), 0);
    energy_stats.update();
    
    auto end_time = std::chrono::steady_clock::now();
    auto t_diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    
    double seconds = t_diff / 1e9;
    double mb = mem / 1048576.0;
    
    std::ostringstream stats;
    stats << std::fixed << std::setprecision(3) << seconds << " s, " 
          << std::setprecision(1) << mb << " Mb";
    
    // Create directory if it doesn't exist
    std::filesystem::create_directories(std::filesystem::path(RESULTS_LOG).parent_path());
    
    std::ofstream log_file(RESULTS_LOG, std::ios::app);
    if (energy_stats.has_energy_metrics) {
        double energy_val = energy_stats.val();
        stats << ", " << std::setprecision(1) << energy_val << " J";
        log_file << test_name << "\t" << t_diff << "\t" << base_mem << "\t" << mem << "\t" << energy_val << std::endl;
    } else {
        stats << ", 0.0 J";
        log_file << test_name << "\t" << t_diff << "\t" << base_mem << "\t" << mem << "\t" << "0.0" << std::endl;
    }
    log_file.close();
    
    waitpid(spawned_pid, nullptr, 0);
    std::cerr << stats.str() << std::endl;
    
    close(client_fd);
    close(server_fd);
    
    return 0;
}