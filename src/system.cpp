#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor &System::Cpu() { return cpu_; }

vector<Process> &System::Processes()
{
    processes_.clear();
    vector<int> pids = LinuxParser::Pids();
    for (const int &pid : pids)
    {
        string user = LinuxParser::User(pid);
        string command = LinuxParser::Command(pid);
        float cpu = LinuxParser::CpuUtilization(pid);
        int ram = LinuxParser::Ram(pid);
        int uptime = LinuxParser::UpTime(pid);

        Process process{pid, user, command, cpu, ram, uptime};
        processes_.push_back(process);
    }
    std::sort(processes_.begin(), processes_.end());
    return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }