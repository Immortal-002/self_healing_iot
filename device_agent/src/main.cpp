#include<iostream>
#include<chrono>
#include<thread>
#include<ctime>
#include<fstream>
#include<string>
#include<sstream>

double read_cpu_percent(){

    std::ifstream f("/proc/stat");
    if(!f.is_open()) return 0.0;
    std::string line;
    std::getline(f,line);
    std::istringstream ss(line);
    std::string cpu;
    uint64_t user=0, nice=0, system=0, idle=0, iowait=0, irq=0, softirq=0, steal=0;
    ss>>cpu>>user>>nice>>system>>idle>>iowait>>irq>> softirq>>steal;
    uint64_t idle_all = idle + iowait;
    uint64_t non_idle = user + nice + system + irq + softirq + steal;
    uint64_t total = idle_all + non_idle;
    static uint64_t last_total=0, last_idle=0;
    uint64_t totald = total - last_total;
    uint64_t idled = idle_all - last_idle;
    last_total = total;
    last_idle = idle_all;
    if (totald == 0) return 0.0;
    double cpu_percentage = (double)(totald - idled) * 100.0 / (double)totald;
    return cpu_percentage;
}

double read_mem_percent() {
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




int main(int argc, char** argv)
{
   const char* node_id = "node-1";
   int interval_ms = 1000;
   std::string logfile="device_agent/device_agent.log";
   for ( int i=1; i<argc;++i){    // skip i=0 because thats program name itself

       std::string a=argv[i];
        if (a == "--node" && i + 1 < argc) { node_id = argv[++i]; }
        else if (a == "--interval" && i + 1 < argc) { interval_ms = std::stoi(argv[++i]); }
        else if (a == "--log" && i + 1 < argc) { logfile = argv[++i]; }
    }


   read_cpu_percent();

   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   std::ofstream out(logfile, std::ios::app);
    if (!out.is_open()) {
        std::cerr << "Warning: failed to open logfile: " << logfile << std::endl;
    }


   while(true) {
        std::time_t t = std::time(nullptr);
        double cpu=read_cpu_percent();
        double mem=read_mem_percent();

        std::ostringstream out_line;
        out_line << "{";
        out_line<< "\"node_id\":\"" << node_id << "\",";
        out_line << "\"timestamp\":" << t << ",";
        out_line << "\"cpu_percent\":" << (int)(cpu+0.5) << ",";
        out_line << "\"mem_percent\":" << (int)(mem+0.5);
        out_line << "}";
        std::string line=out_line.str();
       

         // Print to stdout
        std::cout << line << std::endl;

        // Append to logfile if available
        if (out.is_open()) {
            out << line << std::endl;
            out.flush();
        } 

       std::this_thread::sleep_for(std::chrono::seconds(1));
}


    return 0;
}
