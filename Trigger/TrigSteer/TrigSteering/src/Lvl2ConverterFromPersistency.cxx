/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : Lvl2Converter
 *
 * @brief
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: Lvl2ConverterFromPersistency.cxx,v 1.8 2009-01-16 11:09:51 tbold Exp $
 **********************************************************************************/

#include "TrigSteering/Lvl2ConverterFromPersistency.h"

#include "TrigSteeringEvent/Chain.h"

#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/TriggerElement.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/HLTResult.h"

#include "TrigT1Interfaces/RecEmTauRoI.h"

#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TrigInterfaces/AlgoConfig.h"



using namespace HLT;


Lvl2ConverterFromPersistency::Lvl2ConverterFromPersistency(const std::string& name,
							   const std::string& type, const IInterface* parent)
  : LvlConverter(name, type, parent),
    m_tmpNavigation("HLT::Navigation/AuxiliaryNavigation", this)

{
  declareProperty("l2ResultName", m_l2Name = "HLTResult_L2");
  declareProperty("efResultName", m_efName = "HLTResult_EF");
  declareProperty("AuxiliaryNavigation", m_tmpNavigation, "Navigation used to unpack L2");
}


ErrorCode Lvl2ConverterFromPersistency::hltInitialize()
{
  if ( m_tmpNavigation.retrieve().isFailure() ) {
    ATH_MSG_FATAL("failed to get navigation, can not retrieve navigation tool: "<< m_tmpNavigation);
    return  HLT::BAD_JOB_SETUP;
  }
  return HLT::OK;
}


ErrorCode Lvl2ConverterFromPersistency::hltFinalize()
{
  return HLT::OK;
}


ErrorCode Lvl2ConverterFromPersistency::hltExecute(std::vector<HLT::SteeringChain*>& chains)
{
  if (!m_config) {
    ATH_MSG_ERROR("No AlgoConfig pointer");
    return HLT::ERROR;
  }

  const HLTResult* constL2Result;
  StatusCode sc = evtStore()->retrieve(constL2Result, m_l2Name);

  if(sc.isFailure()){
    ATH_MSG_ERROR("No HLTResult with name " << m_l2Name << " found in StoreGate");
    return HLT::NO_HLT_RESULT;
  }



  const HLTResult* constEFResult;
  sc = evtStore()->retrieve(constEFResult, m_efName);

  if(sc.isFailure()){
    ATH_MSG_ERROR("No HLTResult with name " << m_efName << " found in StoreGate");
    return HLT::NO_HLT_RESULT;
  }

  ATH_MSG_VERBOSE("L2 and EF results retrieved from SG");

  
  // update hltAccessTool with new HLTResult_L2 and retrieve chains from this level:
  if (!m_hltTool->updateResult(*constL2Result, m_config)) {
    ATH_MSG_DEBUG("L2Result errorus, HLTResultAccessTool can't digest it");
    return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::USERDEF_3, HLT::SteeringInternalReason::WRONG_HLT_RESULT);
  }
  const std::vector<Chain>& l2Chains = m_hltTool->getChains();

  /*
  // update hltAccessTool with new HLTResult_EF and retireve chains from this level:
  if (!m_hltTool->updateResult(*constEFResult, m_config)) {
  ATH_MSG_ERROR("L2Result errorus, HLTResultAccessTool can't digest it");
  return HLT::WRONG_HLT_RESULT;
  }
  //  const std::vector<Chain>& efChains = m_hltTool->getChains();
  */
 
  //activate the chains
  ErrorCode ec;
  ec = activateChains(chains, l2Chains);
  if ( ec != HLT::OK ) {
    ATH_MSG_ERROR("Failed to activate chains");
    return ec;
  }

  // do the forward deactivation of the navigation structure
  // Deserialize L2 navigation

  ec = deserializeNavigation(m_tmpNavigation.operator*(), *constL2Result);
  if ( ec != HLT::OK) {
    ATH_MSG_ERROR("Failed to deserialize L2 navigation");
    return ec;
  }

  
  
  // Deserialize EF navigation
  ec = deserializeNavigation(*m_config->getNavigation(), *constEFResult);
  if ( ec != HLT::OK) {
    ATH_MSG_ERROR("Failed to deserialize EF navigation");
    return ec;
  }


  // now is the trick
  if ( ! m_config->getNavigation()->propagateDeactivation(m_tmpNavigation.operator->()) ) {
    ATH_MSG_ERROR("Failed to propagate deactivation from Lvl2");
    return HLT::NAV_ERROR;  
  }
  // once the L2 Navigatio is use it can be reset
  m_tmpNavigation->reset();
  


  // set Lvl1 ID
  m_config->setLvl1Id(constL2Result->getLvl1Id());

  // remove old EFResult from SG
  if ( evtStore()->remove(constEFResult).isFailure() ) {
    ATH_MSG_ERROR("Failed to remove old result from SG");
    return HLT::ERROR;
  }

  return HLT::OK;
}

