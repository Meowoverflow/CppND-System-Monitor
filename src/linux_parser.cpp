#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <stdexcept>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >>kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  float memory_total_free[2]; // memory_total_free[0] = total memory , memory_total_free[1] = free memory
  std::string dummy,line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    for (int i = 0; i < 2; ++i) {
      std::getline(filestream, line);
      std::istringstream linestream(line);
      linestream >> dummy >> memory_total_free[i];
    }
  }
  if (memory_total_free[0] == 0)
    throw std::runtime_error("Math and parsing errors: Attempted to divide by Zero , total memory = 0\n");
  return (memory_total_free[0] - memory_total_free[1]) / memory_total_free[0] ; //used_memory / total_memory
}

long LinuxParser::UpTime() {
  float uptime;
  std::string line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream,line);
    std::istringstream linestream(line);
    linestream >> uptime ;
    return (long)uptime;
  }
  return 0;
}

int LinuxParser::TotalProcesses() {
  int totalProcesses;
  string line , processes;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> processes >> totalProcesses;
      if (processes.compare("processes") == 0)
        return totalProcesses;
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  int runningProcs;
  std::string line , processes;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> processes >> runningProcs;
      if (processes == "procs_running")
        return runningProcs;
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return string();
}
// used VmData instead of VmSize as in suggestions.
string LinuxParser::Ram(int pid) {
  string line , vmDataKey  ;
  int vmDataValue;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> vmDataKey >> vmDataValue;
      if (vmDataKey == "VmData:") {
        vmDataValue = vmDataValue / 1000;
        return to_string(vmDataValue);
      }
    }
  }
  return string();
}

string LinuxParser::Uid(int pid) {
  string line ;
  string tmp ;
  string uid;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> tmp >> uid;
      if (tmp.compare("Uid:") == 0)
        return uid;
    }
  }
  return string();
}

string LinuxParser::User(int pid) {
  string line ;
  string uname ;
  string dummy ;
  string  uid;
  string uid_key = LinuxParser::Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> uname >> dummy >> uid;
      if (uid == uid_key) return uname;
    }
  }
  return string();

}

long LinuxParser::UpTime(int pid) {
  string line ;
  string dummy;
  long startTime;
  auto systemUpTime = LinuxParser::UpTime();
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    getline(filestream , line);
    std::istringstream linestream(line);
    for (int i = 0; i < 21; ++i) {
      linestream >>  dummy ; //(22) starttime
    }
    linestream >> startTime;
    return systemUpTime - (startTime * 1.0 /sysconf(_SC_CLK_TCK));
  }
  return 0;
}

float LinuxParser::CpuUtilization  (int pid) {
  vector<string> tokens(22);
  string line , token;
  long uptime , utime , stime , cutime , cstime , starttime , totalTime;
  double seconds ;
  float cpuUtilization;
  float hertz = sysconf(_SC_CLK_TCK);
  uptime = LinuxParser::UpTime();
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    getline(filestream , line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; ++i) {
      linestream >>  token ; //(22) starttime
      tokens[i] = token;
    }
    utime = stol(tokens[13]) ; // #14
    stime = stol(tokens[14]) ; // #15
    cutime = stol(tokens[15]) ; // #16
    cstime = stol(tokens[16]) ; // #17
    starttime = stol(tokens[21]) ; // #22
    totalTime = utime + stime + cutime + cstime;
    seconds = uptime - (starttime / hertz);
    cpuUtilization =  (totalTime / hertz) * 1.0 / seconds ;
    return cpuUtilization;
  }
  return 0.0;
}
