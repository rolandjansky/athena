/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief  Algorithm to perform predefined ROB requests
 * @author Frank Winklmeier
 *
 * $Id: TimeBurner.cxx 347029 2011-02-22 10:06:04Z stelzer $
 */

#include "TimeBurner.h"

TimeBurner::TimeBurner(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_timeDelay(0)
{
  declareProperty("TimeDelay", m_timeDelay, "Time delay in ms");  
}

HLT::ErrorCode TimeBurner::hltExecute(std::vector<HLT::TEVec>&, unsigned int) {
   usleep(1000*m_timeDelay); // usleep argument is in microseconds
   return HLT::OK;
}
