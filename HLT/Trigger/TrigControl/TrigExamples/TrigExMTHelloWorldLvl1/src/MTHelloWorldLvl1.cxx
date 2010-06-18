/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigExMTHelloWorldLvl1/MTHelloWorldLvl1.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "TrigT1Result/RoIBResult.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"

// static const AlgFactory<MTHelloWorldLvl1>    Factory;
// const IAlgFactory& MTHelloWorldLvl1Factory = Factory;

/////////////////////////////////////////////////////////////////////////////

MTHelloWorldLvl1::MTHelloWorldLvl1(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator), m_StoreGateSvc(0), m_pIROBDataProviderSvc(0), 
  m_retrieveLvl1(false), m_retrieveROBs(false)
{
  // Declare the properties
  declareProperty("RetrieveLvl1", m_retrieveLvl1);
  declareProperty("RetrieveROBs", m_retrieveROBs);
  declareProperty("RobId",        m_listRobIds);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHelloWorldLvl1::initialize(){

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endreq;

#ifdef ATLAS_GAUDI_V21
  SmartIF<IService> tmp_msgSvc(msgSvc());
  if(tmp_msgSvc.isValid()) {
    log << MSG::INFO << " Algorithm = " << name() << " is connected to Message Service = "
	<< tmp_msgSvc->name() << endreq;
  }
#else
  Service* tmp_msgSvc = dynamic_cast<Service*> (msgSvc());
  if(tmp_msgSvc != 0) {
    log << MSG::INFO << " Algorithm = " << name() << " is connected to Message Service = "
	<< tmp_msgSvc->name() << endreq;
  }
#endif

  // Print out the property values
  log << MSG::INFO << " RetrieveLvl1 = " << m_retrieveLvl1 << endreq;
  log << MSG::INFO << " RetrieveROBs = " << m_retrieveROBs << endreq;
  if (m_retrieveROBs) {
    log << MSG::INFO << " Number of ROBs to access = " << m_listRobIds.value().size() << endreq;                                      
    log << MSG::INFO << " List of ROBs: " << endreq;                                   
    std::vector<int>::const_iterator it = m_listRobIds.value().begin();
    while (it != m_listRobIds.value().end())
      {
	m_robIds.push_back(*it);
	log << MSG::DEBUG << " 0x" << MSG::hex << *it << endreq;
	++it;
      }
  }  

  // Part 3: Locate the StoreGateSvc
  StatusCode sc = service("StoreGateSvc", m_StoreGateSvc);
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
    return sc;
  } else {
    log << MSG::INFO << " Algorithm = " << name() << " is connected to StoreGate Service          = "
        << m_StoreGateSvc->name() << endreq;
  }

  // Part 4: Locate the ROBDataProviderSvc
  sc = service("ROBDataProviderSvc", m_pIROBDataProviderSvc);
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Could not find ROBDataProviderSvc" << endreq;
    return sc;
  } else {
    IService* svc = dynamic_cast<IService*>(m_pIROBDataProviderSvc);
    if(svc != 0 ) {
      log << MSG::INFO << " Algorithm = " << name() << " is connected to ROBDataProviderSvc Service = "
	  << svc->name() << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHelloWorldLvl1::execute() {

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "execute()" << endreq;

  log << MSG::DEBUG << " StoreGate structure: Begin execute " << name() << "\n"
      << m_StoreGateSvc->dump() << endreq;

  // Get the Lvl1 Id
  const DataHandle<ROIB::RoIBResult> dobj;
  if (m_retrieveLvl1) {
    StatusCode sc = m_StoreGateSvc->retrieve(dobj,"RoIBResult") ;
    if ( sc.isFailure() ) {
      log << MSG::ERROR << "Could not find Lvl1Result" << endreq;
      return sc;
    } else {
      log << MSG::INFO << " ---> Lvl1Result ID " << dobj->cTPResult().header().L1ID() << endreq;
    }
  }

  // Get the ROBs
  if (m_retrieveROBs) {
    // preload ROBs in memory
    m_pIROBDataProviderSvc->addROBData(m_robIds);

    // retrieve ROBs
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> robFragments;
    m_pIROBDataProviderSvc->getROBData(m_robIds, robFragments) ;
    log << MSG::INFO << " ---> number of ROBs retrieved " << robFragments.size() << endreq;
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>::const_iterator it = robFragments.begin();
    log << MSG::INFO << " ---> List of ROBs found: " << endreq;                                   
    while (it != robFragments.end())
      {
	log << MSG::INFO << " ---> ROB ID = 0x" << MSG::hex << (*it)->rod_source_id() << MSG::dec 
	    << " Level-1 ID = " << (*it)->rod_lvl1_id() << endreq;
	++it;
      }
  }

  log << MSG::DEBUG << " StoreGate structure: End execute " << name() << "\n"
      << m_StoreGateSvc->dump() << endreq;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHelloWorldLvl1::finalize() {

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endreq;

  return StatusCode::SUCCESS;
}
