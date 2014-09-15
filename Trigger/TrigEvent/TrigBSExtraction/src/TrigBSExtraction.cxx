/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBSExtraction/TrigBSExtraction.h"
#include "TrigSteeringEvent/HLTResult.h"

TrigBSExtraction::TrigBSExtraction(const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator),
    m_navigationForEF("HLT::Navigation/Navigation", this),
    m_navigationForL2("HLT::Navigation/NavigationForL2", this),
    m_storeGate("StoreGateSvc", name)
{
  
  declareProperty("L2ResultKey",     m_l2ResultKey = "HLTResult_L2");
  declareProperty("EFResultKey",     m_efResultKey = "HLTResult_EF");
  declareProperty("HLTResultKey",    m_hltResultKey = "HLTResult_HLT");
  declareProperty("Navigation",      m_navigationForEF);
  declareProperty("NavigationForL2",      m_navigationForL2);
  declareProperty("EvtStore", m_storeGate);
}
TrigBSExtraction::~TrigBSExtraction() {}

StatusCode TrigBSExtraction::initialize() {
  MsgStream log( msgSvc(), name());
  if ( m_navigationForEF.retrieve().isFailure() ) {
    log << MSG::FATAL << "failed to get navigation, can not retrieve navigation tool: "
	<< m_navigationForEF << endreq;
    return  StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "successfully retrieved navigation tool: "
	<< m_navigationForEF << endreq;
  }

  if ( m_navigationForL2.retrieve().isFailure() ) {
    log << MSG::FATAL << "failed to get navigation, can not retrieve navigation tool: "
	<< m_navigationForL2 << endreq;
    return  StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "successfully retrieved navigation tool: "
	<< m_navigationForL2 << endreq;
  }


  if( m_storeGate.retrieve().isFailure() ) {
    log << MSG::FATAL << "Unable to locate Service StoreGate!" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigBSExtraction::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode TrigBSExtraction::execute() {
  MsgStream log( msgSvc(), name());
  log << MSG::DEBUG <<"has keys: HLTResult L2="<< m_l2ResultKey <<" EF=" <<m_efResultKey<<" HLT="<<m_hltResultKey<<endreq;
  if ( m_l2ResultKey != "" ) {
    m_nav = &*m_navigationForL2;
    if ( repackFeaturesToSG(m_l2ResultKey,  (m_efResultKey == "") ).isFailure() ) 
      log << MSG::WARNING << "failed unpacking features from BS to SG for: " << m_l2ResultKey << endreq;
    else
      log << MSG::DEBUG << "success unpacking features from BS to SG for: " << m_l2ResultKey << endreq;
  }
  
  m_nav = &*m_navigationForEF;
  if ( m_efResultKey != "" ) {
    if ( repackFeaturesToSG(m_efResultKey, true).isFailure() ) 
      log << MSG::WARNING << "failed unpacking features from BS to SG for: " << m_efResultKey << endreq;
    else
      log << MSG::DEBUG << "success unpacking features from BS to SG for: " << m_efResultKey << endreq;
  }

  if ( m_hltResultKey != "" ) {
    if ( repackFeaturesToSG(m_hltResultKey, true).isFailure() ) 
      log << MSG::WARNING << "failed unpacking features from BS to SG for: " << m_hltResultKey << endreq;
    else
      log << MSG::DEBUG << "success unpacking features from BS to SG for: " << m_hltResultKey << endreq;
  }
  
  m_navigationForEF->reset();
  m_navigationForL2->reset();



  return StatusCode::SUCCESS;
}


StatusCode TrigBSExtraction::repackFeaturesToSG(const std::string& key, bool equalize ) {
  MsgStream log( msgSvc(), name());
  const HLT::HLTResult * constresult;
  HLT::HLTResult * result(0);

  // get HLT Result from SG for key
  //if (!m_storeGate->transientContains<HLT::HLTResult>(key)) {

  log << MSG::DEBUG << "trying to deserialzie content of " << key << endreq;
  if (m_storeGate->contains<HLT::HLTResult>(key)) {
    m_storeGate->retrieve(constresult, key).ignore();
    result = const_cast<HLT::HLTResult*>(constresult);
    
   log << MSG::DEBUG <<"HLTResult is level="<<result->getHLTLevel() << endreq; 
    // reset navigation from previous event
    //    m_nav->reset();

    const std::vector<uint32_t>& navData = result->getNavigationResult();
    if ( navData.size() != 0 ) {
      log << MSG::DEBUG << "navigation payload obtained from  " << key << " has size " << navData.size() << endreq;
      m_nav->deserialize( navData );
    } else {
      log << MSG::WARNING << "navigation payload obtained from  " << key << " has size 0" << endreq;
    }

  } else {
    log << MSG::WARNING << "Trying to get HLT result, but not found with key " 
	<< key << endreq;
  }

  

  if ( equalize ) {
    log << MSG::DEBUG << "Merging L2 and EF navigation structures " 
	<< key << endreq;
    m_nav->merge(*m_navigationForL2);
    m_nav->prepare();  
  }
  bool status=true;

  //change the clid and subtype index in the
  //feature access helpers to the new xAOD containers (that will be converted after this call)
  std::vector< HLT::TriggerElement* > testvec;
  m_nav->getAll(testvec,false);
  log << MSG::DEBUG << "what's happening? " << testvec.size() << endreq;
  
  // pack navigation back to the result
  result->getNavigationResult().clear();
  status = m_nav->serialize(result->getNavigationResult(), result->getNavigationResultCuts());
  log << MSG::DEBUG << "new serialized navigation has size " << result->getNavigationResult().size() << endreq;

  //  m_nav->reset();
  if ( status )
    return StatusCode::SUCCESS;
  return StatusCode::FAILURE;
}

// little templated utility
template<class T>
bool TrigBSExtraction::repackFeature() {
  MsgStream log( msgSvc(), name());
  std::vector<const T*> v;
  log << MSG::VERBOSE << "unpacking features of CLID: " << ClassID_traits<T>::ID() 
      << " name: " << ClassID_traits<T>::typeName() << endreq;  
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
