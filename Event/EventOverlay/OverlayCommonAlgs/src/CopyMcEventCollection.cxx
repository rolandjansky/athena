/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2008
// Ketevi A. Assamagan, October 2009

#include "CopyMcEventCollection.h"
#include "GeneratorObjects/McEventCollection.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "GeneratorObjects/HijingEventParams.h"
//#include "IOVDbDataModel/IOVMetaDataContainer.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"
//#include "xAODEventInfo/EventInfoContainer.h"
//#include "xAODEventInfo/EventInfoAuxContainer.h"

#include <iostream>
#include <typeinfo>

//================================================================
CopyMcEventCollection::CopyMcEventCollection(const std::string &name, ISvcLocator *pSvcLocator) :
  OverlayAlgBase(name, pSvcLocator),
  m_storeGateData2("StoreGateSvc/OriginalEvent2_SG", name),
  m_cnvTool( "xAODMaker::EventInfoCnvTool/EventInfoCnvTool", this )
{
  declareProperty("InfoType", m_infoType="MyEvent");
  declareProperty("RealData", m_realdata=false);
  declareProperty("DataStore2", m_storeGateData2, "help");
  declareProperty("CheckEventNumbers", m_checkeventnumbers=true);
  declareProperty( "CnvTool", m_cnvTool );
}

//================================================================
StatusCode CopyMcEventCollection::overlayInitialize()
{

  if (m_storeGateData2.retrieve().isFailure()) {
    ATH_MSG_FATAL("OverlayAlgBase::initialize): StoreGate[data2] service not found !");
    return StatusCode::FAILURE;
  } 

  CHECK( m_cnvTool.retrieve() );//get the conversion tool for making xAOD::EventInfo

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode CopyMcEventCollection::overlayFinalize() 
{
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode CopyMcEventCollection::overlayExecute() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "CopyMcEventCollection::execute() begin"<< endmsg;

  if (m_realdata){

  //
  // Copy EventInfo stuff from MC to data...
  //

  const EventInfo* mcEvtInfo = 0;
  if (m_storeGateMC->retrieve(mcEvtInfo).isSuccess() ) {
    log << MSG::INFO
	<< "Got EventInfo from MC store: " 
	<< " event " << mcEvtInfo->event_ID()->event_number() 
	<< " run " << mcEvtInfo->event_ID()->run_number()
	<< " timestamp " << mcEvtInfo->event_ID()->time_stamp()
	<< " lbn " << mcEvtInfo->event_ID()->lumi_block()
	<< " bcid " << mcEvtInfo->event_ID()->bunch_crossing_id()
	<< " SIMULATION " << mcEvtInfo->event_type()->test(EventType::IS_SIMULATION)
	<< " mc_channel_number " << mcEvtInfo->event_type()->mc_channel_number()
	<< " mc_event_number " << mcEvtInfo->event_type()->mc_event_number()
	<< " mc_event_weight " << mcEvtInfo->event_type()->mc_event_weight()
	<< " eventflags core " << mcEvtInfo->eventFlags(EventInfo::Core)
	<< " errorstate core " << mcEvtInfo->errorState(EventInfo::Core)
	<< " eventflags lar " << mcEvtInfo->eventFlags(EventInfo::LAr)
	<< " errorstate lar " << mcEvtInfo->errorState(EventInfo::LAr)
	<< " from store " << m_storeGateMC->name() << endmsg;
    
  } else {
    log << MSG::WARNING << "Could not retrieve EventInfo from MC store "<< endmsg;  
  }
  
  const EventInfo* dataEvtInfo = 0;
  if (m_storeGateData->retrieve(dataEvtInfo).isSuccess() ) {
    log << MSG::INFO
	<< "Got EventInfo from Data store: " 
	<< " event " << dataEvtInfo->event_ID()->event_number() 
	<< " run " << dataEvtInfo->event_ID()->run_number()
	<< " timestamp " << dataEvtInfo->event_ID()->time_stamp()
	<< " lbn " << dataEvtInfo->event_ID()->lumi_block()
	<< " bcid " << dataEvtInfo->event_ID()->bunch_crossing_id()
	<< " SIMULATION " << dataEvtInfo->event_type()->test(EventType::IS_SIMULATION)
	<< " mc_channel_number " << dataEvtInfo->event_type()->mc_channel_number()
	<< " mc_event_number " << dataEvtInfo->event_type()->mc_event_number()
	<< " mc_event_weight " << dataEvtInfo->event_type()->mc_event_weight()
	<< " eventflags core " << dataEvtInfo->eventFlags(EventInfo::Core)
	<< " errorstate core " << dataEvtInfo->errorState(EventInfo::Core)
	<< " eventflags lar " << dataEvtInfo->eventFlags(EventInfo::LAr)
	<< " errorstate lar " << dataEvtInfo->errorState(EventInfo::LAr)
	<< " from store " << m_storeGateData->name() << endmsg;    
  } else {
    log << MSG::WARNING << "Could not retrieve EventInfo from Data store "<< endmsg;  
  }

  const EventInfo* data2EvtInfo = m_storeGateData2->tryConstRetrieve<EventInfo>();
  if (data2EvtInfo) {
    log << MSG::INFO
	<< "Got EventInfo from Data2 store: " 
	<< " event " << data2EvtInfo->event_ID()->event_number() 
	<< " run " << data2EvtInfo->event_ID()->run_number()
	<< " timestamp " << data2EvtInfo->event_ID()->time_stamp()
	<< " lbn " << data2EvtInfo->event_ID()->lumi_block()
	<< " bcid " << data2EvtInfo->event_ID()->bunch_crossing_id()
	<< " SIMULATION " << data2EvtInfo->event_type()->test(EventType::IS_SIMULATION)
	<< " mc_channel_number " << data2EvtInfo->event_type()->mc_channel_number()
	<< " mc_event_number " << data2EvtInfo->event_type()->mc_event_number()
	<< " mc_event_weight " << data2EvtInfo->event_type()->mc_event_weight()
	<< " eventflags core " << data2EvtInfo->eventFlags(EventInfo::Core)
	<< " errorstate core " << data2EvtInfo->errorState(EventInfo::Core)
	<< " eventflags lar " << data2EvtInfo->eventFlags(EventInfo::LAr)
	<< " errorstate lar " << data2EvtInfo->errorState(EventInfo::LAr)
	<< " from store " << m_storeGateData2->name() << endmsg;    
  } else {
    log << MSG::INFO << "Could not retrieve EventInfo from Data2 store "<< endmsg;  
  }
  
  const EventInfo* outEvtInfo = 0;
  if (m_storeGateOutput->retrieve(outEvtInfo).isSuccess() ) {
    log << MSG::INFO
	<< "Got EventInfo from Out store: " 
	<< " event " << outEvtInfo->event_ID()->event_number() 
	<< " run " << outEvtInfo->event_ID()->run_number()
	<< " timestamp " << outEvtInfo->event_ID()->time_stamp()
	<< " lbn " << outEvtInfo->event_ID()->lumi_block()
	<< " bcid " << outEvtInfo->event_ID()->bunch_crossing_id()
	<< " SIMULATION " << outEvtInfo->event_type()->test(EventType::IS_SIMULATION)
	<< " mc_channel_number " << outEvtInfo->event_type()->mc_channel_number()
	<< " mc_event_number " << outEvtInfo->event_type()->mc_event_number()
	<< " mc_event_weight " << outEvtInfo->event_type()->mc_event_weight()
	<< " eventflags core " << outEvtInfo->eventFlags(EventInfo::Core)
	<< " errorstate core " << outEvtInfo->errorState(EventInfo::Core)
	<< " eventflags lar " << outEvtInfo->eventFlags(EventInfo::LAr)
	<< " errorstate lar " << outEvtInfo->errorState(EventInfo::LAr)
	<< " from store " << m_storeGateOutput->name()	<< endmsg;
  } else {
    log << MSG::WARNING << "Could not retrieve EventInfo from Out store "<< endmsg;  
  }

  //Stolen from Event/xAOD/xAODEventInfoCnv/src/EventInfoCnvAlg.cxx
  //Check if anything needs to be done for xAOD::EventInfo...
  std::string xaodKey = "EventInfo";
  if( ! m_storeGateOutput->contains< xAOD::EventInfo >( xaodKey ) ) { 
    ATH_MSG_INFO( "Making xAOD::EventInfo with key: " << xaodKey );

    // Create the xAOD object(s):
    xAOD::EventAuxInfo* aux = new xAOD::EventAuxInfo();
    xAOD::EventInfo* xaod = new xAOD::EventInfo();
    xaod->setStore( aux );

    // Do the translation:
    CHECK( m_cnvTool->convert( outEvtInfo, xaod ) );

    //Record the xAOD object(s):
    CHECK( m_storeGateOutput->record( aux, xaodKey + "Aux." ) );
    CHECK( m_storeGateOutput->record( xaod, xaodKey ) );
  }//xAOD::EventInfo

  if (m_checkeventnumbers){
  //Check consistency of output run/event with input runs/events
  if (outEvtInfo->event_ID()->event_number() != dataEvtInfo->event_ID()->event_number()){
    log << MSG::ERROR << "Output event number doesn't match input data event number!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (outEvtInfo->event_ID()->event_number() != mcEvtInfo->event_ID()->event_number()){
    log << MSG::WARNING << "Output event number doesn't match input MC event number!" << endmsg;
    //return StatusCode::FAILURE;
  }
  if (outEvtInfo->event_ID()->run_number() != dataEvtInfo->event_ID()->run_number()){
    log << MSG::ERROR << "Output run number doesn't match input data run number!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (outEvtInfo->event_ID()->run_number() != mcEvtInfo->event_ID()->run_number()){
    log << MSG::ERROR << "Output run number doesn't match input MC run number!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (outEvtInfo->event_ID()->time_stamp() != dataEvtInfo->event_ID()->time_stamp()){
    log << MSG::ERROR << "Output time stamp doesn't match input data time stamp!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (outEvtInfo->event_ID()->time_stamp() != mcEvtInfo->event_ID()->time_stamp()){
    log << MSG::ERROR << "Output time stamp doesn't match input MC time stamp!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (outEvtInfo->event_ID()->lumi_block() != dataEvtInfo->event_ID()->lumi_block()){
    log << MSG::ERROR << "Output lbn doesn't match input data lbn!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (outEvtInfo->event_ID()->lumi_block() != mcEvtInfo->event_ID()->lumi_block()){
    log << MSG::ERROR << "Output lbn doesn't match input MC lbn!" << endmsg;
    return StatusCode::FAILURE;
  } 
 if (outEvtInfo->event_ID()->bunch_crossing_id() != dataEvtInfo->event_ID()->bunch_crossing_id()){
    log << MSG::ERROR << "Output bcid doesn't match input data bcid!" << endmsg;
    return StatusCode::FAILURE;
  }
  }

  EventInfo * newEvtInfo = new EventInfo ( *outEvtInfo );
  newEvtInfo->event_type()->set_mc_channel_number(mcEvtInfo->event_type()->mc_channel_number());
  newEvtInfo->event_type()->set_mc_event_number(mcEvtInfo->event_type()->mc_event_number());
  newEvtInfo->event_type()->set_mc_event_weight(mcEvtInfo->event_type()->mc_event_weight());
  newEvtInfo->setEventFlags(EventInfo::Core, (dataEvtInfo->eventFlags(EventInfo::Core) | mcEvtInfo->eventFlags(EventInfo::Core)));
  newEvtInfo->setErrorState(EventInfo::Core, std::max(dataEvtInfo->errorState(EventInfo::Core),mcEvtInfo->errorState(EventInfo::Core)));

  //Doesn't seem to actually stop the event from being written out...
  // if (EventInfo::Error == mcEvtInfo->errorState(EventInfo::Core)){
  //     log << MSG::WARNING << "Aborting event because EventInfo::Core Error in mcEvtInfo (LooperKiller from G4?)" << endmsg;
  //     setFilterPassed( false );
  //   }
  
  log << MSG::INFO
      << "Have newEvtInfo for Out store: " 
      << " event " << newEvtInfo->event_ID()->event_number() 
      << " run " << newEvtInfo->event_ID()->run_number()
      << " timestamp " << newEvtInfo->event_ID()->time_stamp()
      << " lbn " << newEvtInfo->event_ID()->lumi_block()
      << " bcid " << newEvtInfo->event_ID()->bunch_crossing_id()
      << " SIMULATION " << newEvtInfo->event_type()->test(EventType::IS_SIMULATION)
      << " mc_channel_number " << newEvtInfo->event_type()->mc_channel_number()
      << " mc_event_number " << newEvtInfo->event_type()->mc_event_number()
      << " mc_event_weight " << newEvtInfo->event_type()->mc_event_weight() 
      << " eventflags core " << newEvtInfo->eventFlags(EventInfo::Core)
      << " errorstate core " << newEvtInfo->errorState(EventInfo::Core)
      << " eventflags lar " << newEvtInfo->eventFlags(EventInfo::LAr)
      << " errorstate lar " << newEvtInfo->errorState(EventInfo::LAr)
      << " to store "<<m_storeGateOutput->name()<< endmsg;
  if (m_storeGateOutput->contains<EventInfo>(m_infoType) ){ removeAllObjectsOfType<EventInfo>(&*m_storeGateOutput); }
  if ( m_storeGateOutput->record( newEvtInfo, m_infoType ).isFailure() ) {
    log << MSG::ERROR << "could not record EventInfo to output storeGate, key= " << m_infoType << endmsg;
    return StatusCode::FAILURE;
  }

  }//m_realdata
  
  //
  // Copy the McEventCollection
  //

  std::vector<const McEventCollection*> listOfMcEventCollection;

  /** the signal is MC - so there muct be McEventCollection there */
  const McEventCollection * sigEvtColl = 0;
  if ( m_storeGateMC->retrieve(sigEvtColl, "TruthEvent").isFailure() ) {
     log << MSG::WARNING 
         << "Could not retrieve HepMC collection with key " 
         << "TruthEvent" << endmsg;
   }
   if ( sigEvtColl ) listOfMcEventCollection.push_back( sigEvtColl );
 
  if (!m_realdata){
  /** retrieve McEventCollection from the background
      if the background is real data, there there is no McEventCollection there */
  const McEventCollection * bacEvtColl = 0;
  if ( m_storeGateData->retrieve(bacEvtColl, "TruthEvent").isFailure() ) {
     log << MSG::WARNING
         << "Could not retrieve HepMC collection with key "
         << "TruthEvent" << endmsg;
   }
   if ( bacEvtColl ) listOfMcEventCollection.push_back( bacEvtColl );
  }//!m_realdata

   McEventCollection * newMcEvtColl = new McEventCollection;
   for ( unsigned int i=0; i<listOfMcEventCollection.size(); ++i) { 
      if ( !listOfMcEventCollection[i] ) continue;
      for ( McEventCollection::const_iterator iEvt = listOfMcEventCollection[i]->begin();
          iEvt != listOfMcEventCollection[i]->end();
          ++iEvt ) {
          newMcEvtColl->push_back( new HepMC::GenEvent(**iEvt) );
      } //> end loop over HepMC::GenEvent
   } 

   if ( m_storeGateOutput->record(newMcEvtColl, "TruthEvent").isFailure() ) {
     log << MSG::ERROR << "Could not add new HepMC collection with key " << "TruthEvent" << endmsg;
     return StatusCode::FAILURE;
   }
   
  copyAllObjectsOfType<TrackRecordCollection>(&*m_storeGateOutput, &*m_storeGateMC);
  copyAllObjectsOfType<CaloCalibrationHitContainer>(&*m_storeGateOutput, &*m_storeGateMC);
  if (!m_realdata) {copyAllObjectsOfType<HijingEventParams>(&*m_storeGateOutput, &*m_storeGateData);}

  log << MSG::DEBUG << "CopyMcEventCollection::execute() end"<< endmsg;
  return StatusCode::SUCCESS;
}

//================================================================
//EOF
