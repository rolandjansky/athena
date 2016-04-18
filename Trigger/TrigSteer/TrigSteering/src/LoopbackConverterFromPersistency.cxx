/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : LoopbackConverterFromPeristency
 *
 * @brief
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - UC Irvine
 *
 * File and Version Information:
 * $Id: LoopbackConverterFromPersistency.cxx,v 1.7 2009-02-16 15:47:05 tbold Exp $
 **********************************************************************************/

#include "TrigSteering/LoopbackConverterFromPersistency.h"


#include "TrigSteering/SteeringChain.h"

#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/TriggerElement.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/HLTResult.h"

#include "TrigT1Interfaces/RecEmTauRoI.h"

#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TrigInterfaces/AlgoConfig.h"


using namespace HLT;


LoopbackConverterFromPersistency::LoopbackConverterFromPersistency(const std::string& name,
							   const std::string& type, const IInterface* parent)
  : LvlConverter(name, type, parent)
{
  declareProperty("ActivateAllChains", m_activateAllChains=true, 
		  "If true all chains should be activated irrespectively of the fact that they were rejected in previous pass, else only those which were left active.");


  declareProperty("RemoveOldResult", m_removeOldResult=true, "If set to true removes old HLT result. Should be true for hypos re-running, flase if both results are needed.");
  
  declareProperty("UnpackNavigation", m_unpackNavigation=true, "If set to true unpacks the navigation.");
  declareProperty("ResultName", m_resultName="", "Name for HLT Result. If empty the HLTResult_(L2/EF) are deduced.");
}


ErrorCode LoopbackConverterFromPersistency::hltInitialize()
{
  return HLT::OK;
}


ErrorCode LoopbackConverterFromPersistency::hltFinalize()
{
  return HLT::OK;
}


ErrorCode LoopbackConverterFromPersistency::hltExecute(std::vector<HLT::SteeringChain*>& chains)
{
  const HLTResult* constResult = 0;
  StatusCode sc = evtStore()->retrieve(constResult, (m_resultName.empty() ? "HLTResult"+m_config->getInstance() : m_resultName) );

  if (sc.isFailure()) {
    ATH_MSG_ERROR("No L2Result found in Store");
    return HLT::ERROR;
  }

  // update hltAccessTool with new HLTResult:
  if (!m_hltTool->updateResult(*constResult, m_config)) {
    ATH_MSG_ERROR("L2Result errorus, HLTResultAccessTool can't digest it");
    return HLT::WRONG_HLT_RESULT;
  }

  // Deserialize the navigation


  if ( m_unpackNavigation ) { 
    ErrorCode ec;
    ec = deserializeNavigation(*m_config->getNavigation(), *constResult);
    if ( ec != HLT::OK) {
      ATH_MSG_ERROR("Failed to deserialize navigation");
      return ec;
    }
  }
  // set Lvl1 ID
  m_config->setLvl1Id(constResult->getLvl1Id());

  // get reference to all chains from previous level
  const std::vector<Chain>& previousChains = m_hltTool->getChains();

  //activate the chains
  ErrorCode ec;
  ec = activateChains(chains, previousChains);
  if ( ec != HLT::OK ) {
    ATH_MSG_ERROR("Failed to activate chains");
    return ec;
  }

  // remove old L2Result from SG (needed when rerunning Hypos, not needed when full rerunning is done)
  if ( m_removeOldResult ) {
    if ( evtStore()->remove(constResult).isFailure() ) {
      ATH_MSG_ERROR("Failed to remove old result from SG");
      return HLT::ERROR;
    }
  }

  return HLT::OK;
}




ErrorCode LoopbackConverterFromPersistency::setConfiguredChains(const std::vector<HLT::SteeringChain*>& configuredChains) {
  std::vector<HLT::SteeringChain*>::const_iterator it;
  for ( it = configuredChains.begin(); it != configuredChains.end(); ++it ) {
    m_counterToChain[(*it)->getChainCounter()] = *it;
  }
  return HLT::OK;
}


/**
 * Activates chains
 * NOTE: this is a little different from what is being done in the base class activateChains !!!
 */
ErrorCode LoopbackConverterFromPersistency::activateChains(std::vector<HLT::SteeringChain*>& chains,
							   const std::vector<HLT::Chain>& chainsFromPreviuousRun, bool ) {
  ATH_MSG_DEBUG("activateChains "<< name());

  std::vector<Chain>::const_iterator preChain;
  for ( preChain = chainsFromPreviuousRun.begin(); preChain != chainsFromPreviuousRun.end(); ++preChain ) {    
    if ( preChain->chainPassed() || m_activateAllChains ) {

      ATH_MSG_DEBUG("activating chain of counter "<< preChain->getChainCounter() << std::endl << *preChain);

      unsigned int counter = preChain->getChainCounter();

      if ( m_counterToChain.find(counter) == m_counterToChain.end() ) {
        ATH_MSG_DEBUG("Chain with counter: " << preChain->getChainCounter()
                      << " absent in the list of configured chains, you run different menu");
      } else {
        // else go ahead with activation
        SteeringChain *chain = m_counterToChain[counter];
        chain->setActive();
        // also, set prescale and pass-through status to the same as in the previous run!!!
        if (preChain->isPrescaled()) chain->setPrescaled();
        if (preChain->isPassedThrough()) chain->setPassThrough();
        if (preChain->isResurrected()) chain->setResurrected();
        
        ATH_MSG_DEBUG("activated chain: "<<  *chain);
        
        chains.push_back(chain);
        ATH_MSG_VERBOSE("activated chain of counter "<< preChain->getChainCounter());
      }
    }
  }
  return HLT::OK;
}
