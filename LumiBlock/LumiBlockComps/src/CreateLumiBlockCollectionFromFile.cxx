/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LumiBlockComps/CreateLumiBlockCollectionFromFile.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODLuminosity/LumiBlockRangeContainer.h"
#include "xAODLuminosity/LumiBlockRangeAuxContainer.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoolKernel/IObject.h"
#include "CoralBase/Blob.h"


CreateLumiBlockCollectionFromFile::CreateLumiBlockCollectionFromFile(const std::string& name, ISvcLocator* pSvcLocator) :
// ********************************************************************************************************************
  AthAlgorithm(name, pSvcLocator), m_lastRun(9999999), m_lastLumiBlock(9999999),
  m_lastIOVTime(0), m_numExpected(0),m_metaStore("StoreGateSvc/MetaDataStore", name)
{
  declareProperty("streamName",m_streamName="");
  declareProperty("channelNumber",m_channel=0);
  declareProperty("LBCollName",m_LBColl_name = "LumiBlocks");
  declareProperty("unfinishedLBCollName",m_unfinishedLBColl_name = "IncompleteLumiBlocks");
  declareProperty("suspectLBCollName",m_suspectLBColl_name = "SuspectLumiBlocks");
  declareProperty("DBfolderName",m_folderName="/GLOBAL/FILECOUNT/PROMPT");
  declareProperty("checkEventsExpected",m_checkEventsExpected=true);
}

StatusCode CreateLumiBlockCollectionFromFile::initialize(){
//*******************************************************

  ATH_MSG_INFO( "initialize() and create listeners" );

  ATH_CHECK(m_eventInfoKey.initialize());

  // Locate the StoreGateSvc and initialize our local ptr
  // ****************************************************
  ATH_CHECK( m_metaStore.retrieve() );

  // Set to be listener for end of event
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  ATH_CHECK( incSvc.retrieve() );
  //incSvc->addListener(this, "BeginInputFile", 60); // pri has to be < 100 
                                                 // to be after MetaDataSvc.
  incSvc->addListener(this, "MetaDataStop", 50); // pri has to be > 20 to be 
                                                  // before MetaDataSvc and AthenaOutputStream.

  ServiceHandle<IIoComponentMgr> ioMgr("IoComponentMgr", this->name());
  ATH_CHECK( ioMgr.retrieve() );
  if (!ioMgr->io_register(this).isSuccess()) {
    ATH_MSG_FATAL("Could not register myself with the IoComponentMgr");
    return(StatusCode::FAILURE);
  }

  m_LumiBlockInfo.clear();

  const DataHandle<CondAttrListCollection> aptr;
  if(m_folderName.empty()) {
     ATH_MSG_INFO( "No folderName specified" );
  }
  else if(m_streamName.empty()) {
     ATH_MSG_INFO( "No streamName specified" );
  }
  else if (detStore()->contains<CondAttrListCollection>(m_folderName)) {
    CHECK(detStore()->regFcn(&CreateLumiBlockCollectionFromFile::updateCache, this , aptr, m_folderName));
  }
  else {
    ATH_MSG_FATAL( "detStore() does not contain folder " << m_folderName );
  }
  return StatusCode::SUCCESS;
}


StatusCode CreateLumiBlockCollectionFromFile::execute() {
//*******************************************************

  ATH_MSG_VERBOSE( "execute()" );

  // Check for event header
  SG::ReadHandle<xAOD::EventInfo> evt(m_eventInfoKey);

  // check is only useful for serial running; remove when MT scheduler used
  if(!evt.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve "<< m_eventInfoKey.key());
    return StatusCode::FAILURE;
  }

  if(m_lastRun!=evt->runNumber() ||
     m_lastLumiBlock!=evt->lumiBlock()) {

    IOVTime iovtime(evt->runNumber(),evt->lumiBlock());
    RLBMap::iterator mitr;
    mitr=m_LumiBlockInfo.find(iovtime);
    if (mitr==m_LumiBlockInfo.end()) {
      ATH_MSG_INFO( "Fill LumiBlockInfo with numExpected="<<m_numExpected );
      inOut lbInOut(m_numExpected,1);
      m_LumiBlockInfo[iovtime] = lbInOut;
    }
    else {
      m_LumiBlockInfo[iovtime].second++;
    }

    m_lastRun=evt->runNumber();
    m_lastLumiBlock=evt->lumiBlock();
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

  ATH_MSG_VERBOSE( "finalize()" );
  return StatusCode::SUCCESS;
}
 

//*********************************************************************
StatusCode CreateLumiBlockCollectionFromFile::fillLumiBlockCollection()
// ********************************************************************
{
  // Create the LumiBlockCollection

  xAOD::LumiBlockRangeContainer* piovComplete = new xAOD::LumiBlockRangeContainer();
  xAOD::LumiBlockRangeAuxContainer* piovCompleteAux = new xAOD::LumiBlockRangeAuxContainer();
  piovComplete->setStore( piovCompleteAux );

  xAOD::LumiBlockRangeContainer* piovUnfinished = new xAOD::LumiBlockRangeContainer();
  xAOD::LumiBlockRangeAuxContainer* piovUnfinishedAux = new xAOD::LumiBlockRangeAuxContainer();
  piovUnfinished->setStore( piovUnfinishedAux );

  xAOD::LumiBlockRangeContainer* piovSuspect = new xAOD::LumiBlockRangeContainer();
  xAOD::LumiBlockRangeAuxContainer* piovSuspectAux = new xAOD::LumiBlockRangeAuxContainer();
  piovSuspect->setStore( piovSuspectAux );

  for(RLBMap::iterator mitr=m_LumiBlockInfo.begin(); mitr!=m_LumiBlockInfo.end(); mitr++) {
    xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange();

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
    iovr->setStartRunNumber(mitr->first.run());
    iovr->setStartLumiBlockNumber(mitr->first.event());
    iovr->setStopRunNumber(mitr->first.run());
    iovr->setStopLumiBlockNumber(mitr->first.event());
    iovr->setEventsExpected(mitr->second.first);
    iovr->setEventsSeen(mitr->second.second);
  }

  if(piovComplete->size()>0) {
    ATH_MSG_INFO( "Number of Complete LumiBlocks:" << piovComplete->size() );
    xAOD::LumiBlockRangeContainer::const_iterator it;
    for(it=piovComplete->begin(); it!=piovComplete->end(); it++) {
      msg(MSG::INFO) << "\t [ ("
	  << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
	  << "):("
	  << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
	  << ") eventsSeen = " << (*it)->eventsSeen()
	  << ", eventsExpected = " << (*it)->eventsExpected()
	  << " ]"
	  << endmsg;
    }
  }

  if(piovUnfinished->size()>0) {
    ATH_MSG_INFO( "Number of Unfinished LumiBlocks:" << piovUnfinished->size() );
    xAOD::LumiBlockRangeContainer::const_iterator it;
    for(it=piovUnfinished->begin(); it!=piovUnfinished->end(); it++) {
      msg(MSG::INFO) << "\t [ ("
	  << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
	  << "):("
	  << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
	  << ") eventsSeen = " << (*it)->eventsSeen()
	  << ", eventsExpected = " << (*it)->eventsExpected()
	  << " ]"
	  << endmsg;
    }
  }

  if(piovSuspect->size()>0) {
    ATH_MSG_INFO( "Number of Suspect LumiBlocks:"  << piovSuspect->size() );
    xAOD::LumiBlockRangeContainer::const_iterator it;
    for(it=piovSuspect->begin(); it!=piovSuspect->end(); it++) {
      msg(MSG::INFO) << "\t [ ("
	  << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
	  << "):("
	  << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
	  << ") eventsSeen = " << (*it)->eventsSeen()
	  << ", eventsExpected = " << (*it)->eventsExpected()
	  << " ]"
	  << endmsg;
    }
  }
  

  // Store the LumiBlockCollection in the metadata store
  // =======================================================
  if(piovComplete->size()>0) {
    ATH_CHECK( m_metaStore->record( piovComplete, m_LBColl_name ) );
    ATH_CHECK( m_metaStore->record( piovCompleteAux, m_LBColl_name + "Aux." ) );
  }
  
  if(piovUnfinished->size()>0) {
    ATH_CHECK( m_metaStore->record( piovUnfinished, m_unfinishedLBColl_name ) );
    ATH_CHECK( m_metaStore->record( piovUnfinishedAux, m_unfinishedLBColl_name + "Aux." ) );
  }

  if(piovSuspect->size()>0) {
    ATH_CHECK( m_metaStore->record( piovSuspect, m_suspectLBColl_name ) );
    ATH_CHECK( m_metaStore->record( piovSuspectAux, m_suspectLBColl_name + "Aux." ) );
  }
  
  // Then clear m_LumiBlockInfo.  This is in case we decide to store the 
  // LBColl separately for each input or output file. 
  m_LumiBlockInfo.clear();
  return StatusCode::SUCCESS;
}
// *******************************************************************
void CreateLumiBlockCollectionFromFile::handle(const Incident& inc) {
// ********************************************************************

/* WB: commented this out because it looks redundant (27/11/2015)
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
  else*/ if(inc.type() == "MetaDataStop") {
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
    if(m_LumiBlockInfo.size()>0) {
      StatusCode sc=fillLumiBlockCollection();
        if (sc.isFailure()) {
          ATH_MSG_ERROR( "Could not fill lumiblock collections in finishUp()" );
        }
    }
}


StatusCode CreateLumiBlockCollectionFromFile::updateCache(IOVSVC_CALLBACK_ARGS) {

  // Check if we have anything to do
  // Shouldn't actually get a callback if this folder doesn't exist...

  ATH_MSG_INFO( "In updateCache callback" );
  m_numExpected = 0;

  if (m_folderName.empty()) return StatusCode::SUCCESS;
  if(m_streamName=="") return StatusCode::SUCCESS;
  ATH_MSG_INFO( "folder is not empty" );
  const CondAttrListCollection* attrListColl = 0;
  CHECK(detStore()->retrieve(attrListColl, m_folderName));

  CondAttrListCollection::const_iterator it = attrListColl->begin();
  CondAttrListCollection::const_iterator last  = attrListColl->end();
  CondAttrListCollection::const_iterator match  = attrListColl->end();
  for (; it != last; ++it) {
    if (attrListColl->name_size()>0) {
      CondAttrListCollection::name_const_iterator nitr=attrListColl->chanNamePair((*it).first);
      int chan = (*it).first;
      std::string theName = nitr->second;        
      ATH_MSG_INFO( "channel " << chan << " name " << theName);
      if(theName==m_streamName) {
        m_channel = chan;
        match=it;
        break;
      }         
    }
  }
  // Make sure we found it
  if (match == last) {
    ATH_MSG_WARNING( "Stream " << m_streamName << " not found in " << m_folderName << "!" );
    m_numExpected = 0;
    return StatusCode::SUCCESS;
  }

  // OK, get number of events
  std::ostringstream attrStr1;
  (*match).second.toOutputStream( attrStr1 );

  const coral::AttributeList& attrList = (*match).second;

  // Check data availability
  if (attrList["NEventRec"].isNull()) {
    ATH_MSG_WARNING( " NEventRec not in database. Set it to 0 " );
    return StatusCode::SUCCESS;
  }

  cool::Int32 nev = attrList["NEventRec"].data<cool::Int32>();
  m_numExpected = (uint32_t) nev;
  ATH_MSG_INFO( "database returns NEventRec=" << nev );

 return StatusCode::SUCCESS;
}


StatusCode CreateLumiBlockCollectionFromFile::io_reinit() {
  ATH_MSG_INFO("I/O reinitialization...");
  m_LumiBlockInfo.clear();

  return StatusCode::SUCCESS;
}
