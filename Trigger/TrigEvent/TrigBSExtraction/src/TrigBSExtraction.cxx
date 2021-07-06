/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBSExtraction/TrigBSExtraction.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigSerializeCnvSvc/TrigSerializeConvHelper.h"

TrigBSExtraction::TrigBSExtraction(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
}


StatusCode TrigBSExtraction::initialize() {

  // L2 navigation tool is optional
  if ( !m_l2ResultKey.empty() ) ATH_CHECK( m_navToolL2.retrieve() );
  else m_navToolL2.disable();

  ATH_CHECK( m_navTool.retrieve() );

  // Initialize the TrigSerializeConcHelper here so that all the additional streamerinfos are read in.
  // In case of DataScouting this may give problems if the DS ROBs contain containers which
  // were not compiled with navigation (the container type information is then not known
  // for a given CLID).
  ToolHandle<ITrigSerializeConvHelper> trigCnvHelper("TrigSerializeConvHelper",this);
  ATH_CHECK( trigCnvHelper.retrieve() );

  return StatusCode::SUCCESS;
}


StatusCode TrigBSExtraction::execute() {

  if ( m_navToolL2.isEnabled() ) {
    if ( repackFeaturesToSG(*m_navToolL2, m_l2ResultKey, false).isFailure() )
      ATH_MSG_WARNING( "failed unpacking features from BS to SG for: " << m_l2ResultKey  );
  }
  
  if ( !m_hltResultKey.empty() ) {
    // unpack and merge with L2 result in case there is one
    if ( repackFeaturesToSG(*m_navTool, m_hltResultKey, m_navToolL2.isEnabled() ).isFailure() )
      ATH_MSG_WARNING( "failed unpacking features from BS to SG for: " << m_hltResultKey  );
  }

  for (const auto& ds_key : m_dataScoutingKeys.value()) {
    if ( repackFeaturesToSG(*m_navTool, ds_key, false).isFailure() )
      ATH_MSG_WARNING( "failed unpacking features from BS to SG for: " << ds_key );
  }

  if ( m_navToolL2.isEnabled() ) m_navToolL2->reset();
  m_navTool->reset();

  return StatusCode::SUCCESS;
}


StatusCode TrigBSExtraction::repackFeaturesToSG (HLT::Navigation& navTool,
                                                 const std::string& key, bool equalize ) {

  const HLT::HLTResult * constresult(nullptr);
  HLT::HLTResult * result(nullptr);

  ATH_MSG_DEBUG( "Trying to deserialize content of '" << key << "'" );
  if (evtStore()->contains<HLT::HLTResult>(key)) {
    evtStore()->retrieve(constresult, key).ignore();
    result = const_cast<HLT::HLTResult*>(constresult);
    
    ATH_MSG_DEBUG("HLTResult is level=" << result->getHLTLevel()  );

    const std::vector<uint32_t>& navData = result->getNavigationResult();
    if ( !navData.empty() ) {
      ATH_MSG_DEBUG( "Navigation payload obtained from '" << key << "' has size " << navData.size()  );
      navTool.deserialize( navData );
    } else {
      ATH_MSG_WARNING( "Navigation payload obtained from '" << key << "' has size 0" );
    }

  } else {
    ATH_MSG_WARNING( "No HLTResult found with key '" << key << "'" );
  }

  if ( equalize && m_navToolL2.isEnabled() ) {
    ATH_MSG_DEBUG( "Merging L2 and EF navigation structures for " << key );
    navTool.merge(*m_navToolL2);
  }

  navTool.prepare();
  // change the clid and subtype index in the feature access helpers
  // to the new xAOD containers (that will be converted after this call)
  std::vector< HLT::TriggerElement* > testvec;
  navTool.getAll(testvec,false);

  // pack navigation back into the result
  result->getNavigationResult().clear();
  bool status = navTool.serialize(result->getNavigationResult(), result->getNavigationResultCuts());
  ATH_MSG_DEBUG( "new serialized navigation has size " << result->getNavigationResult().size() );

  return StatusCode(status);
}

