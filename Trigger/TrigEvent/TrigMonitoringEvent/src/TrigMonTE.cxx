/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cassert>
#include <iostream>
#include <sstream>
#include <set>

#include "TrigMonitoringEvent/TrigMonTE.h"
#include "AthenaKernel/errorcheck.h"

namespace SeqBits
{
  const uint32_t maskLow16  = 0x0000ffff; // mask  2 type bits
  const uint32_t maskType   = 0x00c00000; // mask  2 type bits

  const uint32_t shiftType  = 22;         // shift 2 type bits
}

//--------------------------------------------------------------------------------------      
TrigMonTE::TrigMonTE()
  :m_id(0),
   m_encoded(0x0)
{
}

//--------------------------------------------------------------------------------------      
TrigMonTE::TrigMonTE(uint32_t id, uint16_t index)
  :m_id(id),
   m_encoded(index)
{
}

//--------------------------------------------------------------------------------------  
void TrigMonTE::addVar(const TrigMonVar &var)
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
void TrigMonTE::setType(Type type)
{
  //
  // Set trigger element type: use 2 bits
  //
  m_encoded |= (type << SeqBits::shiftType);
}

//--------------------------------------------------------------------------------------      
void TrigMonTE::addState(State state)
{
  //
  // Set state bits directly
  //
  m_encoded |= state;
}

//--------------------------------------------------------------------------------------      
TrigMonTE::Type TrigMonTE::getType() const
{
  //
  // Return trigger element type
  //
  const unsigned int val = (m_encoded & SeqBits::maskType) >> SeqBits::shiftType;

  switch (val) {
  case 0: return kELEM;
  case 1: return kINIT;
  case 2: return kROI;
  case 3: return kL1TH;
  default: break;
  }
  REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonTE")
    << "getType error! Bad value";
  return kELEM;
}

//--------------------------------------------------------------------------------------      
uint16_t TrigMonTE::getIndex() const
{
  //
  // Return index - mask low 16 bits
  //
  return (m_encoded & SeqBits::maskLow16);
}

//--------------------------------------------------------------------------------------  
const std::vector<TrigMonVar> TrigMonTE::getVar() const
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
void TrigMonTE::print(std::ostream &os)
{
  os << str(*this) << std::endl;
}

//--------------------------------------------------------------------------------------      
std::string str(const TrigMonTE &o)
{
  std::stringstream s;
  
  s << "TrigMonTE id=" << o.getId() << " status=";
  if     (o.getType() == TrigMonTE::kINIT) s << "init";
  else if(o.getType() == TrigMonTE::kROI)  s << "roi ";
  else if(o.getType() == TrigMonTE::kL1TH) s << "L1th";
  else if(o.getType() == TrigMonTE::kELEM) s << "elem";
  else                                     s << "bad ";
  s << " index=" << o.getIndex();
  
  s << std::endl << "   children " << o.getChildIndex().size() << ": ";
  for(unsigned int i = 0; i < o.getChildIndex().size(); ++i) s << o.getChildIndex()[i] << " ";
  s << std::endl;

  s << std::endl << "   parents " << o.getParentIndex().size() << ":  ";
  for(unsigned int i = 0; i < o.getParentIndex().size(); ++i) s << o.getParentIndex()[i] << " ";
  s << std::endl;

  s << std::endl << "   features " << o.getClid().size() << ": ";
  for(unsigned int i = 0; i < o.getClid().size(); ++i) s << o.getClid()[i] << " ";
  s << std::endl;

  return s.str();
}

//--------------------------------------------------------------------------------------      
bool Equal(const TrigMonTE &lhs, const TrigMonTE &rhs)
{
  //
  // Compare TriggerElements by id and roi parents
  //
  
  // 1st: compare types
  if(lhs.getType() != rhs.getType()) return false;
  
  // 2nd: initial node?
  if(lhs.getType() == TrigMonTE::kINIT) return true;

  // 3rd: RoI or ELEM node? 
  if(lhs.getId() != rhs.getId()) return false;
    
  const std::set<uint8_t> lhs_set(lhs.getRoiId().begin(), lhs.getRoiId().end());
  const std::set<uint8_t> rhs_set(rhs.getRoiId().begin(), rhs.getRoiId().end());

  return lhs_set == rhs_set;
}
