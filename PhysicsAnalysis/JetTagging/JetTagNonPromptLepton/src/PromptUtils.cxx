/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// C/C++
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>
#include <stdint.h>
#include <stdexcept>

// Local
#include "PromptUtils.h"

using namespace std;

//=============================================================================
std::string Prompt::PrintResetStopWatch(TStopwatch &watch)
{
  watch.Stop();
  
  double realt = watch.RealTime();
  double cput  = watch.CpuTime();
  
  watch.Reset();
  watch.Start();
  
  const int hours = static_cast<int>(realt/3600.0);
  const int  min  = static_cast<int>(realt/60.0) - 60*hours;
  
  realt -= hours * 3600;
  realt -= min * 60;
  
  if (realt < 0) realt = 0;
  if (cput  < 0) cput  = 0;
  
  const int sec = static_cast<int>(realt);
  
  std::stringstream str;
  str << "Real time " 
      << setw(2) << setfill('0') << hours 
      << ":" << setw(2) << setfill('0') << min
      << ":" << setw(2) << setfill('0') << sec
      << " CPU time " << setprecision(3) << fixed << cput;
  
  return str.str();
}
