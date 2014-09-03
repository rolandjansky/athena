/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/POSIXTimeKeeper.h"

POSIXTimeKeeper::POSIXTimeKeeper() : TimeKeeper() { updateTime(); }
POSIXTimeKeeper::~POSIXTimeKeeper() {}

void POSIXTimeKeeper::updateTime() {
  times(&m_tms);
}

time_t POSIXTimeKeeper::timeX() const {
  return m_tms.tms_utime + m_tms.tms_cutime + m_tms.tms_stime + m_tms.tms_cstime;
} 

const std::string&  POSIXTimeKeeper::unitLabel() const {
  static const std::string label("1/100 CPU secs");
  return label;
}

