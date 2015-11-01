/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigT2ZdcSignals.h"
#include <iostream>

TrigT2ZdcSignals::TrigT2ZdcSignals(): m_triggerEnergies(TrigT2ZdcSignals::NUM_ZDC,0.), 
				      m_triggerTimes(TrigT2ZdcSignals::NUM_ZDC,0) {
}

TrigT2ZdcSignals::TrigT2ZdcSignals(const std::vector<float>& triggerEnergies, 
				   const std::vector<float>& triggerTimes)
  : m_triggerEnergies(triggerEnergies),
    m_triggerTimes(triggerTimes)
{
  m_triggerEnergies.resize(TrigT2ZdcSignals::NUM_ZDC,0.);
  m_triggerTimes.resize(TrigT2ZdcSignals::NUM_ZDC,0);
}

TrigT2ZdcSignals::TrigT2ZdcSignals(std::vector<float>&& triggerEnergies, 
				   std::vector<float>&& triggerTimes)
  : m_triggerEnergies(std::move(triggerEnergies)),
    m_triggerTimes(std::move(triggerTimes))
{
  m_triggerEnergies.resize(TrigT2ZdcSignals::NUM_ZDC,0.);
  m_triggerTimes.resize(TrigT2ZdcSignals::NUM_ZDC,0);
}

TrigT2ZdcSignals::~TrigT2ZdcSignals() {
}

// Simple debug method using MsgStream
void TrigT2ZdcSignals::print(void) const {
  std::vector<float>::const_iterator itr;
  std::vector<float>::const_iterator itr_end;

  itr=m_triggerEnergies.begin();
  itr_end=m_triggerEnergies.end();
  std::cout << "m_triggerEnergies = {";
  for(;itr != itr_end; ++itr) {
    std::cout << (*itr) << ",";
  }
  std::cout << "}" << std::endl;

  itr = m_triggerTimes.begin();
  itr_end = m_triggerTimes.end();
  std::cout << "m_triggerTimes = {";
  for(;itr != itr_end; ++itr) {
    std::cout << (*itr) << ",";
  }
  std::cout << "}" << std::endl;
}

// Simple debug method using MsgStream
void TrigT2ZdcSignals::print(MsgStream& log) const {
  std::vector<float>::const_iterator itr;
  std::vector<float>::const_iterator itr_end;

  itr=m_triggerEnergies.begin();
  itr_end=m_triggerEnergies.end();
  log << MSG::DEBUG << "m_triggerEnergies = {";
  for(;itr != itr_end; ++itr) {
    log << MSG::DEBUG << (*itr) << ",";
  }
  log << MSG::DEBUG << "}" << endreq;  

  itr = m_triggerTimes.begin();
  itr_end = m_triggerTimes.end();
  log << MSG::DEBUG << "m_triggerTimes = {";
  for(;itr != itr_end; ++itr) {
    log << MSG::DEBUG << (*itr) << ",";
  }
  log << MSG::DEBUG << "}" << endreq;
}

// Simple string conversion method.
std::string str(const TrigT2ZdcSignals& trigT2ZdcSignals) {
  std::stringstream sstream;
  std::vector<float> counterValues;
  std::vector<float>::const_iterator itr;
  std::vector<float>::const_iterator itr_end;

  counterValues = trigT2ZdcSignals.triggerEnergies();
  itr = counterValues.begin();
  itr_end = counterValues.end();
  sstream << "m_triggerEnergies = {";
  for(;itr != itr_end; ++itr) {
    sstream << (*itr) << ",";
  }
  sstream << "}" << std::endl;

  counterValues = trigT2ZdcSignals.triggerTimes();
  itr = counterValues.begin();
  itr_end = counterValues.end();
  sstream << "m_triggerTimes = {";
  for(;itr != itr_end; ++itr) {
    sstream << (*itr) << ", ";
  }
  sstream << "}";
  
  return sstream.str();
}

MsgStream& operator<< (MsgStream& m, const TrigT2ZdcSignals& trigT2ZdcSignals) {
  return (m << str(trigT2ZdcSignals));
}
