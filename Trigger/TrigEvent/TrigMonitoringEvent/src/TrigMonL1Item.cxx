/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <sstream>

// Local
#include "TrigMonitoringEvent/TrigMonL1Item.h"
#include "TrigMonMSG.h"

namespace L1Bits
{
  const uint32_t maskCtpId = 0x0fff;  // mask ctpid bits
  //const uint32_t maskDecis = 0x7000;  // mask decision bits
}

namespace MSGService
{
  static TrigMonMSG msg("TrigMonL1Item");
}

using namespace std;

//--------------------------------------------------------------------------------------      
TrigMonL1Item::TrigMonL1Item()
  :m_encoded(0x0)
{
}

//--------------------------------------------------------------------------------------      
TrigMonL1Item::TrigMonL1Item(uint16_t encoded)
  :m_encoded(encoded)
{
}

//--------------------------------------------------------------------------------------      
void TrigMonL1Item::setCtpId(unsigned int ctpid)
{
  //
  // Set ctp id
  //
  if(ctpid < 4096) {
    m_encoded  = m_encoded & (~L1Bits::maskCtpId);
    m_encoded |= ctpid;
  }
  else {
    MSGService::msg.Log("TrigMonL1Item::setCtpId error! Bad ctp id", MSG::ERROR);
  }
}

//--------------------------------------------------------------------------------------      
void TrigMonL1Item::addDecision(Decision value)
{
  //
  // Set trigger decision bits
  //
  if     (value == kReset)            { m_encoded  = getCtpId(); }
  else if(value == kL1AfterVeto)      { m_encoded |= value; }
  else if(value == kL1BeforePrescale) { m_encoded |= value; }
  else if(value == kL1AfterPrescale)  { m_encoded |= value; }
}

//--------------------------------------------------------------------------------------      
uint16_t TrigMonL1Item::getCtpId() const
{
  //
  // Mask decision bits and return ctp id
  //
  return m_encoded & L1Bits::maskCtpId;
}

//--------------------------------------------------------------------------------------      
bool TrigMonL1Item::isPassed(Decision value) const
{
  //
  // Check trigger decision value
  //
  if     (value == kPassed)    return isPassed();
  else if(value == kPrescaled) return isPrescaled();

  return m_encoded & value;
}

//--------------------------------------------------------------------------------------      
void TrigMonL1Item::print(std::ostream &os) const
{
  os << str(*this) << endl;
}

//--------------------------------------------------------------------------------------  
std::string str(const TrigMonL1Item &o)
{
  std::stringstream s;  
  s << "TrigMonL1Item: ctp id=" << o.getCtpId() 
    << " passed=" << o.isPassed(TrigMonL1Item::kPassed); 
  
  return s.str();
}
