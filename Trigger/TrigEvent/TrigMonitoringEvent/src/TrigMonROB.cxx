/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <iostream>
#include <sstream>
#include <map>

// Local
#include "TrigMonitoringEvent/TrigMonROB.h"

using namespace std;

//--------------------------------------------------------------------------------------  
TrigMonROB::TrigMonROB()
  :m_word(3, 0)
{
}

//--------------------------------------------------------------------------------------  
TrigMonROB::TrigMonROB(uint32_t requestor_id,
		       long int start_sec,
		       long int start_usec,
		       long int stop_sec, 
		       long int stop_usec)
{
  m_word.push_back(requestor_id);
  m_word.push_back(TrigMonTimer(start_sec, start_usec).getEncoded());
  m_word.push_back(TrigMonTimer(stop_sec,  stop_usec).getEncoded());
}

//--------------------------------------------------------------------------------------      
const std::vector<TrigMonROBSum> TrigMonROB::getSum() const
{
  //
  // Converted encoded words to TrigMonROBSum
  //
  std::vector<TrigMonROBSum> svec;

  for(unsigned int i = 3; i < m_word.size(); ++i) {
    svec.push_back(TrigMonROBSum(m_word[i]));
  }

  return svec;
}

//--------------------------------------------------------------------------------------      
void TrigMonROB::print(std::ostream &os) const
{
  os << str(*this) << "\n";
}

//--------------------------------------------------------------------------------------  
std::string str(const TrigMonROB &o)
{
  std::stringstream s;  
  s << "TrigMonROB: requester id=" << o.getRequestorId() 
    << " timer=" << o.getTimer(); 

  return s.str();
}
