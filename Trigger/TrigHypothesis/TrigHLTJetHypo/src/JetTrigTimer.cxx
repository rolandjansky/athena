/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./JetTrigTimer.h"
#include <cassert>

#include <sstream>


using namespace std::chrono;

void JetTrigTimer::start(){

  assert(m_isRunning == false);
  m_start = system_clock::now();
  m_isRunning = true;
  m_nCalls += 1;
}

void JetTrigTimer::stop(){

  assert(m_isRunning == true);
  m_stop = system_clock::now();
  m_isRunning = false;
  m_delta += duration_cast<nanoseconds>(m_stop - m_start).count();
}


void JetTrigTimer::reset() noexcept {
  m_nCalls = 0;
  m_delta = 0.;
}
  
std::string JetTrigTimer::read(){    
 
  assert (m_isRunning == false);
  std::stringstream ss;
  double avTime  =  m_nCalls == 0 ? 0. : m_delta / m_nCalls;
  ss << "time(ns): " << m_delta << " nCalls: " << m_nCalls << " tav: "
     << avTime << '\n';
  return ss.str();
}
  
std::string JetTrigTimer::readAndReset(){    
  auto s = read();
  reset();
  return s;
}
