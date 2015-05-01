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
  :AthAlgorithm(name, pSvcLocator)
{
}

//---------------------------------------------------------------------------------------
TrigExpressStreamAlg::~TrigExpressStreamAlg()
{
}

//---------------------------------------------------------------------------------------
StatusCode TrigExpressStreamAlg::initialize()
{    
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigExpressStreamAlg::execute()
{
  //
  // Process current event
  //

  const DataHandle<EventInfo> event_handle;
  if(evtStore() -> retrieve(event_handle).isFailure()) {
    ATH_MSG_INFO( "Failed to read EventInfo" );
    return StatusCode::SUCCESS;
  }

  //
  // Print EventInfo and stream tags
  //
  TriggerInfo *trig = event_handle->trigger_info();
  if(!trig) {
    ATH_MSG_INFO( "Failed to get TriggerInfo" );
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
    ATH_MSG_INFO( "Failed to find express stream tag" );
    return StatusCode::SUCCESS;
  }

  ATH_MSG_INFO( ">>>>>>>>>>>>>>>>"
                << " run #" << event_handle->event_ID()->run_number()
                << " lumi #" << event_handle->event_ID()->lumi_block()
                << " event #" << event_handle->event_ID()->event_number() 
                << " has express stream tag" );
  
  const std::string key = "HLT_EXPRESS_OPI_EF";

  if(!evtStore()->contains<TrigOperationalInfoCollection>(key)) {
    ATH_MSG_INFO( "Missing TrigOperationalInfoCollection with key=" << key );
    return StatusCode::SUCCESS;
  }
  
  const TrigOperationalInfoCollection *opi = 0;
  if(!evtStore()->retrieve<TrigOperationalInfoCollection>(opi, key).isSuccess()) {
    ATH_MSG_INFO( "Failed to retreive TrigOperationalInfoCollection with key=" << key );
    return StatusCode::SUCCESS;
  }
  
  ATH_MSG_INFO( "Found TrigOperationalInfoCollectionwith key=" << key 
                << " and size=" << opi->size() );

  for(TrigOperationalInfoCollection::const_iterator it = opi->begin(); it != opi->end(); ++it) {
    const TrigOperationalInfo *ptr = *it;
    if(!ptr) continue;
    
    const std::pair<std::vector<std::string>, std::vector<float> > infos = ptr->infos();
    
    for(unsigned i = 0; i < infos.first.size(); ++i) {
      ATH_MSG_INFO( "Chain with express stream bit: " << infos.first.at(i) );
    }
  }
  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigExpressStreamAlg::finalize()
{
  return StatusCode::SUCCESS;
}
