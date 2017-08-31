/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ALFA_RawDataProvider.cxx
//   Implementation file for class ALFA_RawDataProvider
///////////////////////////////////////////////////////////////////
// (c) ATLAS ALFA Detector software
///////////////////////////////////////////////////////////////////

#include "ALFA_RawDataByteStreamCnv/ALFA_RawDataProvider.h"

////////////////////////
// constructor
////////////////////////
ALFA_RawDataProvider::ALFA_RawDataProvider(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm         (name, pSvcLocator),
  m_robDataProvider("ROBDataProviderSvc",name),
  m_rawDataTool("ALFA_RawDataProviderTool"),
  m_ALFA_RawDataCollectionKey(),
  m_collection()
{
  declareProperty("ALFA_RawDataCollectionKey", m_ALFA_RawDataCollectionKey = "ALFA_RawData");
  declareProperty ("ProviderTool", m_rawDataTool);
}

////////////////////////
// destructor
////////////////////////
ALFA_RawDataProvider::~ALFA_RawDataProvider(){}

////////////////////////
// initialize() -
////////////////////////
StatusCode ALFA_RawDataProvider::initialize()
{

  msg(MSG::DEBUG) << "ALFA_RawDataProvider::initialize" << endmsg;

  // Get ROBDataProviderSvc
  if (m_robDataProvider.retrieve().isFailure())
    {
      msg(MSG::FATAL) << "Failed to retrieve service " << m_robDataProvider << endmsg;
      return StatusCode::FAILURE;
    } else
    msg(MSG::DEBUG) << "Retrieved service " << m_robDataProvider << endmsg;

  // Get ALFARawDataProviderTool
  if (m_rawDataTool.retrieve().isFailure())
    {
      msg(MSG::FATAL) << "Failed to retrieve service " << m_rawDataTool << endmsg;
      return StatusCode::FAILURE;
    } else
    msg(MSG::DEBUG) << "Retrieved service " << m_rawDataTool << endmsg;

  return StatusCode::SUCCESS;
}





////////////////////////
// execute() -
////////////////////////
StatusCode ALFA_RawDataProvider::execute()
{

  msg(MSG::DEBUG) << "ALFA_RawDataProvider::EXECUTE" << endmsg;


  ALFA_RawDataContainer *container = new ALFA_RawDataContainer();
  msg(MSG::DEBUG) << " Created ALFA RDO Container" << endmsg;

  StatusCode sc;

  sc = evtStore()->record(container, m_ALFA_RawDataCollectionKey);

  if (sc.isFailure())
    {
      msg(MSG::FATAL) << "Unable to record ALFA RDO Container" << endmsg;
      return StatusCode::FAILURE;
    } else if (sc.isSuccess()){
    msg(MSG::DEBUG) << "ALFA RDO Container recorded" << endmsg;
  }



  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> listOfRobf;
  std::vector<unsigned int> ROBIDs;
  ROBIDs.push_back(0x00840000);
  ROBIDs.push_back(0x00840001);

  m_robDataProvider->getROBData(ROBIDs, listOfRobf);


  msg(MSG::DEBUG) << "  ROB ID " << std::hex <<ROBIDs <<MSG::dec<< endmsg;

  msg(MSG::DEBUG) << " Number of ROB fragments is " << listOfRobf.size() << endmsg;

  // ask ALFA_RawDataProviderTool to decode it and to fill the container

  if (m_rawDataTool->convert(listOfRobf,container).isFailure())
    {
      msg(MSG::ERROR) << "BS conversion into RDOs failed" << endmsg;
      return StatusCode::RECOVERABLE;
    } else
    msg(MSG::DEBUG) << " Number of collections in container is " << container->size() << endmsg;


  // check retrieve from StoreGate ///////////////////////////////////////////////////////////

  sc = evtStore()->retrieve(container, m_ALFA_RawDataCollectionKey);
  if (sc.isSuccess())
    {
      msg(MSG::DEBUG) << "ALFA RDO Container retrieved" << endmsg;
    } // end check

  return StatusCode::SUCCESS;
}

