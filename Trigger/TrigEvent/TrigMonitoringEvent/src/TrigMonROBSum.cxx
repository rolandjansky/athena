/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <iostream>
#include <sstream>

// Local
#include "TrigMonitoringEvent/TrigMonROBSum.h"
#include "TrigMonMSG.h"

namespace SumBits
{
  uint32_t maskHist  = 0x0000000f;
  uint32_t maskNrob  = 0x00000ff0;
  uint32_t maskSize  = 0xfffff000;

  uint32_t shiftNrob =  4;
  uint32_t shiftSize = 12;
}

namespace MSGService
{
  static TrigMonMSG msg("TrigMonROBSum");
}

//--------------------------------------------------------------------------------------  
TrigMonROBSum::TrigMonROBSum()
  :m_word(0)
{
}

//--------------------------------------------------------------------------------------  
TrigMonROBSum::TrigMonROBSum(uint32_t word)
  :m_word(word)
{
}

//--------------------------------------------------------------------------------------  
bool TrigMonROBSum::setHistory(uint32_t hist)
{
  //
  // Set encoded state
  //
  if(hist < 16) {
    m_word  = m_word & (~SumBits::maskHist);
    m_word |= hist;
    return true;
  }
  MSGService::msg.Log("TrigMonROBSum::setHistory error! Bad int", MSG::ERROR);
  return false;
}

//--------------------------------------------------------------------------------------  
bool TrigMonROBSum::setNROB(uint32_t nrob)
{
  //
  // Set number of robs
  //
  if(nrob < 256) {
    m_word  = m_word & (~SumBits::maskNrob);
    m_word |= (nrob << SumBits::shiftNrob);
    return true;
  }
  MSGService::msg.Log("TrigMonROBSum::setNROB error! Bad int", MSG::ERROR);
  return false;
}

//--------------------------------------------------------------------------------------  
bool TrigMonROBSum::setSize(uint32_t size)
{
  //
  // Set number of robs
  //
  if(size < 1048576) {
    m_word  = m_word & (~SumBits::maskSize);
    m_word |= (size << SumBits::shiftSize);
    return true;
  }
  MSGService::msg.Log("TrigMonROBSum::setSize error! Bad int", MSG::ERROR);
  return false;
}

//--------------------------------------------------------------------------------------  
bool TrigMonROBSum::addROB(const TrigMonROBData &data, int subdet)
{
  //
  // Add rob data
  //
  
  if(getNROB() >     254) return false;
  if(getSize() > 1048574) return false;

  setHistory(data.getHistory());
  setSize(getSize()+data.getROBSize());

  if(subdet < 0) {
    setNROB(getNROB()+1);
  }
  else {
    setNROB(subdet);
  }
  
  return true;
}

//--------------------------------------------------------------------------------------  
uint32_t TrigMonROBSum::getSize() const
{ 
  //
  // Return History enum
  //
  return ((m_word & SumBits::maskSize) >> SumBits::shiftSize);
}

//--------------------------------------------------------------------------------------  
uint32_t TrigMonROBSum::getNROB() const
{ 
  //
  // Return History enum
  //
  return ((m_word & SumBits::maskNrob) >> SumBits::shiftNrob);
}

//--------------------------------------------------------------------------------------  
uint32_t TrigMonROBSum::getSubDet() const
{ 
  //
  // Return History enum
  //
  return ((m_word & SumBits::maskNrob) >> SumBits::shiftNrob);
}

//--------------------------------------------------------------------------------------  
TrigMonROBData::History TrigMonROBSum::getHistory() const
{ 
  //
  // Return History enum
  //
  return Trig::getROBHistory(m_word & SumBits::maskHist);
}

//--------------------------------------------------------------------------------------      
void TrigMonROBSum::print(std::ostream &os) const
{
  os << str(*this) << "\n";
}

//--------------------------------------------------------------------------------------  
std::string str(const TrigMonROBSum &o)
{
  std::stringstream s;  
  s << "TrigMonROBSum: " 
    << Trig::AsStringROBHistory(o.getHistory()) 
    << " nrob=" << o.getNROB() 
    << " size=" << o.getSize();

  return s.str();
}
