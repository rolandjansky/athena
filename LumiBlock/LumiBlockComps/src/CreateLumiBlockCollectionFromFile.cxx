/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LumiBlockComps/CreateLumiBlockCollectionFromFile.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "StoreGate/StoreGateSvc.h"

CreateLumiBlockCollectionFromFile::CreateLumiBlockCollectionFromFile(const std::string& name, ISvcLocator* pSvcLocator) :
// *****************************************************
  Algorithm(name, pSvcLocator), m_lastRun(9999999), m_lastLumiBlock(9999999),
  m_storeGate(0), m_metaStore(0)
{
  declareProperty("LBCollName",m_LBColl_name = "LumiBlocks");
  declareProperty("unfinishedLBCollName",m_unfinishedLBColl_name = "IncompleteLumiBlocks");
}

StatusCode CreateLumiBlockCollectionFromFile::initialize(){
//*******************************************************

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize() and create listeners" << endreq;
                                                                                 
  // Locate the StoreGateSvc and initialize our local ptr
  // ****************************************************
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (!sc.isSuccess() || 0 == m_storeGate) {
      log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
      return StatusCode::FAILURE;
  } 
  sc = service("StoreGateSvc/MetaDataStore", m_metaStore);
  if (!sc.isSuccess() || 0 == m_metaStore) {
      log << MSG::ERROR << "Could not find MetaDataStore" << endreq;
      return StatusCode::FAILURE;
  } 

  // Set to be listener for end of event
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  sc = incSvc.retrieve();
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Unable to get the IncidentSvc" << endreq;
    return(sc);
  }
  incSvc->addListener(this, "BeginInputFile", 60); // pri has to be < 100 
                                                 // to be after MetaDataSvc.
  incSvc->addListener(this, "EndInputFile", 50); // pri has to be > 10 to be 
                                               //before MetaDataSvc.
  incSvc->addListener(this, "LastInputFile", 50); // pri has to be > 20 to be 
                                                  // before MetaDataSvc and AthenaOutputStream.

  m_LumiBlockColl.clear();
  m_cacheLBColl.clear();

  return StatusCode::SUCCESS;
}


StatusCode CreateLumiBlockCollectionFromFile::execute() {
//*******************************************************

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "execute()" << endreq;

  // Check for event header
  const DataHandle<EventInfo> evt;
  StatusCode sc = m_storeGate->retrieve(evt);
  if (sc.isFailure()) {
      log << MSG::FATAL << "Could not find event" << endreq;
      return(StatusCode::FAILURE);
  }
   if (!evt.isValid()) {
      log << MSG::FATAL << "Could not find event" << endreq;
      return(StatusCode::FAILURE);
   }
   log << MSG::DEBUG << "EventInfo event: " << evt->event_ID()->event_number() 
       << " run: " << evt->event_ID()->run_number() 
       << " lumi block: " << evt->event_ID()->lumi_block() 
       << endreq;

   if(m_lastRun!=evt->event_ID()->run_number() || 
      m_lastLumiBlock!=evt->event_ID()->lumi_block()) { 
       IOVTime iov(evt->event_ID()->run_number(),evt->event_ID()->lumi_block());
       m_LumiBlockColl.push_back(iov);
       m_lastRun=evt->event_ID()->run_number();
       m_lastLumiBlock=evt->event_ID()->lumi_block();
   }
  
  return (StatusCode::SUCCESS);
}

//*****************************************************
StatusCode CreateLumiBlockCollectionFromFile::finalize() {
// *****************************************************

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
  return sc;
}
 

//**********************************************************************
StatusCode CreateLumiBlockCollectionFromFile::fillLumiBlockCollection(std::vector<IOVTime>* coll, std::string key )
// *********************************************************************
{
  MsgStream log(msgSvc(), name());
  // Create the LumiBlockCollection
  LumiBlockCollection* piovc = new LumiBlockCollection();
  // Sort the collection and then remove duplicates
  sort(coll->begin(),coll->end());
  coll->erase( unique( coll->begin(), coll->end() ), coll->end() );

  // Iterate through the collection of IOV and create LumiBlockCollection 
  // Note:  the IOVTime contains run and lumiblock even though the method
  // names are run() and event()
  std::vector<IOVTime>::iterator it, ibeg, iend;
  ibeg = coll->begin();
  iend = ibeg;
  for(it=coll->begin(); it<coll->end(); it++) {
    // If run number changes or lumiblock number is not contiguous, make new
    // IOVRange object
    if(it->run()!=iend->run() || it->event()>(iend->event()+1)) {
      IOVRange* iov = new IOVRange(IOVTime(ibeg->run(),ibeg->event()),
                     IOVTime(iend->run(),iend->event()));
      piovc->push_back(iov);
      ibeg=it;
    }
    iend=it;
  } 
  // We need to store the last one since it is left hanging by the above loop
  IOVRange* iov = new IOVRange(IOVTime(ibeg->run(),ibeg->event()),
                     IOVTime(iend->run(),iend->event()));
  piovc->push_back(iov);
  log << MSG::INFO << "Summary of LumiBlock Info:"  << endreq ;
  piovc->dump(std::cout);

  // Store the LumiBlockCollection in the metadata store

  StatusCode sc = m_metaStore->record(piovc, key);
  if (sc.isFailure()) {
     log << MSG::ERROR << "could not register LumiBlockCollection" << endreq;
     return(StatusCode::FAILURE);
   }   

  // Then clear m_LumiBlockColl.  This is in case we decide to store the 
  // LBColl separately for each input or output file.
  coll->clear();
  return StatusCode::SUCCESS;
}
// *******************************************************************
void CreateLumiBlockCollectionFromFile::handle(const Incident& inc) {
// ********************************************************************
  MsgStream log(msgSvc(), name());

  // this incident serves the same purpose as BeginInputFile
  if (inc.type() == "BeginInputFile") {
    const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
    std::string fileName;
    if (fileInc == 0) { fileName = "Undefined "; }
    else { fileName = fileInc->fileName();}
    log <<  MSG::DEBUG << "BeginInputFile: " << fileName << endreq;
    if(m_LumiBlockColl.size()>0) {
      log << MSG::WARNING << " BeginInputFile handle detects non-zero size Cached LumiBlockColl" << endreq;
    }
  }
  else if(inc.type() == "EndInputFile") {
    // ***************************************************************************************************
    // We've hit EndInputFile, so we know that we have finished reading all the events for the LB
    // Therefore we can transfer the list of LB to the cached collection of good LB's
    // ***************************************************************************************************
    log <<  MSG::DEBUG << "EndInputFile incident detected" << endreq;  
    std::vector<IOVTime>::iterator it, ibeg, iend;
    ibeg = m_LumiBlockColl.begin();
    iend = ibeg;
    for(it=m_LumiBlockColl.begin(); it<m_LumiBlockColl.end(); it++) {
      m_cacheLBColl.push_back(*it);
    }
    m_LumiBlockColl.clear();
  }
  else if(inc.type() == "LastInputFile") {
    finishUp();
  }
  else {
    log <<  MSG::INFO << "Unknown Incident: " << inc.type() << endreq;
  }
 
}

// *******************************************************************
void CreateLumiBlockCollectionFromFile:: finishUp() {
// ********************************************************************

   MsgStream log(msgSvc(), name());
   log << MSG::INFO <<  " finishUp: write lumiblocks to meta data store " << endreq;

    if(m_cacheLBColl.size()>0) {
        StatusCode sc=fillLumiBlockCollection(&m_cacheLBColl,m_LBColl_name);
        if (sc.isFailure()) {
            log << MSG::ERROR << "Could not fill lumiblock collection for full files" << endreq;
        }
    }
    if(m_LumiBlockColl.size()>0) {
      StatusCode sc=fillLumiBlockCollection(&m_LumiBlockColl,m_unfinishedLBColl_name);
        if (sc.isFailure()) {
            log << MSG::ERROR << "Could not fill lumiblock collection for unfinished files" << endreq;
        }
    }
}
