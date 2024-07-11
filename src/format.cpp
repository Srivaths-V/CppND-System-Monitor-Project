#include <string>

#include "format.h"

using std::string;
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long  seconds) 
{ 
  long inHours = seconds / 3600;
  long remainderSeconds = seconds % 3600;
  long inMinutes = remainderSeconds / 60;
  long inSeconds = remainderSeconds % 60;
  
  string Hours = std::to_string(inHours);
  string Minutes = std::to_string(inMinutes);
  string Seconds = std::to_string(inSeconds);
  
  if(inHours < 10)
  {
    Hours =  "0"+ std::to_string(inHours);
  }
  if(inMinutes < 10)
  {
    Minutes =  "0"+ std::to_string(inMinutes);
  }
  if(inSeconds < 10)
  {
    Seconds =  "0"+ std::to_string(inSeconds);
  }
  return (Hours + ":" + Minutes + ":" + Seconds); 
}
