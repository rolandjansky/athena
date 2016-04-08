/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBSExtraction/TrigBSExtraction.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigSerializeCnvSvc/TrigSerializeConvHelper.h"

TrigBSExtraction::TrigBSExtraction(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_navigationForEF("HLT::Navigation/Navigation", this),
    m_navigationForL2("HLT::Navigation/NavigationForL2", this),
    m_nav(0)
{
  
  declareProperty("L2ResultKey",     m_l2ResultKey = "HLTResult_L2");
  declareProperty("EFResultKey",     m_efResultKey = "HLTResult_EF");
  declareProperty("HLTResultKey",    m_hltResultKey = "HLTResult_HLT");
  declareProperty("DSResultKeys",    m_dataScoutingKeys);

  declareProperty("Navigation",      m_navigationForEF);
  declareProperty("NavigationForL2", m_navigationForL2);
}
TrigBSExtraction::~TrigBSExtraction() {}

StatusCode TrigBSExtraction::initialize() {
  ATH_CHECK( m_navigationForEF.retrieve() );
  ATH_CHECK( m_navigationForL2.retrieve() );

  // Initialize the TrigSerializeConcHelper here so that all the additional streamerinfos are read in.
  // In case of DataScouting this may give problems if the DS ROBs contain containers which
  // were not compiled with navigation (the container type information is then not known
  // for a gigen CLID).
  ToolHandle<ITrigSerializeConvHelper> trigCnvHelper("TrigSerializeConvHelper",this);
  ATH_CHECK( trigCnvHelper.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode TrigBSExtraction::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode TrigBSExtraction::execute() {
  ATH_MSG_DEBUG("has keys: HLTResult L2="<< m_l2ResultKey <<" EF=" <<m_efResultKey<<" HLT="<<m_hltResultKey );
  if ( m_l2ResultKey != "" ) {
    m_nav = &*m_navigationForL2;
    if ( repackFeaturesToSG(m_l2ResultKey,  (m_efResultKey == "") ).isFailure() ) 
      ATH_MSG_WARNING( "failed unpacking features from BS to SG for: " << m_l2ResultKey  );
    else
      ATH_MSG_DEBUG( "success unpacking features from BS to SG for: " << m_l2ResultKey  );
  }
  
  m_nav = &*m_navigationForEF;
  if ( m_efResultKey != "" ) {
    if ( repackFeaturesToSG(m_efResultKey, true).isFailure() ) 
      ATH_MSG_WARNING( "failed unpacking features from BS to SG for: " << m_efResultKey  );
    else
      ATH_MSG_DEBUG( "success unpacking features from BS to SG for: " << m_efResultKey  );
  }

  if ( m_hltResultKey != "" ) {
    if ( repackFeaturesToSG(m_hltResultKey, true).isFailure() ) 
      ATH_MSG_WARNING( "failed unpacking features from BS to SG for: " << m_hltResultKey  );
    else
      ATH_MSG_DEBUG( "success unpacking features from BS to SG for: " << m_hltResultKey  );
  }

  for (const auto& ds_key : m_dataScoutingKeys.value()) {
    if ( repackFeaturesToSG(ds_key, true).isFailure() ) 
      ATH_MSG_WARNING( "failed unpacking features from BS to SG for: " << ds_key  );
    else
      ATH_MSG_DEBUG( "success unpacking features from BS to SG for: " << ds_key  );
  }

  m_navigationForEF->reset();
  m_navigationForL2->reset();



  return StatusCode::SUCCESS;
}


StatusCode TrigBSExtraction::repackFeaturesToSG(const std::string& key, bool equalize ) {
  const HLT::HLTResult * constresult;
  HLT::HLTResult * result(0);

  // get HLT Result from SG for key
  //if (!m_storeGate->transientContains<HLT::HLTResult>(key)) {

  ATH_MSG_DEBUG( "trying to deserialzie content of " << key  );
  if (evtStore()->contains<HLT::HLTResult>(key)) {
    evtStore()->retrieve(constresult, key).ignore();
    result = const_cast<HLT::HLTResult*>(constresult);
    
    ATH_MSG_DEBUG("HLTResult is level="<<result->getHLTLevel()  );
    // reset navigation from previous event
    //    m_nav->reset();

    const std::vector<uint32_t>& navData = result->getNavigationResult();
    if ( navData.size() != 0 ) {
      ATH_MSG_DEBUG( "navigation payload obtained from  " << key << " has size " << navData.size()  );
      m_nav->deserialize( navData );
    } else {
      ATH_MSG_WARNING( "navigation payload obtained from  " << key << " has size 0"  );
    }

  } else {
    ATH_MSG_WARNING( "Trying to get HLT result, but not found with key " 
                     << key  );
  }

  if ( equalize ) {
    ATH_MSG_DEBUG( "Merging L2 and EF navigation structures " << key  );
    m_nav->merge(*m_navigationForL2);
    m_nav->prepare();  
  }
  bool status=true;

  //change the clid and subtype index in the
  //feature access helpers to the new xAOD containers (that will be converted after this call)
  std::vector< HLT::TriggerElement* > testvec;
  m_nav->getAll(testvec,false);
  ATH_MSG_DEBUG( "what's happening? " << testvec.size()  );
  
  // pack navigation back to the result
  result->getNavigationResult().clear();
  status = m_nav->serialize(result->getNavigationResult(), result->getNavigationResultCuts());
  ATH_MSG_DEBUG( "new serialized navigation has size " << result->getNavigationResult().size()  );

  //  m_nav->reset();
  if ( status )
    return StatusCode::SUCCESS;
  return StatusCode::FAILURE;
}

// little templated utility
template<class T>
bool TrigBSExtraction::repackFeature() {
  std::vector<const T*> v;
  ATH_MSG_VERBOSE( "unpacking features of CLID: " << ClassID_traits<T>::ID() 
                   << " name: " << ClassID_traits<T>::typeName()  );
  bool status = true;
  /*
  try {
    status = m_navigation->getAllFeatures(v);
  } catch ( const std::exception& ex) {
    log << MSG::WARNING << "Unpacking of the features of type: " << ClassID_traits<T>::typeName() << " caused exception: " << ex.what()  << endreq;
  }
  if ( status != true )
    log << MSG::WARNING << "Unpacking of the features of type: " << ClassID_traits<T>::typeName() << " failed" << endreq;

  */
  
  return status;
}
