/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <sys/time.h>

// Framework
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/MsgStream.h"

// Reconstruction
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/TriggerInfo.h"

// Trigger
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

// Local
#include "TrigExpressStreamAlg.h"

//---------------------------------------------------------------------------------------
TrigExpressStreamAlg::TrigExpressStreamAlg(const std::string& name,
					   ISvcLocator* pSvcLocator)
  :Algorithm(name, pSvcLocator),
   m_log(0),
   m_storeGate("StoreGateSvc", name)
{
}

//---------------------------------------------------------------------------------------
TrigExpressStreamAlg::~TrigExpressStreamAlg()
{
}

//---------------------------------------------------------------------------------------
StatusCode TrigExpressStreamAlg::initialize()
{    
  m_log = new MsgStream(msgSvc(), name()); 
  
  if(m_storeGate.retrieve().isFailure()) {
    log() << MSG::ERROR << "Could not retrieve StoreGateSvc!" << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigExpressStreamAlg::execute()
{
  //
  // Process current event
  //

  const DataHandle<EventInfo> event_handle;
  if(m_storeGate -> retrieve(event_handle).isFailure()) {
    log() << MSG::INFO << "Failed to read EventInfo" << endreq;
    return StatusCode::SUCCESS;
  }

  //
  // Print EventInfo and stream tags
  //
  TriggerInfo *trig = event_handle->trigger_info();
  if(!trig) {
    log() << MSG::INFO << "Failed to get TriggerInfo" << endreq;
    return StatusCode::SUCCESS;
  }

  const std::vector<TriggerInfo::StreamTag> &streams = trig->streamTags();
  
  bool found_express_stream = false;
  for(unsigned i = 0; i < streams.size(); ++i) {
    const TriggerInfo::StreamTag &stag = streams.at(i);
    if(stag.type() == "express" && stag.name() == "express") {
      found_express_stream = true;
      break;
    }
  }

  if(!found_express_stream) {
    log() << MSG::INFO << "Failed to find express stream tag" << endreq;
    return StatusCode::SUCCESS;
  }

  log() << MSG::INFO << ">>>>>>>>>>>>>>>>"
	<< " run #" << event_handle->event_ID()->run_number()
	<< " lumi #" << event_handle->event_ID()->lumi_block()
	<< " event #" << event_handle->event_ID()->event_number() 
	<< " has express stream tag" << endreq;
  
  const std::string key = "HLT_EXPRESS_OPI_EF";

  if(!m_storeGate->contains<TrigOperationalInfoCollection>(key)) {
    log() << MSG::INFO << "Missing TrigOperationalInfoCollection with key=" << key << endreq;
    return StatusCode::SUCCESS;
  }
  
  const TrigOperationalInfoCollection *opi = 0;
  if(!m_storeGate->retrieve<TrigOperationalInfoCollection>(opi, key).isSuccess()) {
    log() << MSG::INFO << "Failed to retreive TrigOperationalInfoCollection with key=" << key << endreq;
    return StatusCode::SUCCESS;
  }
  
  log() << MSG::INFO << "Found TrigOperationalInfoCollectionwith key=" << key 
	<< " and size=" << opi->size() << endreq;

  for(TrigOperationalInfoCollection::const_iterator it = opi->begin(); it != opi->end(); ++it) {
    const TrigOperationalInfo *ptr = *it;
    if(!ptr) continue;
    
    const std::pair<std::vector<std::string>, std::vector<float> > infos = ptr->infos();
    
    for(unsigned i = 0; i < infos.first.size(); ++i) {
      log() << MSG::INFO << "Chain with express stream bit: " << infos.first.at(i) << endreq;
    }
  }
  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigExpressStreamAlg::finalize()
{
  //
  // Finalize state
  //
  delete m_log; m_log = 0;

  return StatusCode::SUCCESS;
}
