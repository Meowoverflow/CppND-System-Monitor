#include "system.h"

#include <unistd.h>
#include <utime.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <utility>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System()
    : kernel_(LinuxParser::Kernel()),
      os_(LinuxParser::OperatingSystem())
{}

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  vector pids = LinuxParser::Pids();
  vector<Process> newProcesses;
  for(auto pid : pids) {
    newProcesses.push_back(Process(pid));
  }
  processes_ = std::move(newProcesses);
  return processes_;
}

std::string System::Kernel() { return kernel_ ;}

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return os_; }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }