/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./JetTrigTimer.h"
#include <cassert>

#include <sstream>


using namespace std::chrono;

JetTrigTimer::JetTrigTimer(bool ns): m_nanoseconds(ns) {}
			     
void JetTrigTimer::start(){
  // reset start time to now
  
  m_nCalls += 1;
  m_start = system_clock::now();
}

void JetTrigTimer::update(){
  // reset stop time to now, calculate delta and sum delta.
  // does not change the start time
  
  m_stop = system_clock::now();
  if(m_nanoseconds){
     m_elapsedDelta += duration_cast<nanoseconds>(m_stop - m_start).count();
  } else {
     m_elapsedDelta += duration_cast<microseconds>(m_stop - m_start).count();
  }
}


void JetTrigTimer::reset() noexcept {
  // general reset
  
  m_nCalls = 0;
  m_elapsedDelta = 0.;
  m_nCalls = 0;
  m_start = std::chrono::system_clock::now();
  m_stop = m_start;
}
  
std::string JetTrigTimer::read() const{    
  // reuturn curent data as a string
  
  auto record = read_bare();
  auto delta = std::get<0>(record);
  auto ncalls = std::get<1>(record);
  auto units = std::get<2>(record);
  
  std::stringstream ss;
  double avTime  =  ncalls == 0 ? 0. : delta / m_nCalls;
  ss << "time("<<units <<"): " << delta << " nCalls: " << ncalls << " tav: "
     << avTime;
  return ss.str();
}

std::tuple<double, int, std::string> JetTrigTimer::read_bare() const{
  // reuturn curent data in numeric form

  return std::make_tuple(m_elapsedDelta, m_nCalls, units());
}

std::string JetTrigTimer::readAndReset(){
  // obtain current data as a string then reset the timer
  auto s = read();
  reset();
  return s;
}


double JetTrigTimer::elapsed_to_now() {
  update();
  m_start = system_clock::now();
  return m_elapsedDelta;
}

double JetTrigTimer::elapsed_to_update() {
  //  update the current data, return the time since start()
  return m_elapsedDelta;
}
 
  
std::string JetTrigTimer::units() const {
  return m_nanoseconds ? "ns" : "us";
}   

