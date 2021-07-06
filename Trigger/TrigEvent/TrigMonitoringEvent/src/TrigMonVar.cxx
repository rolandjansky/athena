/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"
#include "TrigMonitoringEvent/TrigMonVar.h"

//--------------------------------------------------------------------------------------
TrigMonVar::TrigMonVar()
  :m_key(0),
   m_data(0.)
{
}

//--------------------------------------------------------------------------------------      
TrigMonVar::TrigMonVar(const unsigned int key, const float data)
  :m_key(key),
   m_data(data)
{
  if(key >= 65535) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonVar")
      << "ctor - error! Overflow of 16 bits key.";
  }
}

//--------------------------------------------------------------------------------------      
void TrigMonVar::print(std::ostream &os) const
{
  os << "TrigMonVar::Print" << std::endl;
}
