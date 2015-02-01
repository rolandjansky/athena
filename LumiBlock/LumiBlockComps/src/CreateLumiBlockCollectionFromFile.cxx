/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LumiBlockComps/CreateLumiBlockCollectionFromFile.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "StoreGate/StoreGateSvc.h"

CreateLumiBlockCollectionFromFile::CreateLumiBlockCollectionFromFile(const std::string& name, ISvcLocator* pSvcLocator) :
// ********************************************************************************************************************
  AthAlgorithm(name, pSvcLocator), m_lastRun(9999999), m_lastLumiBlock(9999999), m_lastIOVTime(0),
  m_metaStore("StoreGateSvc/MetaDataStore", name)
{
  declareProperty("LBCollName",m_LBColl_name = "LumiBlocks");
  declareProperty("unfinishedLBCollName",m_unfinishedLBColl_name = "IncompleteLumiBlocks");
  declareProperty("suspectLBCollName",m_suspectLBColl_name = "SuspectLumiBlocks");

  declareProperty("checkEventsExpected",m_checkEventsExpected=false);
}

StatusCode CreateLumiBlockCollectionFromFile::initialize(){
//*******************************************************

  ATH_MSG_INFO( "initialize() and create listeners" );

  declareProperty ("EvtStore", m_evtStoreName);
  declareProperty ("DetStore", m_detStoreName);
  declareProperty ("UserStore", m_userStoreName);
                                                                                 
  // Locate the StoreGateSvc and initialize our local ptr
  // ****************************************************
  ATH_CHECK( m_metaStore.retrieve() );

  // Set to be listener for end of event
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  ATH_CHECK( incSvc.retrieve() );
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

  ATH_MSG_DEBUG( "execute()" );

  // Check for event header
  const DataHandle<EventInfo> evt;
  ATH_CHECK( evtStore()->retrieve(evt) );
   if (!evt.isValid()) {
     ATH_MSG_FATAL( "Could not find event" );
      return(StatusCode::FAILURE);
   }
   ATH_MSG_DEBUG( "EventInfo event: " << evt->event_ID()->event_number() 
                  << " run: " << evt->event_ID()->run_number() 
                  << " lumi block: " << evt->event_ID()->lumi_block() );

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
         ATH_MSG_WARNING( "Database access to number of expected events not implemented" );
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

  ATH_MSG_INFO( "finalize()" );
  return StatusCode::SUCCESS;
}
 

//*********************************************************************
StatusCode CreateLumiBlockCollectionFromFile::fillLumiBlockCollection()
// ********************************************************************
{
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

  ATH_MSG_INFO( "Summary of LumiBlock Info:"  );
  if(piovComplete->size()>0) {
     ATH_MSG_INFO( "Complete LumiBlocks:" );
     piovComplete->dump(std::cout);
  }

  if(piovUnfinished->size()>0) {
    ATH_MSG_INFO( "Unfinished LumiBlocks:" );
    piovUnfinished->dump(std::cout);
  }

  if(piovSuspect->size()>0) {
    ATH_MSG_INFO( "Suspect LumiBlocks:" );
    piovSuspect->dump(std::cout);
  }

 

  // Store the LumiBlockCollection in the metadata store
  // =======================================================
  StatusCode sc;
  if(piovComplete->size()>0) {
    sc = m_metaStore->record(piovComplete, m_LBColl_name);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("could not register complete LumiBlockCollection");
    }
  }

  if(piovUnfinished->size()>0) {
    sc = m_metaStore->record(piovUnfinished,  m_unfinishedLBColl_name);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("could not register incomplete LumiBlockCollection");
    }
  }

  if(piovSuspect->size()>0) {
    sc = m_metaStore->record(piovSuspect, m_suspectLBColl_name);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("could not register suspect LumiBlockCollection");
    }
  }
  // Then clear m_LumiBlockInfo.  This is in case we decide to store the 
  // LBColl separately for each input or output file.
 
  m_LumiBlockInfo.clear();
  return StatusCode::SUCCESS;
}
// *******************************************************************
void CreateLumiBlockCollectionFromFile::handle(const Incident& inc) {
// ********************************************************************

  // this incident serves the same purpose as BeginInputFile
  if (inc.type() == "BeginInputFile") {
    const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
    std::string fileName;
    if (fileInc == 0) { fileName = "Undefined "; }
    else { fileName = fileInc->fileName();}
    ATH_MSG_DEBUG( "BeginInputFile: " << fileName );
    if(m_LumiBlockInfo.size()>0) {
      ATH_MSG_WARNING( " BeginInputFile handle detects non-zero size Cached LumiBlockColl" );
    }
  }
  else if(inc.type() == "EndInputFile") {
    // ***************************************************************************************************
    // We've hit EndInputFile, so we know that we have finished reading all the events for the LB
    // Therefore we can transfer the list of LB to the cached collection of good LB's
    // ***************************************************************************************************
    ATH_MSG_DEBUG( "EndInputFile incident detected" );
  }
  else if(inc.type() == "LastInputFile") {
    finishUp();
  }
  else {
    ATH_MSG_INFO( "Unknown Incident: " << inc.type() );
  }
 
}

// *******************************************************************
void CreateLumiBlockCollectionFromFile:: finishUp() {
// ********************************************************************

    ATH_MSG_INFO(  " finishUp: write lumiblocks to meta data store " );
    ATH_MSG_INFO(  "LumiBlockInfo.size= "<< m_LumiBlockInfo.size() );
    if(m_LumiBlockInfo.size()>0) {
      StatusCode sc=fillLumiBlockCollection();
        if (sc.isFailure()) {
          ATH_MSG_ERROR( "Could not fill lumiblock collections in finishUp()" );
        }
    }
}
