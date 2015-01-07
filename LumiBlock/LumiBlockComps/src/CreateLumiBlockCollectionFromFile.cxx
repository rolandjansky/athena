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
// ********************************************************************************************************************
  Algorithm(name, pSvcLocator), m_lastRun(9999999), m_lastLumiBlock(9999999), m_lastIOVTime(0),
  m_storeGate(0), m_metaStore(0)
{
  declareProperty("LBCollName",m_LBColl_name = "LumiBlocks");
  declareProperty("unfinishedLBCollName",m_unfinishedLBColl_name = "IncompleteLumiBlocks");
  declareProperty("suspectLBCollName",m_suspectLBColl_name = "SuspectLumiBlocks");

  declareProperty("checkEventsExpected",m_checkEventsExpected=false);
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

  m_LumiBlockInfo.clear();

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

       IOVTime iovtime(evt->event_ID()->run_number(),evt->event_ID()->lumi_block());
       RLBMap::iterator mitr;
       mitr=m_LumiBlockInfo.find(iovtime);
       if (mitr==m_LumiBlockInfo.end()) {

       // Here is where we should access the database
       // ==============================================
       unsigned int expectedEvents = 0;
       if(m_checkEventsExpected) {
         log << MSG::WARNING << "Database access to number of expected events not implemented" << endreq;
         expectedEvents = 8;  // JUST FOR TESTING
       }
       inOut lbInOut(expectedEvents,1);
       m_LumiBlockInfo[iovtime] = lbInOut;
       }
       else {
         m_LumiBlockInfo[iovtime].second++;
       }
   
     m_lastRun=evt->event_ID()->run_number();
     m_lastLumiBlock=evt->event_ID()->lumi_block();
     m_lastIOVTime=iovtime;
   }
   else {
     m_LumiBlockInfo[m_lastIOVTime].second++;
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
 

//*********************************************************************
StatusCode CreateLumiBlockCollectionFromFile::fillLumiBlockCollection()
// ********************************************************************
{
  MsgStream log(msgSvc(), name());
  // Create the LumiBlockCollection
  LumiBlockCollection* piovComplete = new LumiBlockCollection();
  LumiBlockCollection* piovUnfinished = new LumiBlockCollection();
  LumiBlockCollection* piovSuspect = new LumiBlockCollection();


  for(RLBMap::iterator mitr=m_LumiBlockInfo.begin(); mitr!=m_LumiBlockInfo.end(); mitr++) {
    // Create our LB_IOVRange object
    // Looks a bit wierd, but our map is <iovTime, pair<unit32_t,uint32_t> >
    LB_IOVRange* iovr = new LB_IOVRange(mitr->first,mitr->first);
    iovr->setNumExpected(mitr->second.first);
    iovr->setNumSeen(mitr->second.second);


    // Decide which collection it goes into depending on whether the LB is complete
    // =============================================================================
    // Suspect LB's have more events read than the DB says should be there
    if(!m_checkEventsExpected || mitr->second.second == mitr->second.first) {
      piovComplete->push_back(iovr);
    }
    else if(mitr->second.second > mitr->second.first) {
      piovSuspect->push_back(iovr);
    }
    else {
      piovUnfinished->push_back(iovr);
    }
  }

  log << MSG::INFO << "Summary of LumiBlock Info:"  << endreq ;
  if(piovComplete->size()>0) {
    log << MSG::INFO << "Complete LumiBlocks:" << endreq;
     piovComplete->dump(std::cout);
  }

  if(piovUnfinished->size()>0) {
    log << MSG::INFO << "Unfinished LumiBlocks:" << endreq;
    piovUnfinished->dump(std::cout);
  }

  if(piovUnfinished->size()>0) {
    log << MSG::INFO << "Suspect LumiBlocks:" << endreq;
    piovSuspect->dump(std::cout);
  }

  /*

  // Store the LumiBlockCollection in the metadata store
  // =======================================================
  StatusCode sc;
  if(piovComplete->size()>0) {
    sc = m_metaStore->record(piovComplete, m_LBColl_name);
    if (sc.isFailure()) {
       log << MSG::ERROR << "could not register complete LumiBlockCollection" << endreq;
       return(StatusCode::FAILURE);
    }
  }

  if(piovUnfinished->size()>0) {
    sc = m_metaStore->record(piovUnfinished,  m_unfinishedLBColl_name);
    if (sc.isFailure()) {
       log << MSG::ERROR << "could not register incomplete  LumiBlockCollection" << endreq;
       return(StatusCode::FAILURE);
    }
  }

  if(piovSuspect->size()>0) {
    sc = m_metaStore->record(piovSuspect, m_suspectLBColl_name);
    if (sc.isFailure()) {
       log << MSG::ERROR << "could not register suspect LumiBlockCollection" << endreq;
       return(StatusCode::FAILURE);
    }
  }
  // Then clear m_LumiBlockInfo.  This is in case we decide to store the 
  // LBColl separately for each input or output file.
  */
  m_LumiBlockInfo.clear();
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
    if(m_LumiBlockInfo.size()>0) {
      log << MSG::WARNING << " BeginInputFile handle detects non-zero size Cached LumiBlockColl" << endreq;
    }
  }
  else if(inc.type() == "EndInputFile") {
    // ***************************************************************************************************
    // We've hit EndInputFile, so we know that we have finished reading all the events for the LB
    // Therefore we can transfer the list of LB to the cached collection of good LB's
    // ***************************************************************************************************
    log <<  MSG::DEBUG << "EndInputFile incident detected" << endreq;  
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
    if(m_LumiBlockInfo.size()>0) {
      StatusCode sc=fillLumiBlockCollection();
        if (sc.isFailure()) {
            log << MSG::ERROR << "Could not fill lumiblock collections in finishUp()" << endreq;
        }
    }
}
