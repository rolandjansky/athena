/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

#include "CxxUtils/checker_macros.h"

#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"

#include "TrigDecisionTool/TrigDecisionToolCore.h"


Trig::TrigDecisionToolCore::TrigDecisionToolCore() :
  m_expertMethods(&m_cacheGlobalMemory)
{
}

Trig::CacheGlobalMemory* Trig::TrigDecisionToolCore::cgm() {
  Trig::CacheGlobalMemory* ptr = m_cacheGlobalMemory.get();
  // A consequence of placing the cache in a slot-specific wrapper
  ptr->navigation (m_navigation);
  return ptr;
}

const Trig::CacheGlobalMemory* Trig::TrigDecisionToolCore::cgm() const {
  const Trig::CacheGlobalMemory* ptr = m_cacheGlobalMemory.get();
  // A consequence of placing the cache in a slot-specific wrapper
  Trig::CacheGlobalMemory* p ATLAS_THREAD_SAFE = const_cast<Trig::CacheGlobalMemory*>(ptr);
  p->navigation (m_navigation);
  return ptr;
}

Trig::TrigDecisionToolCore::~TrigDecisionToolCore() {
}


StatusCode Trig::TrigDecisionToolCore::initialize() {
  ChainGroupInitialize();
  return StatusCode::SUCCESS;
}

StatusCode Trig::TrigDecisionToolCore::finalize() {
  return StatusCode::SUCCESS;
}
