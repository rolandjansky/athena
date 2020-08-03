/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "ReadPileUpData.h"

#include <list>
#include <map>
#include <vector>
#include "AthExStoreGateExample/MyDataObj.h"
#include "MyContObj.h"
#include "StoreGateExample_ClassDEF.h"

#include "PileUpTools/PileUpMergeSvc.h"

#include "GaudiKernel/ISvcLocator.h"
#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/EventID.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthLinks/DataLink.h"

/////////////////////////////////////////////////////////////////////////////

ReadPileUpData::ReadPileUpData(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), p_mergeSvc (0)
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadPileUpData::initialize(){


  ATH_MSG_INFO ("in initialize()");

  //locate the overlaid StoreGateSvc and initialize our local ptr
  if (!evtStore().retrieve().isSuccess()) {
    ATH_MSG_ERROR ("Could not find StoreGateSvc");
    return StatusCode::FAILURE;
  }
  //locate the PileUpMergeSvc and initialize our local ptr
  if (!(service("PileUpMergeSvc", p_mergeSvc)).isSuccess() || 
      0 == p_mergeSvc) {
    ATH_MSG_ERROR ("Could not find PileUpMergeSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadPileUpData::execute() {

  // An algorithm, like this one, retrieving an object from the StoreGate(SG)
  // can either ask for:
  //    i) the most recent (default) object of a given type 
  //   ii) a specific object of a given type
  //  iii) all objects of a given type
  //This example will show how to perform these three tasks


  ATH_MSG_INFO ("in execute()");

  /////////////////////////////////////////////////////////////////////
  // Get the overlaid event header, print out event and run number

  const PileUpEventInfo* pEvt;
  if (StatusCode::SUCCESS == evtStore()->retrieve(pEvt))
  {
    ATH_MSG_INFO ("Overlaid EventInfo : "
		  << " event: " << pEvt->event_ID()->event_number() 
		  << " run: " << pEvt->event_ID()->run_number());
  }
  else
  {
    ATH_MSG_ERROR (" Unable to retrieve EventInfo from StoreGate ");
    return StatusCode::SUCCESS;
  }

  // access the original event
  const EventInfo *pOrigEvent(0);
  if (!(p_mergeSvc->retrieveOriginal("ZebraEventInfo", pOrigEvent)).isSuccess()) {
    ATH_MSG_ERROR ("Could not find original event");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO 
    ("Original EventInfo (using PileUpMergeSvc): "
     << " event: " << pOrigEvent->event_ID()->event_number() 
     << " run: " << pOrigEvent->event_ID()->run_number());
  

  // access the sub events...
  PileUpEventInfo::SubEvent::const_iterator iEvt = pEvt->beginSubEvt();
  PileUpEventInfo::SubEvent::const_iterator endEvt = pEvt->endSubEvt();
  while (iEvt != endEvt) {
    ATH_MSG_INFO 
      ("SubEvt EventInfo : "
       << " time offset: " << iEvt->time()
       << " event: " << iEvt->pSubEvt->event_ID()->event_number() 
       << " run: " << iEvt->pSubEvt->event_ID()->run_number()
       << " contents: \n" << iEvt->pSubEvtSG->dump());
    ++iEvt;
  }

  //another way to do it (using the Merge service)
  typedef PileUpMergeSvc::TimedList<EventInfo>::type TimedEventList;
  //this is a list<pair<time_type, DataLink<EventInfo> > >
  TimedEventList evtList;
  if (!(p_mergeSvc->retrieveSubEvtsData("ZebraEventInfo", evtList)).isSuccess()) {
    ATH_MSG_ERROR ("Could not fill TimedEventList");
    return StatusCode::FAILURE;
  }
  TimedEventList::iterator iTEvt(evtList.begin());
  TimedEventList::iterator endTEvt(evtList.end());
  // FIXME const_iterator does not work: DataLink bug!
  //  TimedEventList::const_iterator iTEvt(evtList.begin());
  //  TimedEventList::const_iterator endTEvt(evtList.end());
  while (iTEvt != endTEvt) {
    ATH_MSG_INFO 
      ("SubEvt EventInfo (using PileUpMergeSvc): "
       << " time offset: " << iTEvt->first   //time is first of the pair
       << " event: " << iTEvt->second->event_ID()->event_number() 
       << " run: " << iTEvt->second->event_ID()->run_number());
    ++iTEvt;
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadPileUpData::finalize() {

  ATH_MSG_INFO ("in finalize()");
  return StatusCode::SUCCESS;
}
