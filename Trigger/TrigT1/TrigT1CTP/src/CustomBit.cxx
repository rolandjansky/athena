/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// This header file
#include "TrigT1CTP/CustomBit.h"

// CTP DAQ output:
#include "TrigT1Result/CTPResult.h"

// Local includes:
#include "TrigT1CTP/CTPSimulation.h"

unsigned int LVL1CTP::CustomBit::size() const
{ 
  // sanity check 
  if (m_algo == 0) return 0;

  // get pointer to result builder and do sanity check
  m_resultBuilder = m_algo->resultBuilder();
  if (m_resultBuilder == 0) return 0;

  // cache result
  m_result = ROIB::convertToBitset((m_resultBuilder->*m_func)());

  return m_result.size();
}
