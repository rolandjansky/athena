/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTimeAlgs/TrigTimeStamp.h"

double TrigTimeStamp::millisecondsSince() const { 
  return std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now() - m_stamp ).count();
}
