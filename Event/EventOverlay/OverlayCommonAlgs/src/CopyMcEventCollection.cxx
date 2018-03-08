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
  declareProperty("RemoveBkgHardScatterTruth", m_removeBkgHardScatterTruth=true);
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

  // Validate Event Info
  if (m_checkeventnumbers)
  {

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

  if (m_realdata)
  {
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

  //Check consistency of output run/event with input runs/events
  if (m_realdata && outEvtInfo->event_ID()->event_number() != dataEvtInfo->event_ID()->event_number()){
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
  if (m_realdata && outEvtInfo->event_ID()->run_number() != mcEvtInfo->event_ID()->run_number()){
    log << MSG::ERROR << "Output run number doesn't match input MC run number!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (outEvtInfo->event_ID()->time_stamp() != dataEvtInfo->event_ID()->time_stamp()){
    log << MSG::ERROR << "Output time stamp doesn't match input data time stamp!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_realdata && outEvtInfo->event_ID()->time_stamp() != mcEvtInfo->event_ID()->time_stamp()){
    log << MSG::ERROR << "Output time stamp doesn't match input MC time stamp!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (outEvtInfo->event_ID()->lumi_block() != dataEvtInfo->event_ID()->lumi_block()){
    log << MSG::ERROR << "Output lbn doesn't match input data lbn!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_realdata && outEvtInfo->event_ID()->lumi_block() != mcEvtInfo->event_ID()->lumi_block()){
    log << MSG::ERROR << "Output lbn doesn't match input MC lbn!" << endmsg;
    return StatusCode::FAILURE;
  } 
  if (outEvtInfo->event_ID()->bunch_crossing_id() != dataEvtInfo->event_ID()->bunch_crossing_id()){
    log << MSG::ERROR << "Output bcid doesn't match input data bcid!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (outEvtInfo->event_type()->mc_channel_number() != mcEvtInfo->event_type()->mc_channel_number()){
    log << MSG::ERROR << "Output MC channel number doesn't match input MC channel number!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (outEvtInfo->event_type()->mc_event_number() != mcEvtInfo->event_type()->mc_event_number()){
    log << MSG::ERROR << "Output MC event number doesn't match input MC event number!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (outEvtInfo->event_type()->mc_event_weight() != mcEvtInfo->event_type()->mc_event_weight()){
    log << MSG::ERROR << "Output MC event weight doesn't match input MC event weight!" << endmsg;
    return StatusCode::FAILURE;
  }

  } // m_checkeventnumbers

  
  //
  // Copy the McEventCollection
  //
  McEventCollection *newMcEvtColl = new McEventCollection;

  /** the signal is MC - so there muct be McEventCollection there */
  const McEventCollection *sigEvtColl = 0;
  if (m_storeGateMC->retrieve(sigEvtColl, "TruthEvent").isFailure()) {
    ATH_MSG_WARNING("Could not retrieve signal HepMC collection with key " << "TruthEvent");
  } else {
    for (McEventCollection::const_iterator iEvt = sigEvtColl->begin(); iEvt != sigEvtColl->end(); ++iEvt) {
      newMcEvtColl->push_back(new HepMC::GenEvent(**iEvt));
    }
  }

  if (!m_realdata) {
    /** retrieve McEventCollection from the background
        if the background is real data, there there is no McEventCollection there
     */
    const McEventCollection *bgEvtColl = 0;
    if (m_storeGateData->retrieve(bgEvtColl, "TruthEvent").isFailure()) {
      ATH_MSG_WARNING("Could not retrieve background HepMC collection with key " << "TruthEvent");
    } else {
      McEventCollection::const_iterator iEvt = bgEvtColl->begin();
      if (m_removeBkgHardScatterTruth) {
        ++iEvt;
      }
      for ( ; iEvt != bgEvtColl->end(); ++iEvt) {
        newMcEvtColl->push_back(new HepMC::GenEvent(**iEvt));
      }
    }
  } //! m_realdata

  if ( m_storeGateOutput->record(newMcEvtColl, "TruthEvent").isFailure() ) {
    ATH_MSG_ERROR("Could not add new HepMC collection with key " << "TruthEvent");
    return StatusCode::FAILURE;
  }

  /** dump McEventCollection in debug mode to confirm everything is as expected */
  if (msgLvl(MSG::DEBUG)) {
    if (! newMcEvtColl->empty()) {
      ATH_MSG_DEBUG("McEventCollection contents:");
      for (McEventCollection::const_iterator iEvt = newMcEvtColl->begin(); iEvt != newMcEvtColl->end(); ++iEvt) {
        const int signal_process_id((*iEvt)->signal_process_id()),
                  event_number((*iEvt)->event_number());
        ATH_MSG_DEBUG("  GenEvent #" << event_number << ", signal_process_id=" << signal_process_id);
      }
    }
  }

  copyAllObjectsOfType<TrackRecordCollection>(&*m_storeGateOutput, &*m_storeGateMC);
  copyAllObjectsOfType<CaloCalibrationHitContainer>(&*m_storeGateOutput, &*m_storeGateMC);
  if (!m_realdata) {copyAllObjectsOfType<HijingEventParams>(&*m_storeGateOutput, &*m_storeGateData);}

  log << MSG::DEBUG << "CopyMcEventCollection::execute() end"<< endmsg;
  return StatusCode::SUCCESS;
}

//================================================================
//EOF
