#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem()
{
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "PRETTY_NAME")
        {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel()
{
  string os, version_tag, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version_tag >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids()
{
  vector<int> pids;
  DIR *directory = opendir(kProcDirectory.c_str());
  struct dirent *file;
  while ((file = readdir(directory)) != nullptr)
  {
    // Is this a directory?
    if (file->d_type == DT_DIR)
    {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit))
      {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization()
{
  float utilization = 0.0f;
  long int memTotal = 0, memFree = 0, cached = 0;
  string line;
  string key;
  int value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "MemTotal")
        {
          memTotal = value;
        }
        else if (key == "MemFree")
        {
          memFree = value;
        }
        else if (key == "Cached")
        {
          cached = value;
        }
      }
    }
    if (memTotal > 0)
    {
      utilization = (float)(memTotal - memFree - cached) / memTotal;
    }
  }
  return utilization;
}

long LinuxParser::UpTime()
{
  string line;
  long uptimeIncludingSuspend;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open() && std::getline(stream, line))
  {
    std::istringstream linestream(line);
    linestream >> uptimeIncludingSuspend;
  }
  return uptimeIncludingSuspend;
}

vector<int> LinuxParser::CpuUtilization()
{
  string line, cpu_key;
  int stat;
  vector<int> cpuStats;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open() && std::getline(stream, line))
  {
    std::istringstream linestream(line);
    linestream >> cpu_key;
    while (linestream >> stat)
    {
      cpuStats.push_back(stat);
    }
  }
  return cpuStats;
}

float LinuxParser::CpuUtilization(int pid)
{
  string line, cpu_value;
  long unsigned int utime, stime, cutime, cstime, starttime;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open() && std::getline(stream, line))
  {
    std::istringstream linestream(line);
    for (int i = 1; i <= 22; ++i)
    {
      switch (i)
      {
      case 14:
        linestream >> utime;
        break;
      case 15:
        linestream >> stime;
        break;
      case 16:
        linestream >> cutime;
        break;
      case 17:
        linestream >> cstime;
        break;
      case 22:
        linestream >> starttime;
        break;
      default:
        linestream >> cpu_value;
        break;
      }
    }
    float total_time = (float)(utime + stime + cutime + cstime) / sysconf(_SC_CLK_TCK);
    long unsigned int seconds = UpTime() - (starttime / sysconf(_SC_CLK_TCK));
    if (seconds)
    {
      return total_time / seconds;
    }
  }
  return 0.0f;
}

int LinuxParser::TotalProcesses()
{
  int processesTotal = 0;
  string line;
  string key;
  int value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "processes")
        {
          processesTotal = value;
        }
      }
    }
  }
  return processesTotal;
}

int LinuxParser::RunningProcesses()
{
  int processesRunning = 0;
  string line;
  string key;
  int value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "procs_running")
        {
          processesRunning = value;
        }
      }
    }
  }
  return processesRunning;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid)
{
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open() && std::getline(stream, line))
  {
    return line;
  }
  return string();
}

// Read and return the memory used by a process
int LinuxParser::Ram(int pid)
{
  string line;
  string key;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key)
      {
        if (key == "VmSize:")
        {
          int size;
          linestream >> size;
          return size / 1000;
        }
      }
    }
  }
  return 0;
}

string LinuxParser::Uid(int pid)
{
  string line;
  string key;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key)
      {
        if (key == "Uid:")
        {
          string uid;
          linestream >> uid;
          return uid;
        }
      }
    }
  }
  return string();
}

string LinuxParser::User(int pid)
{
  string uid = Uid(pid);

  string line, userNameStatus, xValueStatus, uidStatus;

  std::ifstream stream(kPasswordPath);
  if (stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> userNameStatus >> xValueStatus >> uidStatus)
      {
        if (uidStatus == uid)
        {
          return userNameStatus;
        }
      }
    }
  }

  return string();
}

// Read and return the uptime of a process in seconds
long LinuxParser::UpTime(int pid)
{
  unsigned long long uptime = 0;
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open() && std::getline(stream, line))
  {
    std::istringstream linestream(line);
    for (int i = 1; i <= 22; i++)
    {
      linestream >> value;
    }
    // the value is expressed in clock ticks. Divided by sysconf(_SC_CLK_TCK) to get seconds
    uptime = UpTime() - stoull(value) / sysconf(_SC_CLK_TCK);
  }
  return uptime;
}