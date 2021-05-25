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
    : pid_(pid) 
    { 
      this->Command(pid);
      this->User(pid);
    }

int Process::Pid() { return pid_; }
void Process::Command(int pid) {
  this->command_ = LinuxParser::Command(pid);
}
void Process::User(int pid) {
  this->user_ = LinuxParser::User(pid);
}
float Process::CpuUtilization() const { return LinuxParser::CpuUtilization(pid_); }

string Process::Command() { return command_; }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return user_; }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const {
  return this->CpuUtilization() < a.CpuUtilization();
}
bool Process::operator>(Process const& a) const {
  return this->CpuUtilization() > a.CpuUtilization();
}