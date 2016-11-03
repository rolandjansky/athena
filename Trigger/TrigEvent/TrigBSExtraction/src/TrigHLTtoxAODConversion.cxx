/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBSExtraction/TrigHLTtoxAODConversion.h"
#include "TrigSteeringEvent/HLTResult.h"


TrigHLTtoxAODConversion::TrigHLTtoxAODConversion(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_navigation("HLT::Navigation/Navigation", this),
    m_xAODTool("TrigBStoxAODTool/BStoxAOD",this)
{
  
  declareProperty("HLTResultKey", m_ResultKey = "HLTResult_HLT");
  declareProperty("Navigation", m_navigation);
  declareProperty("BStoxAOD",m_xAODTool);
}

TrigHLTtoxAODConversion::~TrigHLTtoxAODConversion() {}

StatusCode TrigHLTtoxAODConversion::initialize() {

  CHECK(m_navigation.retrieve());
  CHECK(m_xAODTool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode TrigHLTtoxAODConversion::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode TrigHLTtoxAODConversion::execute() {
  //convert all of the present AOD containers to xAOD


    
  ATH_MSG_VERBOSE(evtStore()->dump());
  //we need to const cast b/c we want to change it later
  const HLT::HLTResult* cresult(0);
  CHECK(evtStore()->retrieve(cresult,m_ResultKey));
  HLT::HLTResult* result = const_cast<HLT::HLTResult*>(cresult);

  m_navigation->reset();

  //populate the navigation with what's stored in the HLTResult
  const std::vector<uint32_t>& navData = result->getNavigationResult();
  
  ATH_MSG_DEBUG("serialized navigation has size " << navData.size());
  m_navigation->deserialize( navData );
  m_navigation->prepare();
  CHECK(m_xAODTool->convert(&*m_navigation));

  //after AOD TrigPasBitsCollection was converted to xAOD::TrigPassBitsContainer,
  //let's fill new xAOD::TrigPassBits objects with the proper pointers to
  //converted xAOD containers
  CHECK(m_xAODTool->setTrigPassBits(&*m_navigation));

  //this will redirect all Features to the xAOD converters
  //Note: &* is not a no-op in the following. m_navigation is toolhandle
  //the deref operator * gives back the object held by the tool, i.e. Navigation
  //and ref operator & gives pointer to it.
  CHECK(m_xAODTool->rewireNavigation(&*m_navigation));

  //once we have rewired the result we put it back into the HLTResult
  //so it is available downstream
  result->getNavigationResult().clear();
  bool status = m_navigation->serialize(result->getNavigationResult(), result->getNavigationResultCuts());

  ATH_MSG_DEBUG("new serialized navigation has size " << result->getNavigationResult().size());

  if (!status) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}
