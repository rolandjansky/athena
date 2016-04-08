/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cassert>
#include <iostream>
#include <sstream>

// Local
#include "TrigMonitoringEvent/TrigMonTimer.h"
#include "TrigMonMSG.h"

using namespace std;

namespace MSGService
{
  static TrigMonMSG msg("TrigMonTimer");
}

namespace TimerBits
{
  const uint32_t maskSec  = 0xfff00000;
  const uint32_t maskUSec = 0x000fffff;
  
  const uint32_t shiftSec  = 20;
  const uint32_t shiftUSec = 0;

  const uint32_t moduloSec = 3600;
}

//--------------------------------------------------------------------------------------      
TrigMonTimer::TrigMonTimer()
  :m_encoded(0x0)
{
}

//--------------------------------------------------------------------------------------      
TrigMonTimer::TrigMonTimer(long int tv_sec,
			   long int tv_usec)
  :m_encoded(0x0)
{
  if(!(tv_sec < 0.0) && !(tv_usec < 0.0)) {
    const uint32_t sec  = static_cast<uint32_t>(tv_sec % TimerBits::moduloSec);
    const uint32_t usec = static_cast<uint32_t>(tv_usec);
    
    // Yes, these are redundant checks.
    if(sec  >= TimerBits::moduloSec) {
      std::stringstream ss;
      ss << "TrigMonTimer ctor error! sec="  <<  sec << ", " << tv_sec;
      MSGService::msg.Log(ss.str(), MSG::ERROR);
    }
    if(usec >= 1000000) {
      std::stringstream ss;
      ss << "TrigMonTimer ctor error! usec=" << usec << ", " << tv_usec;
      MSGService::msg.Log(ss.str(), MSG::ERROR);
    }

    m_encoded |= (sec << TimerBits::shiftSec);
    m_encoded |= usec;
  }
}

//--------------------------------------------------------------------------------------      
TrigMonTimer::TrigMonTimer(uint32_t encoded)
  :m_encoded(encoded)
{
}

//--------------------------------------------------------------------------------------      
uint32_t TrigMonTimer::getSec() const
{
  // return value in seconds
  return static_cast<uint32_t>((m_encoded & TimerBits::maskSec) >> TimerBits::shiftSec);
}

//--------------------------------------------------------------------------------------      
uint32_t TrigMonTimer::getMicroSec() const
{
  // return value in seconds
  return static_cast<uint32_t>((m_encoded & TimerBits::maskUSec) >> TimerBits::shiftUSec);
}

//--------------------------------------------------------------------------------------      
double TrigMonTimer::getElapsed(const TrigMonTimer &start_time) const
{
  // return elapsed time - assume time period is less than moduloSec seconds

  // get time difference in seconds
  int secs = getSec() - start_time.getSec();
  if(getSec() < start_time.getSec()) secs = TimerBits::moduloSec+getSec()-start_time.getSec();

  // get time difference in microseconds
  int usecs = getMicroSec() - start_time.getMicroSec();
  
  return static_cast<double>(secs)*1000.0 + static_cast<double>(usecs)/1000.0;
}
