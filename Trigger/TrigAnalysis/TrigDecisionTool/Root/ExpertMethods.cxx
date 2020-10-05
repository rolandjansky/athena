/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS)
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


#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS) // Full Athena

Trig::ExpertMethods::ExpertMethods(SG::SlotSpecificObj<Trig::CacheGlobalMemory>* cgm) 
  : m_cacheGlobalMemory(cgm),
    m_useExperimentalAndExpertMethods(false)   
{
}

#else // Analysis or Standalone

Trig::ExpertMethods::ExpertMethods(Trig::CacheGlobalMemory* cgm) 
  : m_cacheGlobalMemory(cgm),
    m_useExperimentalAndExpertMethods(false)   
{
}

#endif

Trig::ExpertMethods::~ExpertMethods() {}

bool Trig::ExpertMethods::checkExperimentalAndExpertMethods() const {
  if (m_useExperimentalAndExpertMethods) return true;
  else {
    ATH_MSG_ERROR("You have not confirmed the use of experimental or expert TrigDecisionTool methods at this time.  Please take care before using such methods.");
    return false;
  }
}


const TrigConf::TriggerItem*
Trig::ExpertMethods::getItemConfigurationDetails(const std::string& chain) {
  if (!(checkExperimentalAndExpertMethods())) return 0;
  ATH_MSG_VERBOSE("getting L1 item configuration details for: " << chain);
  return cgm(true)->config_item(chain);
}

const TrigConf::HLTChain*
Trig::ExpertMethods::getChainConfigurationDetails(const std::string& chain) {
  if (!(checkExperimentalAndExpertMethods())) return 0;
  ATH_MSG_VERBOSE("getting chain configuration details for: " << chain);
  return cgm(true)->config_chain(chain);
}


const HLT::Chain* Trig::ExpertMethods::getChainDetails(const std::string& chain) const {
  if (!(checkExperimentalAndExpertMethods())) return 0;
  return cgm()->chain(chain);
  
}

const LVL1CTP::Lvl1Item* Trig::ExpertMethods::getItemDetails(const std::string& chain) const {
  if (!(checkExperimentalAndExpertMethods())) return 0;
  return cgm()->item(chain);
}

#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS) // Full Athena

const HLT::NavigationCore* Trig::ExpertMethods::getNavigation() const
{
  if (!(checkExperimentalAndExpertMethods())) return 0;
  return dynamic_cast<const HLT::NavigationCore*>(cgm()->navigation());
}

Trig::CacheGlobalMemory* Trig::ExpertMethods::cgm(bool onlyConfig) const {
  if ( ! onlyConfig ) {
    if ( !const_cast<Trig::CacheGlobalMemory*>(m_cacheGlobalMemory->get())->assert_decision() ) {
      ATH_MSG_WARNING("TDT has not ben able to unpack trigger decision");    
    } 
  } 
  return m_cacheGlobalMemory->get(); 
}

#else // Analysis or Standalone

const HLT::TrigNavStructure* Trig::ExpertMethods::getNavigation() const
{
  if (!(checkExperimentalAndExpertMethods())) return 0;
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

  SG::ReadHandleKey<xAOD::TrigDecision>* trigDecRH = cgm()->xAODTrigDecisionKey();
  if (trigDecRH && !trigDecRH->empty()) {
    SG::ReadHandle<xAOD::TrigDecision> trigDec = SG::makeHandle(*trigDecRH);
    if(!trigDec.isValid()) {
      ATH_MSG_DEBUG("TDT has not been able to retrieve xTrigDecision");
    } else {
      return trigDec->efTruncated();
    }
  }

#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS)
  const HLT::HLTResult* res(nullptr);
  auto navigation = getNavigation();
  if(!navigation || navigation->getAccessProxy()->retrieve(res, "HLTResult_HLT").isFailure()) {
    ATH_MSG_WARNING("TDT has not ben able to get HLTResult_HLT");
    return false;
  }
  return res->isHLTResultTruncated();   
#else
  ATH_MSG_ERROR("isHLTTruncated only supported with a xAOD::TrigDecision ReadHandle (Runs 2,3) or in full Athena (Run 2)");
  return false;
#endif
  
}
