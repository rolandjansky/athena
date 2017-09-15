/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ALFA_RawDataProvider.cxx
//   Implementation file for class ALFA_RawDataProvider_charge
///////////////////////////////////////////////////////////////////
// (c) ATLAS ALFA Detector software
///////////////////////////////////////////////////////////////////

#include "ALFA_RawDataByteStreamCnv/ALFA_RawDataProvider_charge.h"

////////////////////////
// constructor
////////////////////////
ALFA_RawDataProvider_charge::ALFA_RawDataProvider_charge(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm (name, pSvcLocator),
  m_robDataProvider("ROBDataProviderSvc",name),
  m_rawDataTool_charge("ALFA_RawDataProviderTool_charge"),
  m_ALFA_RawDataCollectionKey_charge(),
  m_collection()
{
  declareProperty("ALFA_RawDataCollectionKey_charge", m_ALFA_RawDataCollectionKey_charge = "ALFA_RawData_charge");
  declareProperty ("ProviderTool_charge", m_rawDataTool_charge);
}

////////////////////////
// destructor
////////////////////////
ALFA_RawDataProvider_charge::~ALFA_RawDataProvider_charge(){
}

////////////////////////
// initialize() -
////////////////////////
StatusCode ALFA_RawDataProvider_charge::initialize() {

  msg(MSG::DEBUG) << "ALFA_RawDataProvider_charge::initialize" << endmsg;

  // Get ROBDataProviderSvc
  if (m_robDataProvider.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_robDataProvider << endmsg;
    return StatusCode::FAILURE;
  } else
    msg(MSG::DEBUG) << "Retrieved service " << m_robDataProvider << endmsg;

  // Get ALFARawDataProviderTool
  if (m_rawDataTool_charge.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_rawDataTool_charge << endmsg;
    return StatusCode::FAILURE;
  } else
    msg(MSG::DEBUG) << "Retrieved service " << m_rawDataTool_charge << endmsg;

  return StatusCode::SUCCESS;
}





////////////////////////
// execute() -
////////////////////////
StatusCode ALFA_RawDataProvider_charge::execute() {

msg(MSG::DEBUG) << "ALFA_RawDataProvider_charge::EXECUTE" << endmsg;


  ALFA_RawDataContainer_charge *container = new ALFA_RawDataContainer_charge();
   msg(MSG::DEBUG) << " Created ALFA RDO Container_charge" << endmsg;

 StatusCode sc;

 sc = evtStore()->record(container, m_ALFA_RawDataCollectionKey_charge);

  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Unable to record ALFA RDO Container_charge" << endmsg;
    return StatusCode::FAILURE;
   } else if (sc.isSuccess()){
  msg(MSG::DEBUG) << "ALFA RDO Container_charge recorded" << endmsg;
  }



  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> listOfRobf;
  std::vector<unsigned int> ROBIDs;
  ROBIDs.push_back(0x00840000);
  ROBIDs.push_back(0x00840001);

  m_robDataProvider->getROBData(ROBIDs, listOfRobf);


    msg(MSG::DEBUG) << "  ROB ID " << std::hex <<ROBIDs <<MSG::dec<< endmsg;

    msg(MSG::DEBUG) << " Number of ROB fragments is " << listOfRobf.size() << endmsg;

  // ask ALFA_RawDataProviderTool to decode it and to fill the container

 if (m_rawDataTool_charge->convert_charge(listOfRobf,container).isFailure()){
    msg(MSG::ERROR) << "BS conversion into RDOs failed" << endmsg;
} else
    msg(MSG::DEBUG) << " Number of collections in container is " << container->size() << endmsg;


  // check retrieve from StoreGate ///////////////////////////////////////////////////////////

  sc = evtStore()->retrieve(container, m_ALFA_RawDataCollectionKey_charge);
  if (sc.isSuccess())
  {
	msg(MSG::DEBUG) << "ALFA RDO Container retrieved" << endmsg;
  } // end check

  return StatusCode::SUCCESS;
}

