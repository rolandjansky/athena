/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/TimeKeeper.h"

#include "GaudiKernel/MsgStream.h"

const unsigned int TimeKeeper::SKIPCYCLES(2);

TimeKeeper::TimeKeeper() : m_cycles(0), m_setupTime(0) {}

TimeKeeper::~TimeKeeper() {}

bool TimeKeeper::nextIter() {
  if (m_cycles<SKIPCYCLES) m_setupTime = timeX(); 
  ++m_cycles;
  updateTime();
  return !timeOver();
}

time_t TimeKeeper::timeL() const {
  return allocTime() - timeX();
} 

bool TimeKeeper::timeOver() const {
  return timeL() < 2 * averageIterTime(); //FIXME use e.g. + 3 sigmas
} 

time_t TimeKeeper::averageIterTime() const {
  //subtract setup time from estimate
  return (m_cycles<SKIPCYCLES) ? 0 : (timeX() - m_setupTime)  / (m_cycles - SKIPCYCLES + 1);
} 

std::ostream& operator << (std::ostream& ost, const TimeKeeper& tk) {
  ost << "TimeKeeper status: (all times in " << tk.unitLabel() << ")" 
      << "\n    allocated job time " << tk.allocTime() 
      << " - job time used so far " << tk.timeX() 
      << "\n    average time per iteration " << tk.averageIterTime() 
      << " - time left " << tk.timeL() ;
  if (tk.timeOver()) ost 
      << "\n    Not enough time for another iteration";
  return ost;
}

MsgStream& operator << (MsgStream& ost, const TimeKeeper& tk) {
  ost << "TimeKeeper status: (all times in " << tk.unitLabel() << ")" 
      << "\n    allocated job time " << tk.allocTime() 
      << " - job time used so far " << tk.timeX()  
      << "\n    average time per iteration " << tk.averageIterTime() 
      << " - time left " << tk.timeL() ;
  if (tk.timeOver()) ost 
      << "\n    Not enough time for another iteration";
  return ost;
}
