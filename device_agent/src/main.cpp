// device_agent/src/main.cpp
// Cross-platform telemetry agent (Linux + Windows)
// Build with CMake (provided) on Linux / Windows (MSVC or MinGW)

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
  #define NOMINMAX
  #include <windows.h>
#else
  // linux headers
  #include <unistd.h>
#endif

// ---------------- Linux impl ----------------
#ifndef _WIN32

#include <fstream>
#include <sstream>

double read_cpu_percent_linux() {
    std::ifstream f("/proc/stat");
    if (!f.is_open()) return 0.0;
    std::string line;
    std::getline(f, line);
    std::istringstream ss(line);
    std::string cpu;
    uint64_t user=0, nice=0, system=0, idle=0, iowait=0, irq=0, softirq=0, steal=0;
    ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    uint64_t idle_all = idle + iowait;
    uint64_t non_idle = user + nice + system + irq + softirq + steal;
    uint64_t total = idle_all + non_idle;

    static uint64_t last_total = 0, last_idle = 0;
    uint64_t totald = total - last_total;
    uint64_t idled = idle_all - last_idle;
    last_total = total;
    last_idle = idle_all;
    if (totald == 0) return 0.0;
    double cpu_percentage = (double)(totald - idled) * 100.0 / (double)totald;
    if (cpu_percentage < 0) cpu_percentage = 0;
    return cpu_percentage;
}

double read_mem_percent_linux() {
    std::ifstream f("/proc/meminfo");
    if (!f.is_open()) return 0.0;
    std::string line;
    uint64_t mem_total = 0, mem_free = 0, buffers = 0, cached = 0;
    while (std::getline(f, line)) {
        std::istringstream ss(line);
        std::string key;
        uint64_t value;
        std::string unit;
        ss >> key >> value >> unit;
        if (key == "MemTotal:") mem_total = value;
        else if (key == "MemFree:") mem_free = value;
        else if (key == "Buffers:") buffers = value;
        else if (key == "Cached:") cached = value;
        if (mem_total && mem_free && buffers && cached) break;
    }
    if (mem_total == 0) return 0.0;
    uint64_t used = mem_total - mem_free - buffers - cached;
    double used_percent = (double)used * 100.0 / (double)mem_total;
    return used_percent;
}

#endif // !_WIN32

// ---------------- Windows impl ----------------
#ifdef _WIN32

// CPU usage using GetSystemTimes. Simple two-sample calculation.
static bool filetime_to_uint64(const FILETIME &ft, uint64_t &out) {
    out = ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    return true;
}

double read_cpu_percent_windows() {
    static uint64_t last_idle = 0, last_total = 0;
    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) return 0.0;
    uint64_t idle, kernel, user;
    filetime_to_uint64(idleTime, idle);
    filetime_to_uint64(kernelTime, kernel);
    filetime_to_uint64(userTime, user);

    uint64_t sys = kernel + user;
    uint64_t total = sys + idle;

    uint64_t totald = total - last_total;
    uint64_t idled = idle - last_idle;
    last_total = total;
    last_idle = idle;
    if (totald == 0) return 0.0;
    double cpu = (double)(totald - idled) * 100.0 / (double)totald;
    if (cpu < 0) cpu = 0;
    return cpu;
}

double read_mem_percent_windows() {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    if (!GlobalMemoryStatusEx(&status)) return 0.0;
    double used = (double)(status.ullTotalPhys - status.ullAvailPhys);
    double perc = used * 100.0 / (double)status.ullTotalPhys;
    return perc;
}

#endif // _WIN32

// ---------------- Common ----------------
int main(int argc, char** argv) {
    std::string node_id = "node-1";
    int interval_ms = 1000;
    std::string logfile = "device_agent/device_agent.log";

    // CLI args
    for (int i=1;i<argc;++i) {
        std::string a = argv[i];
        if (a == "--node" && i+1<argc) node_id = argv[++i];
        else if (a == "--interval" && i+1<argc) interval_ms = std::stoi(argv[++i]);
        else if (a == "--log" && i+1<argc) logfile = argv[++i];
    }

    // Warm up CPU sampling
#ifndef _WIN32
    read_cpu_percent_linux();
#else
    read_cpu_percent_windows();
#endif
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::ofstream out(logfile, std::ios::app);
    if (!out.is_open()) {
        std::cerr << "Warning: failed to open logfile: " << logfile << std::endl;
    }

    while (true) {
        std::time_t t = std::time(nullptr);
#ifndef _WIN32
        double cpu = read_cpu_percent_linux();
        double mem = read_mem_percent_linux();
#else
        double cpu = read_cpu_percent_windows();
        double mem = read_mem_percent_windows();
#endif

        std::ostringstream out_line;
        out_line << "{";
        out_line << "\"node_id\":\"" << node_id << "\",";
        out_line << "\"timestamp\":" << t << ",";
        out_line << "\"cpu_percent\":" << (int)(cpu + 0.5) << ",";
        out_line << "\"mem_percent\":" << (int)(mem + 0.5);
        out_line << "}";

        std::string line = out_line.str();
        // stdout
        std::cout << line << std::endl;
        // logfile
        if (out.is_open()) {
            out << line << std::endl;
            out.flush();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
    }
    return 0;
}
