#include "processor.h"


Processor::Processor()
{
  previous_IdleJiffies_ = LinuxParser::IdleJiffies();
  previous_ActiveJiffies_ = LinuxParser::ActiveJiffies();
  previous_TotalJiffies_ = LinuxParser::Jiffies();
}

// Return the aggregate CPU utilization

float Processor::Utilization() 
{ 
	current_IdleJiffies_ = LinuxParser::IdleJiffies(); 
	current_ActiveJiffies_ = LinuxParser::ActiveJiffies(); 
	current_TotalJiffies_ = LinuxParser::Jiffies();

  deltaIdle = current_IdleJiffies_ - previous_IdleJiffies_;
  deltaTotal = current_TotalJiffies_ - previous_TotalJiffies_;

  previous_IdleJiffies_ = current_IdleJiffies_;
  previous_ActiveJiffies_ = current_ActiveJiffies_;
  previous_TotalJiffies_ = current_TotalJiffies_;
  
  if(deltaTotal == 0)
  {
    return 0.0f;
  }
  else
  {
    return (static_cast<float>(deltaTotal - deltaIdle)/static_cast<float>(deltaTotal));
  }
}