/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : DecisionAnalysis
 *
 * @brief access to information about the trigger decision
 *
 * @author Michael Begel  <michael.begel@cern.ch> - Brookhaven National Laboratory
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - LAPP Annecy
 *
 ***********************************************************************************/

#include <vector>
#include <exception>
#include "TrigDecisionTool/ExpertMethods.h"

#ifndef XAOD_ANALYSIS // Full athena
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigNavigation/AccessProxy.h"
#endif

#include "AsgDataHandles/ReadHandle.h"

#include "TrigSteeringEvent/Chain.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TrigDecisionTool/TDTUtilities.h"
#include "TrigDecisionTool/CacheGlobalMemory.h"
#include "TrigDecisionTool/Logger.h"

#include "xAODTrigger/TrigDecision.h"


#ifndef XAOD_STANDALONE // AthAnalysis or Full Athena

Trig::ExpertMethods::ExpertMethods(SG::SlotSpecificObj<Trig::CacheGlobalMemory>* cgm) 
  : m_cacheGlobalMemory(cgm)
{
}

#else // AnalysisBase

Trig::ExpertMethods::ExpertMethods(Trig::CacheGlobalMemory* cgm) 
  : m_cacheGlobalMemory(cgm)
{
}

#endif

Trig::ExpertMethods::~ExpertMethods() {}

const TrigConf::TriggerItem*
Trig::ExpertMethods::getItemConfigurationDetails(const std::string& chain) const {
  ATH_MSG_VERBOSE("getting L1 item configuration details for: " << chain);
  return cgm(true)->config_item(chain);
}

const TrigConf::HLTChain*
Trig::ExpertMethods::getChainConfigurationDetails(const std::string& chain) const {
  ATH_MSG_VERBOSE("getting chain configuration details for: " << chain);
  return cgm(true)->config_chain(chain);
}


const HLT::Chain* Trig::ExpertMethods::getChainDetails(const std::string& chain) const {
  return cgm()->chain(chain);
}

const LVL1CTP::Lvl1Item* Trig::ExpertMethods::getItemDetails(const std::string& chain) const {
  return cgm()->item(chain);
}

#ifndef XAOD_STANDALONE // AthAnalysis or full Athena

Trig::CacheGlobalMemory* Trig::ExpertMethods::cgm(bool onlyConfig) const {
  if ( ! onlyConfig ) {
    if ( !const_cast<Trig::CacheGlobalMemory*>(m_cacheGlobalMemory->get())->assert_decision() ) {
      ATH_MSG_WARNING("TDT has not ben able to unpack trigger decision");    
    } 
  } 
  return m_cacheGlobalMemory->get(); 
}

// NOTE: Nested ifndef
#ifndef XAOD_ANALYSIS // Full Athena only sub-part 

const HLT::NavigationCore* Trig::ExpertMethods::getNavigation() const
{
  return dynamic_cast<const HLT::NavigationCore*>(cgm()->navigation());
}

#else // AthAnalysis only sub-part

const HLT::TrigNavStructure* Trig::ExpertMethods::getNavigation() const
{
  return dynamic_cast<const HLT::TrigNavStructure*>(cgm()->navigation());
}

#endif // NOTE: End of nested ifndef

#else // AnalysisBase

const HLT::TrigNavStructure* Trig::ExpertMethods::getNavigation() const
{
  return cgm()->navigation();
}

Trig::CacheGlobalMemory* Trig::ExpertMethods::cgm(bool onlyConfig) const {
  if ( ! onlyConfig ) {
    if ( !const_cast<Trig::CacheGlobalMemory*>(m_cacheGlobalMemory)->assert_decision() ) {
      ATH_MSG_WARNING("TDT has not ben able to unpack trigger decision");    
    } 
  } 
  return m_cacheGlobalMemory; 
}

#endif


bool Trig::ExpertMethods::isHLTTruncated() const {

  const SG::ReadHandleKey<xAOD::TrigDecision>* trigDecRH = cgm()->xAODTrigDecisionKey();
  if (trigDecRH && !trigDecRH->empty()) {
    SG::ReadHandle<xAOD::TrigDecision> trigDec = SG::makeHandle(*trigDecRH);
    if(!trigDec.isValid()) {
      ATH_MSG_DEBUG("TDT has not been able to retrieve xTrigDecision");
    } else {
      return trigDec->efTruncated();
    }
  }

#ifndef XAOD_ANALYSIS // Full Athena only 
  SG::ReadHandle<HLT::HLTResult> hltResult("HLTResult_HLT");
  if(!hltResult.isValid()) {
    ATH_MSG_WARNING("TDT has not ben able to get HLTResult_HLT");
    return false;
  }
  return hltResult->isHLTResultTruncated();   
#else // AnalysisBase or AthAnalysis
  ATH_MSG_ERROR("isHLTTruncated only supported with a xAOD::TrigDecision ReadHandle (Runs 2,3) or in full Athena (Run 2)");
  return false;
#endif
  
}
