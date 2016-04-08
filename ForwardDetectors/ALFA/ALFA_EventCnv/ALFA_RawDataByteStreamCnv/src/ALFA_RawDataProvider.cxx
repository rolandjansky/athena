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
  m_nRawDataCollection(0),
  m_nRawData(0),
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

  msg(MSG::DEBUG) << "ALFA_RawDataProvider::initialize" << endreq;

  // Get ROBDataProviderSvc
  if (m_robDataProvider.retrieve().isFailure())
    {
      msg(MSG::FATAL) << "Failed to retrieve service " << m_robDataProvider << endreq;
      return StatusCode::FAILURE;
    } else
    msg(MSG::DEBUG) << "Retrieved service " << m_robDataProvider << endreq;

  // Get ALFARawDataProviderTool
  if (m_rawDataTool.retrieve().isFailure())
    {
      msg(MSG::FATAL) << "Failed to retrieve service " << m_rawDataTool << endreq;
      return StatusCode::FAILURE;
    } else
    msg(MSG::DEBUG) << "Retrieved service " << m_rawDataTool << endreq;

  return StatusCode::SUCCESS;
}





////////////////////////
// execute() -
////////////////////////
StatusCode ALFA_RawDataProvider::execute()
{

  msg(MSG::DEBUG) << "ALFA_RawDataProvider::EXECUTE" << endreq;


  ALFA_RawDataContainer *container = new ALFA_RawDataContainer();
  msg(MSG::DEBUG) << " Created ALFA RDO Container" << endreq;

  StatusCode sc;

  sc = evtStore()->record(container, m_ALFA_RawDataCollectionKey);

  if (sc.isFailure())
    {
      msg(MSG::FATAL) << "Unable to record ALFA RDO Container" << endreq;
      return StatusCode::FAILURE;
    } else if (sc.isSuccess()){
    msg(MSG::DEBUG) << "ALFA RDO Container recorded" << endreq;
  }



  std::vector<const ROBFragment*> listOfRobf;
  std::vector<unsigned int> ROBIDs;
  ROBIDs.push_back(0x00840000);
  ROBIDs.push_back(0x00840001);

  m_robDataProvider->getROBData(ROBIDs, listOfRobf);


  msg(MSG::DEBUG) << "  ROB ID " << std::hex <<ROBIDs <<MSG::dec<< endreq;

  msg(MSG::DEBUG) << " Number of ROB fragments is " << listOfRobf.size() << endreq;

  // ask ALFA_RawDataProviderTool to decode it and to fill the container

  if (m_rawDataTool->convert(listOfRobf,container).isFailure())
    {
      msg(MSG::ERROR) << "BS conversion into RDOs failed" << endreq;
      return StatusCode::RECOVERABLE;
    } else
    msg(MSG::DEBUG) << " Number of collections in container is " << container->size() << endreq;


  // check retrieve from StoreGate ///////////////////////////////////////////////////////////

  sc = evtStore()->retrieve(container, m_ALFA_RawDataCollectionKey);
  if (sc.isSuccess())
    {
      msg(MSG::DEBUG) << "ALFA RDO Container retrieved" << endreq;

/*
      ALFA_RawDataContainer::const_iterator RawData_Collection_Beg = container->begin();
      ALFA_RawDataContainer::const_iterator RawData_Collection_End = container->end();

      //int MBId_prova;

      //loop over collection (container) with hits (i.e. over 1 event)
      for(;RawData_Collection_Beg!=RawData_Collection_End;++RawData_Collection_Beg)
	{

	  //MBId_prova =(*RawData_Collection_Beg)->GetMBId_POT();

	  //msg(MSG::DEBUG)  << " MBId = " << MBId_prova <<  endreq;


	  ALFA_RawDataCollection::const_iterator p_RawData_Beg=(*RawData_Collection_Beg)->begin();//pointer to RawData data vector
	  ALFA_RawDataCollection::const_iterator p_RawData_End=(*RawData_Collection_Beg)->end();

	  //int PMFId_prova;
	  //int FiberNum_prova;


	  // Loop over RawData   -> apparently this is not used anymore

	  for(;p_RawData_Beg!=p_RawData_End; ++p_RawData_Beg)
	    {

	      if (((*p_RawData_Beg)->GetWordId_PMF())==3)
		{
		  //PMFId_prova = ((*p_RawData_Beg)->GetPMFId_PMF());
		  //msg(MSG::DEBUG)  << " PMFId = " << ((*p_RawData_Beg)->GetPMFId_PMF()) <<  endreq;

		} else {PMFId_prova = 100;}

	      for (unsigned int i=0;i<16;i++)
		{
		  //FiberNum_prova = ((*p_RawData_Beg)->GetWordId_PMF()*16+i);

		  //if((*p_RawData_Beg)->HitChan()[i]!=100) msg(MSG::DEBUG)  <<"    hit = " << (*p_RawData_Beg)-> HitChan()[i]<< endreq;
		}


	    } // loop raw data

	}// loop collection
	*/
    } // end check

  return StatusCode::SUCCESS;
}

