/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <iostream>
#include <sstream>

#include "AthenaKernel/errorcheck.h"
#include "TrigMonitoringEvent/TrigMonAlg.h"
#include "TrigMonitoringEvent/TrigMonRoi.h"

namespace AlgBits
{
  const uint32_t maskCache = 0x80; // bit used to set caching state
  const uint32_t maskPos   = 0x7f; // mask bottom 7 bits
}


//--------------------------------------------------------------------------------------  
TrigMonAlg::TrigMonAlg()
  :m_byte(1, 0)
{
}

//--------------------------------------------------------------------------------------  
TrigMonAlg::TrigMonAlg(unsigned int position, bool is_cached)
  :m_byte(1, 0)
{
  if(position < 128) {
    m_byte[0] = position;
  }
  else {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonAlg")
      << "ctor error! Position is out of allowed range.";
  }

  if(is_cached) {
    //
    // Set top bit for caching state
    //
    m_byte[0] |= AlgBits::maskCache;
  }
}

//--------------------------------------------------------------------------------------  
void TrigMonAlg::addTimer(const TrigMonTimer &tbeg, const TrigMonTimer &tend)
{
  //
  // Save start and stop time for one algorithm call
  //
  if(isCached()) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonAlg")
      << "addTimers error! Cached algorithm has no timers.";
    return;
  }
  else if(!m_word.empty()) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonAlg")
      << "addTimers error! Timers already added!";
    return;
  }

  m_word.push_back(tbeg.getEncoded());
  m_word.push_back(tend.getEncoded());
}

//--------------------------------------------------------------------------------------  
void TrigMonAlg::addRoiId(unsigned int roi_id)
{
  //
  // Add roid id, check that id is less than 256
  // 
  if(roi_id < 256) {
    m_byte.push_back(static_cast<uint8_t>(roi_id)); 
  }
  else if (roi_id == 256) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonAlg")
      << "addRoiId error! RoiId value is out of range! (only reported for ID=256)";
  }
}

//--------------------------------------------------------------------------------------  
void TrigMonAlg::addWord(unsigned int word)
{
  //
  // Save start and stop time for one algorithm call
  //
  if(m_word.size() != 2 && !isCached()) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonAlg")
      << "addWord error! Timers must be added first.";
    return;
  }

  m_word.push_back(word);
}

//--------------------------------------------------------------------------------------  
uint8_t TrigMonAlg::getPosition() const
{
  //
  // Return algorithm position with top bit set to zero
  //
  return (m_byte[0] & AlgBits::maskPos);
}

//--------------------------------------------------------------------------------------  
uint8_t TrigMonAlg::getNRoi() const
{
  //
  // Get number of RoIs
  //
  return m_byte.size()-1;
}

//--------------------------------------------------------------------------------------  
uint8_t TrigMonAlg::getRoiId(unsigned int i) const
{
  //
  // Return roi id
  //
  if(i+1 < m_byte.size()) return m_byte[i+1];

  return Trig::getRoiId_Unknown();
}

//--------------------------------------------------------------------------------------  
const std::set<uint8_t> TrigMonAlg::getRoiIdSet() const
{
  //
  // Return set of all roi ids
  //
  return std::set<uint8_t>(m_byte.begin()+1, m_byte.end());
}

//--------------------------------------------------------------------------------------  
bool TrigMonAlg::isCached() const
{
  //
  // check top bit for caching state
  //
  return (m_byte[0] & AlgBits::maskCache);
}

//--------------------------------------------------------------------------------------  
bool TrigMonAlg::isCalled() const
{
  //
  // check top bit for caching state
  //
  return !isCached();
}

//--------------------------------------------------------------------------------------  
const TrigMonTimer TrigMonAlg::start() const
{
  //
  // This function only make sense when timers exists, so first check isCalled()=false
  //
  if(!isCached() && m_word.size() >= 2) {
    return TrigMonTimer(m_word[0]);
  }
  
  return TrigMonTimer(0);
}

//--------------------------------------------------------------------------------------  
const TrigMonTimer TrigMonAlg::stop() const
{
  //
  // This function only make sense when timers exists, so first check isCalled()=true
  //
  if(!isCached() && m_word.size() >= 2) {
    return TrigMonTimer(m_word[1]);
  }
  
  return TrigMonTimer(0);
}

//--------------------------------------------------------------------------------------  
std::string str(const TrigMonAlg &o)
{
  std::stringstream s;  
  s << "TrigMonAlg: called=" << o.isCalled() 
    << " cached=" << o.isCached() 
    << " timer=" << o.elapsed()
    << " rois=";
  
  for(unsigned int i = 0; i < o.getNRoi(); ++i) {
    s << "   " << int(o.getRoiId(i)) << "\n";
  }
  
  return s.str();
}
