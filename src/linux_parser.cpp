#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{ 
  string line, keyword, number;
  float usedMemoryFraction, freeMemory, totalMemory;
  
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> keyword >> number) 
    {
      if (keyword == "MemTotal:")
      {
        totalMemory = std::stof(number);
      }
      else
      if (keyword == "MemFree:")
      {
         freeMemory = std::stof(number);
      }
    }
  }

  usedMemoryFraction = (totalMemory - freeMemory)/totalMemory;
  
  return usedMemoryFraction; 
}

// Read and return the system uptime
long LinuxParser::UpTime() 
{ 
  long upTime;
  string upTimeStr, idleTimeStr;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTimeStr >> idleTimeStr;
    upTime = std::stol(upTimeStr);
  }
  return upTime;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() 
{ 
   string line, jiffies;
   vector<string> cpuUtilization;
   std::ifstream stream(kProcDirectory + kStatFilename);
   if(stream.is_open())
   {
     std::getline(stream, line);
     std::istringstream linestream(line);
     linestream >> jiffies;
     while(linestream >> jiffies)
     {
      cpuUtilization.push_back(jiffies);
     }
   }
   return cpuUtilization; 
  
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() 
{ 
  long jiffiesTotal = 0;
  vector<string> jiffies = CpuUtilization();

  for (string taskBasedJiffy : jiffies)
  {
    jiffiesTotal = jiffiesTotal + std::stol(taskBasedJiffy);
  }
  
  return jiffiesTotal; 
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() 
{ 
  long activeJiffies = Jiffies() - IdleJiffies();
  
  return activeJiffies/sysconf(_SC_CLK_TCK); 
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() 
{ 
  vector<string> jiffies = CpuUtilization();
  
  long idleJiffies = std::stol(jiffies[CPUStates::kIdle_])+ std::stol(jiffies[CPUStates::kIOwait_]) + std::stol(jiffies[CPUStates::kSteal_]);
  
  return idleJiffies/sysconf(_SC_CLK_TCK); 
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{ 
   int totalProcesses;
   string keyword, number, line;
   std::ifstream stream(kProcDirectory+kStatFilename);
   if(stream.is_open())
   {
     while(std::getline(stream,line))
     {
        std::istringstream linestream(line);
        linestream >> keyword >> number;
        if(keyword == "processes")
        {
          totalProcesses = std::stoi(number);
          break;
        }
     }
   }
   return totalProcesses;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() 
{ 
  int runningProcesses;
  string keyword, number, line;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open())
  {
    while(std::getline(stream,line))
    {
      std::istringstream linestream(line);
      linestream >> keyword >> number;
      if(keyword == "procs_running")
      {
          runningProcesses = stoi(number);
          break;
      }
    }
  }

  return runningProcesses;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) 
{ 
  string jiffy, line;
  long activeJiffiesForPID, utime, stime, cutime, cstime;
  vector<string> timeValues;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> jiffy) 
    {
      timeValues.push_back(jiffy);
    }
  }
  utime = std::stol(timeValues[13]);
  stime = std::stol(timeValues[14]);
  cutime = std::stol(timeValues[15]);
  cstime = std::stol(timeValues[16]);
  activeJiffiesForPID = (utime + stime + cutime + cstime);
  
  return activeJiffiesForPID/ sysconf(_SC_CLK_TCK);
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) 
{ 
  string command, line;
  std::ifstream filestream(kProcDirectory+std::to_string(pid)+ kCmdlineFilename);
  if(filestream.is_open())
  {
    std::getline(filestream,line);
    std::istringstream linestream(line);
    linestream >> command;
  }
  return command; 
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) 
{ 
  string line, ram, keyword, number;
  std::ifstream filestream(kProcDirectory + std::to_string(pid)+ kStatusFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> keyword >> number) 
      {
        if (keyword == "VmSize:") {
          ram = std::to_string(std::stoi(number) / 1024);
        }
       }
     }
   }
  return ram; 
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) 
{ 
  string line;
  string userID, keyword, uID;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> keyword >> userID) 
      {
        if (keyword == "Uid:") 
        {
          uID = userID;
        }
      }
    }
  }
  return uID;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) 
{ 
  string processUser, password, userID, user, line;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) 
  {
    while (std::getline(stream, line)) 
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> processUser >> password >> userID) 
      {
        if (userID == LinuxParser::Uid(pid)) 
        {
          user = processUser;
        }
      }
    }
  }
  return user;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) 
{ 
  string number, line;
  long processUpTime;
  vector<string> timeValues;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) 
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> number) 
    {
      timeValues.push_back(number);
    }
  }
  processUpTime = LinuxParser::UpTime() - std::stol(timeValues[21]) / sysconf(_SC_CLK_TCK);
  return processUpTime;
}
