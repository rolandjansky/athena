/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <sstream>

// Local
#include "TrigMonitoringEvent/TrigMonChain.h"

namespace ChainBits 
{
  const uint32_t maskLow16 = 0x0000ffff;
  //const uint32_t maskDecis = 0x01ff0000;
}

//--------------------------------------------------------------------------------------
TrigMonChain::TrigMonChain(unsigned int encoded)
  :m_encoded(encoded)
{
}

//--------------------------------------------------------------------------------------      
TrigMonChain::TrigMonChain(unsigned int level, unsigned int counter)
  :m_encoded(Trig::getEncodedId(level, counter))
{
}

//--------------------------------------------------------------------------------------      
TrigMonChain::TrigMonChain(const std::string &level, unsigned int counter)
  :m_encoded(Trig::getEncodedId(level, counter))
{
}

//--------------------------------------------------------------------------------------      
uint16_t TrigMonChain::getCounter() const
{
  return Trig::getCounterFromEncodedId(getEncodedId());
}

//--------------------------------------------------------------------------------------      
uint16_t TrigMonChain::getLevel() const
{
  return Trig::getLevelFromEncodedId(getEncodedId());
}

//--------------------------------------------------------------------------------------      
void TrigMonChain::addDecision(Decision value)
{
  //
  // set trigger decision bits
  //
  if(value == kReset) {
    m_encoded = (m_encoded & ChainBits::maskLow16);
  }
  else {
    m_encoded |= value;
  }
}

//--------------------------------------------------------------------------------------  
void TrigMonChain::addVar(const TrigMonVar &var)
{
  //
  // Store variable as int and float, reserve 0-9 keys
  //
  if(var.getKey() > 9) {
    m_var_key.push_back(var.getKey());
    m_var_val.push_back(var.getData());
  }
}

//--------------------------------------------------------------------------------------  
uint16_t TrigMonChain::getEncodedId() const
{
  //
  // Return low 16 bits
  //
  return m_encoded & ChainBits::maskLow16;
}

//--------------------------------------------------------------------------------------  
void TrigMonChain::addTimer(float timer)
{
  //
  // Add sequence timer at key = 0
  //
  m_var_key.push_back(0); 
  m_var_val.push_back(timer); 
}

//--------------------------------------------------------------------------------------  
float TrigMonChain::getTimer() const
{
  //
  // Get sequence timer which is stored at key=0
  //

  if(m_var_key.size() == m_var_val.size()) {
    //
    // Look for key == 0
    //
    for(unsigned int i = 0; i < m_var_key.size(); ++i) {
      if(m_var_key[i] == 0) return m_var_val[i];
    }
  }

  return 0.0;
}

//--------------------------------------------------------------------------------------      
bool TrigMonChain::isPassed(Decision value) const
{
  //
  // Check trigger decision value
  //
  return m_encoded & value;
}

//--------------------------------------------------------------------------------------  
const std::vector<TrigMonVar> TrigMonChain::getVar() const
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

//--------------------------------------------------------------------------------------      
void TrigMonChain::print(std::ostream &os) const
{
  os << str(*this) << "\n";
}

//--------------------------------------------------------------------------------------  
std::string str(const TrigMonChain &o)
{
  std::stringstream s;  
  s << "TrigMonChain: level=" << o.getLevel() 
    << " passed=" << o.isPassed(TrigMonChain::kPassed); 
  
  return s.str();
}
