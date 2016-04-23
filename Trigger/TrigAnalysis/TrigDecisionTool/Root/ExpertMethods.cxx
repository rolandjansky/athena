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

#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigNavigation/AccessProxy.h"
#endif

#include "TrigSteeringEvent/Chain.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TrigDecisionTool/TDTUtilities.h"
#include "TrigDecisionTool/CacheGlobalMemory.h"
#include "TrigDecisionTool/Logger.h"

Trig::ExpertMethods::ExpertMethods(Trig::CacheGlobalMemory* cgm) 
  : m_cacheGlobalMemory(cgm),
    m_useExperimentalAndExpertMethods(false)   
{
}

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

#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
const HLT::NavigationCore* Trig::ExpertMethods::getNavigation() const
{
  if (!(checkExperimentalAndExpertMethods())) return 0;
  return dynamic_cast<const HLT::NavigationCore*>(cgm()->navigation());
}
#else
const HLT::TrigNavStructure* Trig::ExpertMethods::getNavigation() const
{
  if (!(checkExperimentalAndExpertMethods())) return 0;
  return cgm()->navigation();
}
#endif


Trig::CacheGlobalMemory* Trig::ExpertMethods::cgm(bool onlyConfig) const {
  if ( ! onlyConfig ) {
    if ( !const_cast<Trig::CacheGlobalMemory*>(m_cacheGlobalMemory)->assert_decision() ) {
      ATH_MSG_WARNING("TDT has not ben able to unpack trigger decision");    
    } 
  } 
  return m_cacheGlobalMemory; 
}




bool Trig::ExpertMethods::isHLTTruncated() const {
#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
const HLT::HLTResult* res(0);
auto navigation = dynamic_cast<HLT::NavigationCore*>(const_cast<HLT::TrigNavStructure*>(cgm()->navigation()));
if(!navigation || navigation->getAccessProxy()->retrieve(res, "HLTResult_L2").isFailure()) {
ATH_MSG_WARNING("TDT has not been able to get HLTResult_L2");
return false;
} 
return res->isHLTResultTruncated();    
#else
ATH_MSG_ERROR("isHLTTruncated only supported in full Athena");
return false;
#endif
}
