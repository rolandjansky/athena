/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CreateLumiBlockCollectionFromFile.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODLuminosity/LumiBlockRangeContainer.h"
#include "xAODLuminosity/LumiBlockRangeAuxContainer.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoolKernel/IObject.h"

#include <memory>

CreateLumiBlockCollectionFromFile::CreateLumiBlockCollectionFromFile(const std::string& name, ISvcLocator* pSvcLocator)
// ********************************************************************************************************************
  : AthAlgorithm(name, pSvcLocator)
  , m_lastRun(9999999)
  , m_lastLumiBlock(9999999)
  , m_lastIOVTime(0)
  , m_metaStore("StoreGateSvc/MetaDataStore", name)
{
}

StatusCode CreateLumiBlockCollectionFromFile::initialize()
//*******************************************************
{
  ATH_MSG_INFO( "initialize() and create listeners" );

  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_rchk.initialize(!m_streamName.empty()));

  ATH_CHECK( m_metaStore.retrieve() );

  // Set to be listener for MetaDataStop
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  ATH_CHECK( incSvc.retrieve() );
  incSvc->addListener(this, "MetaDataStop", 50); // pri has to be > 20 to be 
                                                 // before MetaDataSvc and AthenaOutputStream.

  ServiceHandle<IIoComponentMgr> ioMgr("IoComponentMgr", this->name());
  ATH_CHECK( ioMgr.retrieve() );
  if (!ioMgr->io_register(this).isSuccess()) {
    ATH_MSG_FATAL("Could not register myself with the IoComponentMgr");
    return(StatusCode::FAILURE);
  }
  
  m_LumiBlockInfo.clear();

  if(m_streamName.empty()) {
    ATH_MSG_INFO( "No streamName specified" );
  }

  return StatusCode::SUCCESS;
}


StatusCode CreateLumiBlockCollectionFromFile::execute()
//*******************************************************
{
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
      uint32_t nEvents = getNEventsFromDb();
      ATH_MSG_INFO( "Fill LumiBlockInfo with NEvents="<<nEvents);
      inOut lbInOut(nEvents,1);
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
  
  return StatusCode::SUCCESS;
}

//*****************************************************
StatusCode CreateLumiBlockCollectionFromFile::finalize()
// *****************************************************
{
  ATH_MSG_VERBOSE( "finalize()" );
  return StatusCode::SUCCESS;
}
 

//*********************************************************************
StatusCode CreateLumiBlockCollectionFromFile::fillLumiBlockCollection()
// ********************************************************************
{
  // Create the LumiBlockCollection

  std::unique_ptr<xAOD::LumiBlockRangeContainer> piovComplete = std::make_unique<xAOD::LumiBlockRangeContainer>();
  std::unique_ptr<xAOD::LumiBlockRangeAuxContainer> piovCompleteAux = std::make_unique<xAOD::LumiBlockRangeAuxContainer>();
  piovComplete->setStore( piovCompleteAux.get() );

  std::unique_ptr<xAOD::LumiBlockRangeContainer> piovUnfinished = std::make_unique<xAOD::LumiBlockRangeContainer>();
  std::unique_ptr<xAOD::LumiBlockRangeAuxContainer> piovUnfinishedAux = std::make_unique<xAOD::LumiBlockRangeAuxContainer>();
  piovUnfinished->setStore( piovUnfinishedAux.get() );

  std::unique_ptr<xAOD::LumiBlockRangeContainer> piovSuspect = std::make_unique<xAOD::LumiBlockRangeContainer>();
  std::unique_ptr<xAOD::LumiBlockRangeAuxContainer> piovSuspectAux = std::make_unique<xAOD::LumiBlockRangeAuxContainer>();
  piovSuspect->setStore( piovSuspectAux.get() );

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
      ATH_MSG_INFO("\t [ ("
		   << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
		   << "):("
		   << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
		   << ") eventsSeen = " << (*it)->eventsSeen()
		   << ", eventsExpected = " << (*it)->eventsExpected()
		   << " ]");
    }
  }

  if(piovUnfinished->size()>0) {
    ATH_MSG_INFO( "Number of Unfinished LumiBlocks:" << piovUnfinished->size() );
    xAOD::LumiBlockRangeContainer::const_iterator it;
    for(it=piovUnfinished->begin(); it!=piovUnfinished->end(); it++) {
      ATH_MSG_INFO("\t [ ("
		   << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
		   << "):("
		   << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
		   << ") eventsSeen = " << (*it)->eventsSeen()
		   << ", eventsExpected = " << (*it)->eventsExpected()
		   << " ]");
    }
  }

  if(piovSuspect->size()>0) {
    ATH_MSG_INFO( "Number of Suspect LumiBlocks:"  << piovSuspect->size() );
    xAOD::LumiBlockRangeContainer::const_iterator it;
    for(it=piovSuspect->begin(); it!=piovSuspect->end(); it++) {
      ATH_MSG_INFO("\t [ ("
		   << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
		   << "):("
		   << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
		   << ") eventsSeen = " << (*it)->eventsSeen()
		   << ", eventsExpected = " << (*it)->eventsExpected()
		   << " ]");
    }
  }

  // Store the LumiBlockCollection in the metadata store
  // =======================================================
  if(piovComplete->size()>0) {
    ATH_CHECK( m_metaStore->record( std::move(piovComplete), m_LBColl_name ) );
    ATH_CHECK( m_metaStore->record( std::move(piovCompleteAux), m_LBColl_name + "Aux." ) );
  }
  
  if(piovUnfinished->size()>0) {
    ATH_CHECK( m_metaStore->record( std::move(piovUnfinished), m_unfinishedLBColl_name ) );
    ATH_CHECK( m_metaStore->record( std::move(piovUnfinishedAux), m_unfinishedLBColl_name + "Aux." ) );
  }

  if(piovSuspect->size()>0) {
    ATH_CHECK( m_metaStore->record( std::move(piovSuspect), m_suspectLBColl_name ) );
    ATH_CHECK( m_metaStore->record( std::move(piovSuspectAux), m_suspectLBColl_name + "Aux." ) );
  }
  
  // Then clear m_LumiBlockInfo.  This is in case we decide to store the 
  // LBColl separately for each input or output file. 
  m_LumiBlockInfo.clear();
  return StatusCode::SUCCESS;
}

// *******************************************************************
void CreateLumiBlockCollectionFromFile::handle(const Incident& inc)
// ********************************************************************
{
  if(inc.type() == "MetaDataStop") {
    ATH_MSG_INFO(  " finishUp: write lumiblocks to meta data store " );
    if(m_LumiBlockInfo.size()>0) {
      if(fillLumiBlockCollection().isFailure()) {
	ATH_MSG_ERROR( "Could not fill lumiblock collections" );
      }
    }
  }
  else {
    ATH_MSG_INFO( "Unknown Incident: " << inc.type() );
  }
}

uint32_t CreateLumiBlockCollectionFromFile::getNEventsFromDb()
{
  uint32_t nEvents{0};

  if (m_streamName.empty()) return nEvents;

  SG::ReadCondHandle<CondAttrListCollection> attrListColl(m_rchk);

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
        match=it;
        break;
      }         
    }
  }
  // Make sure we found it
  if (match == last) {
    ATH_MSG_WARNING( "Stream " << m_streamName << " not found in /GLOBAL/FILECOUNT/PROMPT !" );
    return nEvents;
  }

  // OK, get number of events
  const coral::AttributeList& attrList = (*match).second;

  // Check data availability
  if (attrList["NEventRec"].isNull()) {
    ATH_MSG_WARNING( " NEventRec not in database. Set it to 0 " );
    return nEvents;
  }

  cool::Int32 nev = attrList["NEventRec"].data<cool::Int32>();
  nEvents = (uint32_t) nev;
  ATH_MSG_INFO( "database returns NEventRec=" << nev );

  return nEvents;
}


StatusCode CreateLumiBlockCollectionFromFile::io_reinit() {
  ATH_MSG_INFO("I/O reinitialization...");
  m_LumiBlockInfo.clear();

  return StatusCode::SUCCESS;
}
