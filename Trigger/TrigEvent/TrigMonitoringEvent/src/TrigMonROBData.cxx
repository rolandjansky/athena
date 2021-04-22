/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <iostream>
#include <sstream>

#include "AthenaKernel/errorcheck.h"
#include "TrigMonitoringEvent/TrigMonROB.h"

namespace ROBbits
{
  const uint32_t maskHistory = 0x0000000f;
  const uint32_t maskStatus  = 0x000000f0;
  const uint32_t maskState   = 0x000000ff;
  const uint32_t maskSize    = 0xffffff00;
  const uint32_t shiftSize   = 8;
}


//--------------------------------------------------------------------------------------
TrigMonROBData::TrigMonROBData()
  :m_rob_id(0),
   m_word(0)
{
}

//--------------------------------------------------------------------------------------  
TrigMonROBData::TrigMonROBData(uint32_t rob_id, uint32_t rob_size)
  :m_rob_id(rob_id),
   m_word(0)
{
  if(rob_size >= 16777215) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonROBData")
      << "Size out of range";
  }
  else {
    m_word |= (rob_size << ROBbits::shiftSize);
  }
}

//--------------------------------------------------------------------------------------  
void TrigMonROBData::setHistory(History hist)
{
  //
  // Set history enum.
  //
  m_word  = m_word & (~ROBbits::maskHistory);
  m_word |= hist;
}

//--------------------------------------------------------------------------------------  
void TrigMonROBData::setStatusOK()
{
  //
  // Set status bit
  //
  m_word |= kOK;
}

//--------------------------------------------------------------------------------------  
void TrigMonROBData::setStatusPREFETCH()
{
  //
  // Set status bit
  //
  m_word |= kPREFETCH;
}

//--------------------------------------------------------------------------------------  
bool TrigMonROBData::isStatusOk() const
{
  //
  // Return status bit
  //
  return (m_word & kOK);
}

//--------------------------------------------------------------------------------------  
bool TrigMonROBData::isStatusPrefetched() const
{
  //
  // Return status bit
  //
  return (m_word & kPREFETCH);
}

//--------------------------------------------------------------------------------------  
uint32_t TrigMonROBData::getROBSize() const
{
  //
  // Get data size
  //
  return ((m_word & ROBbits::maskSize) >> ROBbits::shiftSize);
}

//--------------------------------------------------------------------------------------  
uint32_t TrigMonROBData::getEncodedState() const
{
  //
  // Return encoded Status and History integer
  //
  return (m_word & ROBbits::maskState);
}

//--------------------------------------------------------------------------------------  
TrigMonROBData::History TrigMonROBData::getHistory() const
{
  //
  // Mask history bits and get history
  //
  return Trig::getROBHistory(m_word);
}

//--------------------------------------------------------------------------------------  
TrigMonROBData::Status TrigMonROBData::getStatus() const
{
  //
  // Mask status bits and get status
  //
  return Trig::getROBStatus(m_word);
}

//--------------------------------------------------------------------------------------      
void TrigMonROBData::print(std::ostream &os) const
{
  os << str(*this) << "\n";
}

//--------------------------------------------------------------------------------------  
std::string str(const TrigMonROBData &o)
{
  std::stringstream str;  
  str << "TrigMonROBData: rob id=" << o.getROBId() 
      << " size=" << o.getROBSize(); 

  return str.str();
}

//--------------------------------------------------------------------------------------  
TrigMonROBData::History Trig::getROBHistory(unsigned int val)
{
  const uint32_t hist = val & ROBbits::maskHistory;

  if(hist == 1) return TrigMonROBData::kRETRIEVED;
  if(hist == 2) return TrigMonROBData::kCACHED;
  if(hist == 4) return TrigMonROBData::kIGNORED;
  if(hist == 8) return TrigMonROBData::kDISABLED;

  return TrigMonROBData::kUNCLASSIFIED;
}

//--------------------------------------------------------------------------------------  
TrigMonROBData::Status Trig::getROBStatus(unsigned int val)
{
  const uint32_t stat = val & ROBbits::maskStatus;

  if(stat & TrigMonROBData::kOK)       return TrigMonROBData::kOK;
  if(stat & TrigMonROBData::kPREFETCH) return TrigMonROBData::kPREFETCH;

  return TrigMonROBData::kUNKNOWN;
}

//--------------------------------------------------------------------------------------  
std::string Trig::AsStringROBHistory(TrigMonROBData::History hist)
{
  if(hist == TrigMonROBData::kUNCLASSIFIED) return "UNCLASSIFIED";
  if(hist == TrigMonROBData::kRETRIEVED)    return "RETRIEVED";
  if(hist == TrigMonROBData::kCACHED)       return "CACHED";
  if(hist == TrigMonROBData::kIGNORED)      return "IGNORED";
  if(hist == TrigMonROBData::kDISABLED)     return "DISABLED";
  
  return "UNCLASSIFIED";
}
