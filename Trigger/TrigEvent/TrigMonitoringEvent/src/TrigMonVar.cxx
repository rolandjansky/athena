/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $

// Local
#include "TrigMonitoringEvent/TrigMonVar.h"

using namespace std;

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
    cerr << "TrigMonVar ctor - error! Overflow of 16 bits key." << endl;
  }
}

//--------------------------------------------------------------------------------------      
void TrigMonVar::print(std::ostream &os) const
{
  os << "TrigMonVar::Print" << std::endl;
}
