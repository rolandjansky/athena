/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : TrigDecisionTool
 *
 * @brief main tool
 *
 * @author Michael Begel  <michael.begel@cern.ch> - Brookhaven National Laboratory
 *
 ***********************************************************************************/

#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"

#include "TrigDecisionTool/TrigDecisionToolCore.h"


#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS) // Full Athena

Trig::TrigDecisionToolCore::TrigDecisionToolCore()
{
  SG::SlotSpecificObj<Trig::CacheGlobalMemory>* ptr = &m_cacheGlobalMemory;
  m_expertMethods=new ExpertMethods(ptr);
}

Trig::CacheGlobalMemory* Trig::TrigDecisionToolCore::cgm() const { 
  const Trig::CacheGlobalMemory* ptr = m_cacheGlobalMemory.get();
  // A consiquence of placing the cache in a slot-specific wrapper 
  return const_cast<Trig::CacheGlobalMemory*>(ptr);
}

#else // Analysis or Standalone

Trig::TrigDecisionToolCore::TrigDecisionToolCore()
{
  Trig::CacheGlobalMemory* ptr = &m_cacheGlobalMemory;
  m_expertMethods=new ExpertMethods(ptr);
}

Trig::CacheGlobalMemory* Trig::TrigDecisionToolCore::cgm() const { 
  return const_cast<Trig::CacheGlobalMemory*>(&m_cacheGlobalMemory);
}

#endif


Trig::TrigDecisionToolCore::~TrigDecisionToolCore() {
  delete m_expertMethods;
}


StatusCode Trig::TrigDecisionToolCore::initialize() {
  ChainGroupInitialize();
  return StatusCode::SUCCESS;
}

StatusCode Trig::TrigDecisionToolCore::finalize() {
  return StatusCode::SUCCESS;
}


