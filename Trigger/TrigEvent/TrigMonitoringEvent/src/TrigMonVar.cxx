/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $

// Local
#include "TrigMonitoringEvent/TrigMonVar.h"
#include "TrigMonMSG.h"

using namespace std;
namespace MSGService
{
  static TrigMonMSG msg("TrigMonVar");
}

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
    MSGService::msg.Log("TrigMonVar ctor - error! Overflow of 16 bits key.", MSG::ERROR);
  }
}

//--------------------------------------------------------------------------------------      
void TrigMonVar::print(std::ostream &os) const
{
  os << "TrigMonVar::Print" << std::endl;
}
