#include <string>
#include "processor.h"
#include "linux_parser.h"

using std::string;
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  string line;
  float utilization;
  long user , nice , systemTime , idle , iowait , irq ,softirq , steal , guest , guestNice , idleTotla , nonIdleTotal , total;
  std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> user >> nice >> systemTime >> idle >> iowait >> irq >> softirq >> steal >> guest >> guestNice ;
    idleTotla = idle + iowait;
    nonIdleTotal = user + nice + systemTime + irq + softirq + steal ;
    total = idleTotla + nonIdleTotal;
    utilization = (total - idleTotla)/total;
    return utilization * 1.0;
  }
  return 0.0;
}