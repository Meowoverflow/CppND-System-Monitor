#include <string>
#include "processor.h"
#include "linux_parser.h"

using std::string;

float Processor::Utilization() {
  string line , cpu;
  float utilization;
  long user , nice , systemTime , idle , iowait , irq ,softirq , steal , guest , guestNice , idleTotla , nonIdleTotal , total;
  std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> systemTime >> idle >> iowait >> irq >> softirq >> steal >> guest >> guestNice ;
    idleTotla = idle + iowait;
    nonIdleTotal = user + nice + systemTime + irq + softirq + steal ;
    total = idleTotla + nonIdleTotal;
    utilization = (total - idleTotla) * 1.0 /total;
    return utilization;
  }
  return 0.0;
}