/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <algorithm>
#include <sstream>

#include "AthenaKernel/errorcheck.h"
#include "TrigMonitoringEvent/TrigConfSeq.h"


//--------------------------------------------------------------------------------------  
TrigConfSeq::TrigConfSeq()
  :m_output_te_name(),
   m_output_te_index(0),
   m_output_te_id(0),
   m_topo_te(0)
{
}

//--------------------------------------------------------------------------------------  
TrigConfSeq::TrigConfSeq(unsigned int id,
		       unsigned int index,
		       const std::string &name)
  :m_output_te_name(name),
   m_output_te_index(index),
   m_output_te_id(id),
   m_topo_te(0)
{
}

//--------------------------------------------------------------------------------------  
void TrigConfSeq::clearStrings()
{
  //
  // Clear all string variables
  //
  m_output_te_name.clear();
  for(unsigned int i = 0; i < m_alg.size(); ++i) m_alg[i].clearStrings();
}

//--------------------------------------------------------------------------------------  
const TrigConfAlg& TrigConfSeq::getAlg(unsigned int pos) const
{
  if(pos >= m_alg.size()) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigConfSeq")
      << "getAlg(" << pos << ") error! Index is greater than alg size=" << m_alg.size() << " for " << m_output_te_name;
    return m_alg.front();
  }
  if(m_alg[pos].getPosition() != pos) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigConfSeq")
      << "getAlg(" << pos << ") error! Index mismatch: " << m_alg[pos].getPosition() << "!=" << pos;
  }

  return m_alg[pos];
}

//--------------------------------------------------------------------------------------  
std::vector<TrigConfAlg>::const_iterator TrigConfSeq::findName(const std::string &aname) const
{
  // find first algorithm by name
  std::vector<TrigConfAlg>::const_iterator it = m_alg.begin();

  for(; it != m_alg.end(); ++it) {
    if(it->getName() == aname) break;
  }

  return it;
}

//--------------------------------------------------------------------------------------  
std::vector<TrigConfAlg>::const_iterator TrigConfSeq::findType(const std::string &atype) const
{
  // find first algorithm by type
  std::vector<TrigConfAlg>::const_iterator it = m_alg.begin();

  for(; it != m_alg.end(); ++it) {
    if(it->getType() == atype) break;
  }

  return it;
}

//--------------------------------------------------------------------------------------  
bool TrigConfSeq::matchAlgName(const std::string &aname) const
{
  //
  // Match algorithm name to a list of my configured algorithms
  //
  return (TrigConfSeq::findName(aname) != m_alg.end());
}

//--------------------------------------------------------------------------------------  
bool TrigConfSeq::matchAlgType(const std::string &atype) const
{
  //
  // Match algorithm type to a list of my configured algorithms
  //
  return (TrigConfSeq::findType(atype) != m_alg.end());
}

//--------------------------------------------------------------------------------------  
void TrigConfSeq::print(std::ostream &os) const
{
  os << str(*this) << std::endl;
}

//--------------------------------------------------------------------------------------  
std::string str(const TrigConfSeq &o)
{
  std::stringstream s;  
  s << "TrigConfSeq: " << o.getName() << " id=" << o.getId() 
    << " contains " << o.getAlg().size() << " algorithm(s): " << std::endl;
  
  for(unsigned int i = 0; i < o.getAlg().size(); ++i) {
    s << "   " << str(o.getAlg()[i]) << std::endl;
  }

  return s.str();
}
