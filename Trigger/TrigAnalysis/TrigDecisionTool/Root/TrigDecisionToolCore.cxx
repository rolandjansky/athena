/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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


Trig::TrigDecisionToolCore::TrigDecisionToolCore() :
  m_expertMethods(&m_cacheGlobalMemory)
{
}

#ifndef XAOD_STANDALONE // AtheAnalysis or full Athena

Trig::CacheGlobalMemory* Trig::TrigDecisionToolCore::cgm() const { 
  const Trig::CacheGlobalMemory* ptr = m_cacheGlobalMemory.get();
  // A consequence of placing the cache in a slot-specific wrapper
  Trig::CacheGlobalMemory* p = const_cast<Trig::CacheGlobalMemory*>(ptr);
  p->navigation (m_navigation);
  return p;
}

#else // AnalysisBase

Trig::CacheGlobalMemory* Trig::TrigDecisionToolCore::cgm() const {
  return const_cast<Trig::CacheGlobalMemory*>(&m_cacheGlobalMemory);
}

#endif


Trig::TrigDecisionToolCore::~TrigDecisionToolCore() {
}


StatusCode Trig::TrigDecisionToolCore::initialize() {
  ChainGroupInitialize();
  return StatusCode::SUCCESS;
}

StatusCode Trig::TrigDecisionToolCore::finalize() {
  return StatusCode::SUCCESS;
}
