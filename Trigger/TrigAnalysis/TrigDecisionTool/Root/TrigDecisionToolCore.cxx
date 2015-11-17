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

Trig::TrigDecisionToolCore::TrigDecisionToolCore()
{
  m_cacheGlobalMemory=new CacheGlobalMemory();
  m_expertMethods=new ExpertMethods(m_cacheGlobalMemory);
  ChainGroupInitialize();
}

Trig::TrigDecisionToolCore::~TrigDecisionToolCore() {
  delete m_cacheGlobalMemory;
  delete m_expertMethods;
}


StatusCode Trig::TrigDecisionToolCore::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode Trig::TrigDecisionToolCore::finalize() {
  return StatusCode::SUCCESS;
}


