#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"

class Processor {
 public:
  float Utilization();  
  Processor();
  
 private:
  long previous_IdleJiffies_{};
  long previous_ActiveJiffies_{};
  long previous_TotalJiffies_{};
  long current_IdleJiffies_{}; 
  long current_ActiveJiffies_{}; 
  long current_TotalJiffies_{};
  long deltaIdle{};
  long deltaTotal{};
  
};

#endif