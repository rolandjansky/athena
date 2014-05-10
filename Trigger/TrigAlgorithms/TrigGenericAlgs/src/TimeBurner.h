/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGENERICALGS_TimeBurner_h
#define TRIGGENERICALGS_TimeBurner_h

/**
 * @brief  Algorithm to sleep for some time
 * @author Joerg Stelzer
 *
 * $Id: TimeBurner.h 346947 2011-02-21 16:58:50Z stelzer $
 */

#include "TrigInterfaces/AllTEAlgo.h"

#include <vector>

/**
 * Algorithm to sleep for some configurable time .
 */

class TimeBurner : public HLT::AllTEAlgo {
public:
   TimeBurner(const std::string& name, ISvcLocator* pSvcLocator);

   HLT::ErrorCode hltInitialize() { return HLT::OK; }
   HLT::ErrorCode hltExecute(std::vector<HLT::TEVec>&, unsigned int); 
   HLT::ErrorCode hltFinalize()   { return HLT::OK; }

private:
   // Properties
   int m_timeDelay; // time delay in ms (default 0ms)
};

#endif
