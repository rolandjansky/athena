/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BCM_RawDataProvider.cxx
//   Implementation file for class BCM_RawDataProvider
///////////////////////////////////////////////////////////////////
// (c) ATLAS BCM Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//  Version 00-00-01 12/05/2008 Daniel Dobos
//  Version 00-00-02 19/05/2008 Daniel Dobos
//  Version 00-00-11 09/02/2009 Daniel Dobos
///////////////////////////////////////////////////////////////////

#include "BCM_RawDataByteStreamCnv/BCM_RawDataProvider.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "BCM_RawDataByteStreamCnv/BCM_RawDataProviderTool.h"

#include "InDetBCM_RawData/BCM_RDO_Container.h"

////////////////////////
// constructor
////////////////////////
BCM_RawDataProvider::BCM_RawDataProvider(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm         (name, pSvcLocator),
  m_robDataProvider ("ROBDataProviderSvc",name),
  m_rawDataTool     ("BCM_RawDataProviderTool")
{
  declareProperty ("RDOKey"      , m_RDO_Key = "BCM_RDOs");
  declareProperty ("ProviderTool", m_rawDataTool);
}

////////////////////////
// destructor
////////////////////////
BCM_RawDataProvider::~BCM_RawDataProvider(){
}

////////////////////////
// initialize() -
////////////////////////
StatusCode BCM_RawDataProvider::initialize() {

  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "BCM_RawDataProvider::initialize" << endreq;

  // Get ROBDataProviderSvc
  if (m_robDataProvider.retrieve().isFailure()) {
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Failed to retrieve service " << m_robDataProvider << endreq;
    return StatusCode::FAILURE;
  } else
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Retrieved service " << m_robDataProvider << endreq;
 
  // Get BCMRawDataProviderTool
  if (m_rawDataTool.retrieve().isFailure()) {
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Failed to retrieve service " << m_rawDataTool << endreq;
    return StatusCode::FAILURE;
  } else
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Retrieved service " << m_rawDataTool << endreq;
 
  return StatusCode::SUCCESS;
}

////////////////////////
// execute() -
////////////////////////
StatusCode BCM_RawDataProvider::execute() {

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Create BCM RDO Container" << endreq;
  BCM_RDO_Container *container = new BCM_RDO_Container();
  if (evtStore()->record(container, m_RDO_Key).isFailure()) {
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Unable to record BCM RDO Container" << endreq;
    return StatusCode::FAILURE;
  }

  // ask ROBDataProviderSvc for the vector of ROBFragment for all BCM ROBIDs
  // std::vector<const ROBFragment*> listOfRobf_all;
  std::vector<const ROBFragment*> listOfRobf;
  
  // std::vector<unsigned int> ROBIDs_all;
  std::vector<unsigned int> ROBIDs;
  // changed 0x0081000A -> 0x0081004A and 0x0081000C -> 0x0081004C
  /*
  // Run-1 IDs
  ROBIDs_all.push_back(0x0081000A);
  ROBIDs_all.push_back(0x0081000C);
  
  //Run-2 IDs
  ROBIDs_all.push_back(0x0081004A);
  ROBIDs_all.push_back(0x0081004C);
  */
  //Run-1 ID
  ROBIDs.push_back(0x0081000A);
  ROBIDs.push_back(0x0081000C);
  //Run-2 IDs
  ROBIDs.push_back(0x0081004A);
  ROBIDs.push_back(0x0081004C);
  
  //m_robDataProvider->getROBData(ROBIDs_all, listOfRobf_all);
  
  m_robDataProvider->getROBData(ROBIDs, listOfRobf);
  
  // For Run-1 if 4 fragment present select only the PRO BCM 
  /*
  if(  listOfRobf_all.size() == 4 ) {
    ROBIDs.push_back(0x0081000A);
    ROBIDs.push_back(0x0081000C);
  }
  */
  
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of ROB fragments is " << listOfRobf.size() << endreq;

  // ask BCM_RawDataProviderTool to decode it and to fill the container
  if (m_rawDataTool->convert(listOfRobf,container).isFailure())
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "BS conversion into RDOs failed" << endreq;

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of collections in container is " << container->size() << endreq;

  return StatusCode::SUCCESS;
}

