#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process
{
public:
    Process(int pid,
            std::string user,
            std::string command,
            float cpu,
            int ram,
            long int uptime)
        : pid_(pid), user_(user), command_(command), cpu_(cpu), ram_(ram), uptime_(uptime){};
    int Pid();
    std::string User();
    std::string Command();
    float CpuUtilization();
    std::string Ram();
    long int UpTime();
    bool operator<(Process const &a) const;

private:
    int pid_;
    std::string user_;
    std::string command_;
    float cpu_;
    int ram_;
    long int uptime_;
};

#endif