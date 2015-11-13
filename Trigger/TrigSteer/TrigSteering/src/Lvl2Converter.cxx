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
 * $Id: Lvl2Converter.cxx,v 1.24 2009-03-25 08:17:12 tbold Exp $
 **********************************************************************************/

#include "TrigSteering/Lvl2Converter.h"
#include "TrigSteeringEvent/Chain.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigNavigation/Navigation.h"
#include "TrigSteering/HLTResultAccessTool.h"
#include "TrigSerializeResult/StringSerializer.h"

#include <limits>

using namespace HLT;


Lvl2Converter::Lvl2Converter(const std::string& name,
			     const std::string& type, const IInterface* parent)
  : LvlConverter(name, type, parent),
    m_smk(0),
    m_psk(0)
{
  declareProperty("l2ResultName", m_l2Name = "HLTResult_L2", "StoreGate key for L2 HLTResult");
  declareProperty("MakePassThroughSticky", m_stickyPassThrough = true, "keep L2 Chain passThrough state ?");
  declareProperty("checkConfig", m_checkConfig = true, "Check configuration keys in L2 result.");
  declareProperty("errorLevel" , m_errorLevel="ABORT_CHAIN", "Error action above which the L2 result should be declared as to bad to run the EF." );
}

ErrorCode Lvl2Converter::hltInitialize() { 
  m_maxEC = getErrorCodePosFromStr(m_errorLevel+" UNKNOWN UNKNOWN");
  return HLT::OK; 
} 



ErrorCode Lvl2Converter::hltExecute(std::vector<HLT::SteeringChain*>& chains)
{
  if (!m_config) {
    ATH_MSG_ERROR("No AlgoConfig pointer");
    return HLT::ERROR;
  }

  const HLTResult* constL2Result;
  StatusCode sc = evtStore()->retrieve(constL2Result, m_l2Name);
  if(sc.isFailure()){
    ATH_MSG_ERROR("No HLTResult with name " << m_l2Name << " found in StoreGate");
    return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::SteeringInternalReason::NO_HLT_RESULT);
 }
  HLTResult* l2result = const_cast<HLTResult*>(constL2Result);


  // make sure we're reading in a L2 HLTResult !
  if ( l2result->getHLTLevel() != HLT::L2) {
    ATH_MSG_ERROR("HLTResult level is not HLT::L2");
    return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::USERDEF_4,  HLT::SteeringInternalReason::WRONG_HLT_RESULT);
  }

  // make sure that the result was ever seen by L2 (it can be the case that it is created at L2SV)
  if ( l2result->isCreatedOutsideHLT() ) {
    ATH_MSG_DEBUG("L2Result was not created by steering at L2. Was created by L2Supervisor");
    return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::USERDEF_1,  HLT::SteeringInternalReason::NO_HLT_RESULT);
  }

  // make sure that the processing at L2 was OK
  ATH_MSG_DEBUG("L2Result indicates at L2: " << strErrorCode(l2result->getHLTStatus()) 
                 << " L1 converter: " << strErrorCode(l2result->getLvlConverterStatus()));
  if ( l2result->getHLTStatus().action() > m_maxEC.action() ) {
    return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::USERDEF_2, HLT::SteeringInternalReason::WRONG_HLT_RESULT);
  }


  // make sure that config keys are the same as in Lvl2 (but only if set)
  if (m_checkConfig) {
    static uint32_t noKey = std::numeric_limits<uint32_t>::max();  // see HLTFrame
    if ( (m_smk != l2result->getConfigSuperMasterKey() || m_psk != l2result->getConfigPrescalesKey()) &&
	 (m_smk != noKey && l2result->getConfigSuperMasterKey() != noKey &&
	  m_psk != noKey && l2result->getConfigPrescalesKey() != noKey) ) {
      if (msgLvl(MSG::FATAL)) {
        std::string appName("UNKNOWN");
        StringSerializer().deserialize(l2result->getExtras(), appName);
        ATH_MSG_FATAL("L2Result created with configuration keys (SMK,PSK) = (" 
                      <<  l2result->getConfigSuperMasterKey() << "," << l2result->getConfigPrescalesKey()
                      << ") by " << appName << " while EF uses (" << m_smk << "," << m_psk << ")");                      
      }
      return HLT::ErrorCode( HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP, 
			     HLT::SteeringInternalReason::WRONG_HLT_RESULT);
    }
  }

  // update hltAccessTool with new HLTResult:
  if (! m_hltTool->updateResult(*l2result, m_config)) {
    ATH_MSG_DEBUG("L2Result erroneus, HLTResultAccessTool can't digest it");
    return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::USERDEF_3, HLT::SteeringInternalReason::WRONG_HLT_RESULT);
  }

  // get reference to all chains from previous level
  const std::vector<Chain>& previousChains = m_hltTool->getChains();

  // unpack navigation from previous level
  ErrorCode ec;
  ec = deserializeNavigation(*m_config->getNavigation(), *l2result);
  if ( ec != HLT::OK ) {
    ATH_MSG_ERROR("Failed to deserialize navigation");
    return ec;
  }

  //activate the chains
  ec = activateChains(chains, previousChains, m_stickyPassThrough);
  if ( ec != HLT::OK ) {
    ATH_MSG_ERROR("Failed to activate chains");
    return ec;
  }


  return HLT::OK;
}
