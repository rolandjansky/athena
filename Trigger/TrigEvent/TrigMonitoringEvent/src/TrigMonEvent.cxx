/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <algorithm>
#include <cmath>

// Local
#include "TrigMonitoringEvent/TrigMonEvent.h"

namespace EventBits
{
  const uint32_t maskLumi = 0x0000ffff;
  const uint32_t maskBxid = 0xffff0000;
  const uint32_t shftBxid = 16;

  static unsigned TrigMonEventVersion = 2;
}

using namespace std;

//--------------------------------------------------------------------------------------  
// Helper function for decoding/encoding event content
//
void setTrigMonEventVersion(unsigned version)
{
  EventBits::TrigMonEventVersion = version;
}

unsigned getTrigMonEventVersion() {
  return EventBits::TrigMonEventVersion;
}

//--------------------------------------------------------------------------------------  
TrigMonEvent::TrigMonEvent()
  :m_event(0),
   m_lumi(0),
   m_bxid(0),
   m_run(0),
   m_sec(0),
   m_nsec(0)
{
}

//--------------------------------------------------------------------------------------  
void TrigMonEvent::clear()
{
  //
  // Erase all event data
  //
  m_event = 0;
  m_lumi  = 0;
  m_bxid  = 0;
  m_run   = 0;
  m_sec   = 0;
  m_nsec  = 0;

  m_word.clear();
  m_rob.clear();
  m_roi.clear();
  m_seq.clear();
  m_te.clear();
  m_l1.clear();
  m_hlt.clear();
  m_var_key.clear();
  m_var_val.clear();
}

//--------------------------------------------------------------------------------------  
void TrigMonEvent::setEventID(uint32_t event,
			      uint32_t lumi,
			      uint32_t bxid,
			      uint32_t run,
			      uint32_t sec,
			      uint32_t nsec)
{
  //
  // Fill EventID data
  //
  m_event = event;
  m_run   = run;
  m_sec   = sec;
  m_nsec  = nsec;
  
  //
  // Pack luminosity block and bunch crossing id into m_lumi variable
  //
  m_lumi   = 0;
  m_lumi  |= lumi  & EventBits::maskLumi;
  m_lumi  |= bxid << EventBits::shftBxid;

  /*
    cout << "TrigMonEvent::setEventID -"
         << " lumi=" << lumi << "=" << getLumi()
         << " bxid=" << bxid << "=" << getBunchId() << endl;
  */
}

//--------------------------------------------------------------------------------------  
void TrigMonEvent::setTimer(float timer)
{
  //
  // Convert timer to int with 0.1ms precision and 4.0e5s maximum: stored in m_bxid variable
  //
  timer *= 10.0;

  if(timer > 4.0e9) timer = 4.0e9;
    
  const float timerI = std::floor(timer);

  if(timer - timerI > 0.499) { 
    timer = timerI + 1;
  }
  else {
    timer = timerI;
  }

  m_bxid = static_cast<uint32_t>(timer);
}

//--------------------------------------------------------------------------------------  
float TrigMonEvent::getTimer() const
{
  //
  // Read timer stored as m_bxid variable
  //
  return static_cast<float>(m_bxid)/10.0;
}

//--------------------------------------------------------------------------------------  
uint32_t TrigMonEvent::getLumi() const
{
  return (m_lumi & EventBits::maskLumi);
}

//--------------------------------------------------------------------------------------  
uint32_t TrigMonEvent::getBunchId() const
{
  return (m_lumi & EventBits::maskBxid) >> EventBits::shftBxid;
}

//--------------------------------------------------------------------------------------  
void TrigMonEvent::addVar(uint32_t key, float var)
{
  //
  // Add new variable
  //
  m_var_key.push_back(key);
  m_var_val.push_back(var);
}

//--------------------------------------------------------------------------------------  
bool TrigMonEvent::getVar(uint32_t key, float &var) const
{
  //
  // Get float variable
  //
  if(m_var_val.size() != m_var_key.size()) return false; 

  for(unsigned int i = 0; i < m_var_key.size(); ++i) {
    if(key == m_var_key[i]) {
      var = m_var_val[i];
      return true;
    }
  }

  return false;
}

//--------------------------------------------------------------------------------------  
bool TrigMonEvent::getKey(uint32_t &key, float var) const
{
  //
  // Get int variable
  //
  if(m_var_val.size() != m_var_key.size()) return false; 

  for(unsigned int i = 0; i < m_var_val.size(); ++i) {
    if(var == m_var_val[i]) {
      key = m_var_key[i];
      return true;
    }
  }

  return false;
}

//--------------------------------------------------------------------------------------  
const std::vector<TrigMonL1Item> TrigMonEvent::getL1Items() const
{
  //
  // Build variables on a fly and return vector by value
  //
  std::vector<TrigMonL1Item> l1vec;

  //
  // Iterate over encoded items
  //
  l1vec.reserve(m_l1.size());

  for(unsigned int i = 0; i < m_l1.size(); ++i) {
    l1vec.push_back(TrigMonL1Item(m_l1[i]));
  }

  return l1vec;
}

//--------------------------------------------------------------------------------------  
const std::vector<TrigMonChain> TrigMonEvent::getChains() const
{
  //
  // Build variables on a fly and return vector by value
  //
  std::vector<TrigMonChain> chvec;

  //
  // Iterate over chains
  //
  chvec.reserve(m_hlt.size());

  for(unsigned int i = 0; i < m_hlt.size(); ++i) {
    chvec.push_back(TrigMonChain(m_hlt[i]));
  }

  return chvec;
}

//--------------------------------------------------------------------------------------  
const std::vector<TrigMonVar> TrigMonEvent::getVar() const
{
  //
  // Build variables on a fly and return vector by value
  //
  std::vector<TrigMonVar> var;

  if(m_var_key.size() == m_var_val.size()) {
    //
    // Iterate over keys abd values
    //
    var.reserve(m_var_key.size());

    for(unsigned int i = 0; i < m_var_key.size(); ++i) {
      var.push_back(TrigMonVar(m_var_key[i], m_var_val[i]));
    }
  }

  return var;
}
