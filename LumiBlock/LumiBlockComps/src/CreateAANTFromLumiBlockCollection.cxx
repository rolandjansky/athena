/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LumiBlockComps/CreateAANTFromLumiBlockCollection.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "StoreGate/StoreGateSvc.h"

CreateAANTFromLumiBlockCollection::CreateAANTFromLumiBlockCollection(const std::string& name, ISvcLocator* pSvcLocator) :
// *****************************************************
  Algorithm(name, pSvcLocator)
{

  declareProperty("LBCollName",m_LBColl_name = "LumiBlocks");

   m_tempLBColl = new LumiBlockCollection();          // Temp storage while reading the file

}

StatusCode CreateAANTFromLumiBlockCollection::initialize(){
//*******************************************************

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endreq;
                                                                                 
  // Locate the StoreGateSvc and initialize our local ptr
  // ****************************************************
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (!sc.isSuccess() || 0 == m_storeGate) {
      log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
      return StatusCode::FAILURE;
  } 
  sc = service("StoreGateSvc/InputMetaDataStore", m_metaStore);
  if (!sc.isSuccess() || 0 == m_metaStore) {
      log << MSG::ERROR << "Could not find MetaDataStore" << endreq;
      return StatusCode::FAILURE;
  } 


  // Initialize histogram service
  sc = serviceLocator()->service("THistSvc", tHistSvc);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to retrieve pointer to THistSvc!" << endreq;
    return sc;
  }
  
  // Create MetaData TTree
  MetaDataTree = new TTree("MetaData","MetaData");
  sc=tHistSvc->regTree("/AANT/MetaData",MetaDataTree);
  if(sc.isFailure()){
    log << MSG::ERROR << "Cannot register MetaDataTree" << endreq;
  }
  // create branch for LumiBlockCollection
  MetaDataTree->Bronch("LumiBlockCollection_p1_LumiBlocks", "LumiBlockCollection_p1", &m_lbc);

  // Create UserData Tree
  UserDataTree = new TTree("UserData","UserData");
  sc=tHistSvc->regTree("/AANT/UserData",UserDataTree);
  if(sc.isFailure()){
    log << MSG::ERROR << "Cannot register UserDataTree" << endreq;
  }
  // create branch for LumiBlockCollection
  UserDataTree->Branch("vectord", &m_vectord);

  return StatusCode::SUCCESS;
}


StatusCode CreateAANTFromLumiBlockCollection::execute() {
//*******************************************************

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "execute()" << endreq;

  m_vectord->push_back(123.0);
  m_vectord->push_back(456.0);
  UserDataTree->Fill();  

  return (StatusCode::SUCCESS);
}


//*****************************************************
StatusCode CreateAANTFromLumiBlockCollection::clear() {
// *****************************************************

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "clear()" << endreq;

  m_vectord->clear();

  return StatusCode::SUCCESS;
}

//*****************************************************
StatusCode CreateAANTFromLumiBlockCollection::finalize() {
// *****************************************************

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "finalize()" << endreq;

  // Look for LB information on input store and transfer it to temporay cache
  // ===========================================================================
  const DataHandle<LumiBlockCollection> iovc;
  StatusCode sc = m_metaStore->retrieve(iovc, m_LBColl_name);
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Could not find LumiBlockColl in input metatdata store" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::DEBUG << " Found LumiBlockColl in input metadata store: " << endreq;
  }
  
  // Now cache it locally                                                                                            
  m_tempLBColl->reserve(m_tempLBColl->size()+iovc->size());
  LumiBlockCollection::const_iterator i(iovc->begin()), ie(iovc->end());
  while (i != ie) {m_tempLBColl->push_back(new IOVRange(*(*i++)));}
  
  // try to persistify it
  m_lbc = m_lbc_conv.createPersistent( m_tempLBColl, log);
  log << MSG::INFO << "Hello World! m_lbs: " << m_lbc << endreq;

  MetaDataTree->Fill();

  sc=StatusCode::SUCCESS;

  return sc;
}
 

