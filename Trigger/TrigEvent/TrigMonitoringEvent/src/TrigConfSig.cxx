/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <algorithm>
#include <sstream>

// Local
#include "TrigMonitoringEvent/TrigConfSig.h"

using namespace std;

//--------------------------------------------------------------------------------------  
TrigConfSig::TrigConfSig() 
  :m_counter(0),
   m_logic(0),
   m_label()
{
}

//--------------------------------------------------------------------------------------  
TrigConfSig::TrigConfSig(uint32_t counter, int logic, const std::string &label) 
  :m_counter(counter),
   m_logic(logic),
   m_label(label)
{
}

//--------------------------------------------------------------------------------------  
void TrigConfSig::clearStrings()
{
  //
  // Clear all string variables
  //
  m_label.clear();
}

//--------------------------------------------------------------------------------------      
bool TrigConfSig::matchOutputTE(uint32_t te_id) const
{
  return (std::find(m_output_te.begin(), m_output_te.end(), te_id) != m_output_te.end());
}

//--------------------------------------------------------------------------------------      
void TrigConfSig::print(std::ostream &os) const
{
  os << str(*this) << endl;
}

//--------------------------------------------------------------------------------------  
std::string str(const TrigConfSig &o)
{
  std::stringstream s;
  s << "TrigConfSig: " << o.getLabel() << " counter=" << o.getCounter();
  
  return s.str();  
}
