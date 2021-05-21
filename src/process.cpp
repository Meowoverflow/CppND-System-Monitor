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
float Process::CpuUtilization() { return LinuxParser::CpuUtilization(pid_); }

string Process::Command() { return command_; }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return user_; }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }