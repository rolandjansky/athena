/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

  // xAOD converter tool (for Run-1 data)
  if ( !m_xAODTool.empty() ) ATH_CHECK( m_xAODTool.retrieve() );
  else m_xAODTool.disable();

  // Initialize the TrigSerializeConcHelper here so that all the additional streamerinfos are read in.
  // In case of DataScouting this may give problems if the DS ROBs contain containers which
  // were not compiled with navigation (the container type information is then not known
  // for a given CLID).
  ToolHandle<ITrigSerializeConvHelper> trigCnvHelper("TrigSerializeConvHelper",this);
  ATH_CHECK( trigCnvHelper.retrieve() );

  return StatusCode::SUCCESS;
}


StatusCode TrigBSExtraction::execute() {

  const bool isRun1 = m_navToolL2.isEnabled();
  if ( isRun1 ) {
    if ( repackFeaturesToSG(*m_navToolL2, m_l2ResultKey, false, false).isFailure() )
      ATH_MSG_WARNING( "failed unpacking features from BS to SG for: " << m_l2ResultKey  );
  }
  
  if ( !m_hltResultKey.empty() ) {
    // unpack, merge with L2 result and do xAOD conversion
    // xAOD conversion is only done for HLTResult_EF in Run-1
    if ( repackFeaturesToSG(*m_navTool, m_hltResultKey, isRun1, isRun1).isFailure() )
      ATH_MSG_WARNING( "failed unpacking features from BS to SG for: " << m_hltResultKey  );
  }

  for (const auto& ds_key : m_dataScoutingKeys.value()) {
    if ( repackFeaturesToSG(*m_navTool, ds_key, false, false).isFailure() )
      ATH_MSG_WARNING( "failed unpacking features from BS to SG for: " << ds_key );
  }

  if ( isRun1 ) m_navToolL2->reset();
  m_navTool->reset();

  return StatusCode::SUCCESS;
}


StatusCode TrigBSExtraction::repackFeaturesToSG (HLT::Navigation& navTool,
                                                 const std::string& key,
                                                 bool equalize,
                                                 bool xAODCnv) {

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
    return StatusCode::SUCCESS;
  }

  if ( equalize && m_navToolL2.isEnabled() ) {
    ATH_MSG_DEBUG( "Merging L2 and EF navigation structures for " << key );
    navTool.merge(*m_navToolL2);
  }

  navTool.prepare();

  // optional xAOD conversion for Run-1 AOD containers
  if ( xAODCnv && m_xAODTool.isEnabled() ) {

    ATH_CHECK( m_xAODTool->convert(&navTool) );

    // after AOD TrigPassBitsCollection was converted to xAOD::TrigPassBitsContainer,
    // let's fill new xAOD::TrigPassBits objects with the proper pointers to
    // converted xAOD containers
    ATH_CHECK( m_xAODTool->setTrigPassBits(&navTool) );

    // this will redirect all Features to the xAOD converters
    ATH_CHECK( m_xAODTool->rewireNavigation(&navTool) );
  }

  // pack navigation back into the result
  result->getNavigationResult().clear();
  bool status = navTool.serialize(result->getNavigationResult(), result->getNavigationResultCuts());
  ATH_MSG_DEBUG( "new serialized navigation has size " << result->getNavigationResult().size() );

  return StatusCode(status);
}

