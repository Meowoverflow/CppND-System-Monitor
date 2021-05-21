#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid)
    : pid_(pid) ,
      command_(LinuxParser::Command(pid)) ,
      user_(LinuxParser::User(pid))
    { }

int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
//float Process::CpuUtilization() const { return LinuxParser::CpuUtilization(pid_); }
float Process::CpuUtilization() const { return LinuxParser::CpuUtilizationPro(pid_); }

string Process::Command() { return command_; }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return user_; }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const {
  return this->CpuUtilization() < a.CpuUtilization();
}