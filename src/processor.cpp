#include "processor.h"
#include "linux_parser.h"

#include <vector>

using std::vector;

float Processor::Utilization()
{
    vector<int> stats = LinuxParser::CpuUtilization();

    int user = stats[0];
    int nice = stats[1];
    int system = stats[2];
    int idleStat = stats[3];
    int iowait = stats[4];
    int irq = stats[5];
    int softirq = stats[6];
    int steal = stats[7];

    int prevIdle = idle_;
    int idle = idleStat + iowait;

    int prevNonIdle = nonIdle_;
    int nonIdle = user + nice + system + irq + softirq + steal;

    int prevTotal = prevIdle + prevNonIdle;
    int total = idle + nonIdle;

    // differentiate : actual value minus the previous one
    int totald = total - prevTotal;
    int idled = idle - prevIdle;

    idle_ = idle;
    nonIdle_ = nonIdle;

    return (float)(totald - idled) / totald;
}