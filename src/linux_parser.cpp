#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <thread>
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

// ToDo BONUS: Update this to use std::filesystem
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

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

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
    //std::istringstream linestream(line);
    return line;
  }
  return string();
}

string LinuxParser::Ram(int pid) {
  string line , vmsizeKey  ;
  int vmsizeValue;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> vmsizeKey >> vmsizeValue;
      if (vmsizeKey == "VmSize:") return to_string(vmsizeValue / 1000);
    }
  }
  return string();
}

string LinuxParser::Uid(int pid) {
  string line , tmp , uid;
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
  string line , uname , x , uid;
  string uid_key = LinuxParser::Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> uname >> x >> uid;
      if (uid == uid_key) return uname;
    }
  }
  return string();

}

long LinuxParser::UpTime(int pid) {
  string line , starttime;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    getline(filestream , line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; ++i) {
      linestream >>  starttime ; //(22) starttime
    }
    return stol(starttime)/sysconf(_SC_CLK_TCK);
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
  }
  utime = stol(tokens[13]) ; // #14
  stime = stol(tokens[14]) ; // #15
  cutime = stol(tokens[15]) ; // #16
  cstime = stol(tokens[16]) ; // #17
  starttime = stol(tokens[21]) ; // #15
  totalTime = utime + stime + cutime + cstime;
  seconds = uptime - (starttime / hertz);
  cpuUtilization =  ((totalTime / hertz) / seconds) ;
  return cpuUtilization;
}

float LinuxParser::CpuUtilizationPro(int pid) {
  float utilOld , utilNew ;
  utilOld = LinuxParser::CpuUtilization(pid);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  utilNew = LinuxParser::CpuUtilization(pid);
  if (utilOld > utilNew)
    return 0.0;
  return (utilNew - utilOld) ;
}