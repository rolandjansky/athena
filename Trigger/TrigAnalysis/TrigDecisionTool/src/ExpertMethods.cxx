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
#include "AthenaKernel/getMessageSvc.h"

#include "TrigDecisionTool/ExpertMethods.h"
#include "TrigSteeringEvent/HLTResult.h"

#include "TrigNavigation/AccessProxy.h"

#include "TrigSteeringEvent/Chain.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TrigDecisionTool/TDTUtilities.h"
#include "TrigDecisionTool/CacheGlobalMemory.h"
#include "TrigDecisionTool/Logger.h"

#define MLOG(x)   if (log().level()<=MSG::x) log() << MSG::x

Trig::ExpertMethods::ExpertMethods(Trig::CacheGlobalMemory* cgm) 
  : Logger("ExpertMethods"),
    m_cacheGlobalMemory(cgm),
    useExperimentalAndExpertMethods(false)   
{
}

Trig::ExpertMethods::~ExpertMethods() {}

bool Trig::ExpertMethods::checkExperimentalAndExpertMethods() const {
  if (useExperimentalAndExpertMethods) return true;
  else {
    log() << MSG::ERROR 
	  <<"You have not confirmed the use of experimental or expert TrigDecisionTool methods at this time.  Please take care before using such methods." 
	  << endreq;
    return false;
  }
}


const TrigConf::TriggerItem*
Trig::ExpertMethods::getItemConfigurationDetails(const std::string& chain) {
  if (!(checkExperimentalAndExpertMethods())) return 0;
  MLOG(VERBOSE) << "getting L1 item configuration details for: " << chain << endreq;
  return cgm(true)->config_item(chain);
}

const TrigConf::HLTChain*
Trig::ExpertMethods::getChainConfigurationDetails(const std::string& chain) {
  if (!(checkExperimentalAndExpertMethods())) return 0;
  MLOG(VERBOSE) << "getting chain configuration details for: " << chain << endreq;
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

const HLT::NavigationCore* Trig::ExpertMethods::getNavigation() const
{
  if (!(checkExperimentalAndExpertMethods())) return 0;
  return cgm()->navigation();
}
Trig::CacheGlobalMemory* Trig::ExpertMethods::cgm(bool onlyConfig) const {
  if ( ! onlyConfig ) {
    if ( !const_cast<Trig::CacheGlobalMemory*>(m_cacheGlobalMemory)->assert_decision() ) {
      log() << MSG::WARNING
	    <<"TDT has not ben able to unpack trigger decision" 
	    << endreq;    
    } 
  } 
  return m_cacheGlobalMemory; 
}




bool Trig::ExpertMethods::isHLTTruncated() const {
  const HLT::HLTResult* res(0);
  if(cgm()->navigation()->getAccessProxy()->retrieve(res, "HLTResult_L2").isFailure()) {
    log() << MSG::WARNING
	  <<"TDT has not ben able to get HLTResult_L2" 
	  << endreq;    
    return false;
  } 
  return res->isHLTResultTruncated();    
}
