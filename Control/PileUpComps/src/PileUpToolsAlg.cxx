/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfo/EventID.h"

#include "PileUpTools/IPileUpTool.h"

#include "PileUpToolsAlg.h"

/////////////////////////////////////////////////////////////////////////////

PileUpToolsAlg::PileUpToolsAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_puTools()
{
  declareProperty("PileUpTools", m_puTools, "IPileUpTools to be run for each event");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PileUpToolsAlg::initialize(){
  StatusCode sc(StatusCode::FAILURE);
  msg() << MSG::DEBUG << "Initializing " << name()
        << " - package version " << PACKAGE_VERSION << endreq ;
  //locate the pu tools and initialize them
  if (!(sc=m_puTools.retrieve()).isSuccess()) {
    ATH_MSG_ERROR ("Could not retrieve PileUpTools");
  }
  return sc;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PileUpToolsAlg::execute() {

  // An algorithm, like this one, retrieving an object from the StoreGate(SG)
  // can either ask for:
  //    i) the most recent (default) object of a given type
  //   ii) a specific object of a given type
  //  iii) all objects of a given type
  //This example will show how to perform these three tasks


  ATH_MSG_DEBUG ("in execute()");

  /////////////////////////////////////////////////////////////////////
  // Get the overlaid event header, print out event and run number

  const PileUpEventInfo* evt;
  if (StatusCode::SUCCESS == evtStore()->retrieve(evt))
  {
    ATH_MSG_INFO ("Overlaid EventInfo : "
                  << " event: " << evt->event_ID()->event_number()
                  << " run: " << evt->event_ID()->run_number());
  }
  else
  {
    ATH_MSG_ERROR (" Unable to retrieve OverlaidEventInfo from StoreGate ");
    return StatusCode::RECOVERABLE;
  }

  //strictly speaking this should not be done once per event
  ToolHandleArray<IPileUpTool>::iterator bPUT(m_puTools.begin());
  ToolHandleArray<IPileUpTool>::iterator ePUT(m_puTools.end());

  // access the sub events...
  PileUpEventInfo::SubEvent::const_iterator iEvt = evt->beginSubEvt();
  PileUpEventInfo::SubEvent::const_iterator endEvt = evt->endSubEvt();

  // first and last event in a xing
  PileUpEventInfo::SubEvent::const_iterator fEvt = evt->beginSubEvt();
  PileUpEventInfo::SubEvent::const_iterator lEvt = fEvt;
  int currXing(iEvt->time());
  int lastXing(currXing);

  // give IPileUpTools a chance to setup for this event
  ToolHandleArray<IPileUpTool>::iterator iPUT(bPUT);
  const unsigned int nInputEvents = std::distance(iEvt,endEvt);
  ATH_MSG_DEBUG( "execute: There are " << nInputEvents << " subevents in this Event." );

  std::map<const IPileUpTool*, unsigned int> eventsToProcessByTool;
  while (iEvt != endEvt) {
    currXing = iEvt->time();
    lEvt=iEvt;
    //check if we are in a new xing
    if (currXing != lastXing) {
      //loop over PileUpTools for the last xing
      iPUT = bPUT;
      while (iPUT != ePUT) {
        IPileUpTool *put(&(**iPUT));
        if (put->toProcess(lastXing)) {
          eventsToProcessByTool[put] += std::distance(fEvt,lEvt);
        }
        ++iPUT;
      }
      lastXing=currXing;
      fEvt=lEvt;
    }
    ++iEvt;
  }
  //loop over the PileUpTools for the final xing
  if (lEvt != endEvt) {
    lEvt=endEvt;
    iPUT = bPUT;
    while (iPUT != ePUT) {
      IPileUpTool *put(&(**iPUT));
      if (put->toProcess(lastXing)) {
        eventsToProcessByTool[put] += std::distance(fEvt,lEvt);
      }
      ++iPUT;
    }
  }
  //reset everything
  iPUT = bPUT;
  iEvt = evt->beginSubEvt();
  fEvt = evt->beginSubEvt();
  lEvt = evt->beginSubEvt();
  currXing = (iEvt->time());
  lastXing = (currXing);
  //set nInputEvents
  while (iPUT != ePUT && ((**(iPUT)).prepareEvent(eventsToProcessByTool[&(**iPUT)])).isSuccess())  {++iPUT;}

  while (iEvt != endEvt) {
    ATH_MSG_DEBUG
      ("SubEvt EventInfo : "
       << " time offset: " << iEvt->time()
       << " event: " << iEvt->pSubEvt->event_ID()->event_number()
       << " run: " << iEvt->pSubEvt->event_ID()->run_number());
    //FIXME       << " contents: \n" << iEvt->pSubEvtSG->dump());
    currXing = iEvt->time();
    lEvt=iEvt;
    //check if we are in a new xing
    if (currXing != lastXing) {

      //loop over the PileUpTools for the last xing
      iPUT = bPUT;
      StatusCode sc(StatusCode::SUCCESS);
      while (sc.isSuccess() && (iPUT != ePUT)) {
        IPileUpTool& put(**iPUT);
        if (put.toProcess(lastXing)) {
          sc=put.processBunchXing(lastXing, fEvt, lEvt);
        } else {
          ATH_MSG_VERBOSE( "Skipping IPileUpTool " << put.name()
                           << " for xing " << lastXing );
        }
        ++iPUT;
      }
      //clear the bkg events of previous xing.
      if(!(this->clearXing(fEvt, lEvt).isSuccess())) return StatusCode::FAILURE;
      lastXing=currXing;
      fEvt=lEvt;
    }
    ++iEvt;
  }
  //loop over the PileUpTools for the final xing
  if (lEvt != endEvt) {
    lEvt=endEvt;
    iPUT = bPUT;
    StatusCode sc(StatusCode::SUCCESS);
    while (sc.isSuccess() && (iPUT != ePUT)) {
      IPileUpTool& put(**iPUT);
      if (put.toProcess(lastXing)) {
        sc=put.processBunchXing(lastXing, fEvt, lEvt);
      } else {
        ATH_MSG_VERBOSE( "Skipping IPileUpTool " << put.name()
                         << " for xing " << lastXing );
      }
      ++iPUT;
    }
    //clear the bkg events of previous xing.
    if(!(this->clearXing(fEvt, lEvt).isSuccess())) return StatusCode::FAILURE;
  }

  iPUT = bPUT;
  while (iPUT != ePUT && ((**(iPUT++)).mergeEvent()).isSuccess()) ;

  //final loop over the PileUpTools



  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PileUpToolsAlg::finalize() {

  ATH_MSG_DEBUG ("in finalize()");
  return StatusCode::SUCCESS;
}


StatusCode PileUpToolsAlg::clearXing(PileUpEventInfo::SubEvent::const_iterator& fEvt, PileUpEventInfo::SubEvent::const_iterator& lEvt) {
  PileUpEventInfo::SubEvent::const_iterator iClearEvt=fEvt;
  StatusCode sc = StatusCode::SUCCESS;
  while (sc.isSuccess() && (iClearEvt != lEvt)) {
    if (!(sc = iClearEvt->pSubEvtSG->clearStore()).isSuccess()) {
      msg() << MSG::ERROR
            << "Can not clear store "
            << iClearEvt->pSubEvtSG->name() << endreq;
    } else {
#ifndef NDEBUG
      msg() << MSG::VERBOSE
            << "Cleared store " << iClearEvt->pSubEvtSG->name() << endreq;
#endif
    }
    ++iClearEvt;
  }
  return sc;
}
