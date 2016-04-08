/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Class header
#include "PileUpToolsAlg.h"

#include "PileUpTools/IPileUpTool.h"

/////////////////////////////////////////////////////////////////////////////

PileUpToolsAlg::PileUpToolsAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_puTools()
{
  declareProperty("PileUpTools", m_puTools, "IPileUpTools to be run for each event");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PileUpToolsAlg::initialize(){
  StatusCode sc(StatusCode::FAILURE);
  ATH_MSG_DEBUG ("Initializing " << name() << " - package version " << PACKAGE_VERSION);
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

  const xAOD::EventInfo *evt(nullptr);
  if (StatusCode::SUCCESS == evtStore()->retrieve(evt))
  {
    ATH_MSG_INFO ("Overlaid EventInfo : "
                  << " event: " << evt->eventNumber()
                  << " run: " << evt->runNumber());
  }
  else
  {
    ATH_MSG_ERROR (" Unable to retrieve OverlaidEventInfo from StoreGate ");
    return StatusCode::RECOVERABLE;
  }

  //strictly speaking this should not be done once per event
  ToolHandleArray<IPileUpTool>::iterator bPUT(m_puTools.begin());
  ToolHandleArray<IPileUpTool>::iterator ePUT(m_puTools.end());

  // Reset the filters
  ToolHandleArray<IPileUpTool>::iterator iPUT(bPUT);
  while (iPUT != ePUT){
    (**iPUT).resetFilter();
    ++iPUT;
  }

  // access the sub events...
  SubEventIterator iEvt = evt->subEvents().begin();
  SubEventIterator endEvt = evt->subEvents().end();

  // first and last event in a xing
  SubEventIterator fEvt = evt->subEvents().begin();
  SubEventIterator lEvt = fEvt;
  int currXing(iEvt->time());
  int lastXing(currXing);

  // give IPileUpTools a chance to setup for this event
  iPUT=bPUT;
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
  iEvt = evt->subEvents().begin();
  fEvt = evt->subEvents().begin();
  lEvt = evt->subEvents().begin();
  currXing = (iEvt->time());
  lastXing = (currXing);
  //set nInputEvents
  while (iPUT != ePUT && ((**(iPUT)).prepareEvent(eventsToProcessByTool[&(**iPUT)])).isSuccess())  {++iPUT;}

  while (iEvt != endEvt) {
    ATH_MSG_DEBUG
      ("SubEvt EventInfo : "
       << " time offset: " << iEvt->time()
       << " event: " << iEvt->ptr()->eventNumber()
       << " run: " << iEvt->ptr()->runNumber());
    //FIXME       << " contents: \n" << iEvt->evtStore()->dump());
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

  // Bit wasteful, but see if we aborted here
  iPUT=bPUT;
  while (iPUT != ePUT){
    if (!(**(iPUT)).filterPassed()){
      ATH_MSG_VERBOSE( "Filter " << (**(iPUT)).name() << " failed - will stop the event" );
      this->setFilterPassed(false);
      break;
    }
    ++iPUT;
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PileUpToolsAlg::finalize() {

  ATH_MSG_DEBUG ("in finalize()");
  return StatusCode::SUCCESS;
}


StatusCode PileUpToolsAlg::clearXing(SubEventIterator& fEvt, SubEventIterator& lEvt) {
  SubEventIterator iClearEvt=fEvt;
  StatusCode sc = StatusCode::SUCCESS;
  while (sc.isSuccess() && (iClearEvt != lEvt)) {
    if (!(sc = iClearEvt->ptr()->evtStore()->clearStore()).isSuccess()) {
      ATH_MSG_ERROR("Can not clear store " << iClearEvt->ptr()->evtStore()->name());
    } else {
#ifndef NDEBUG
      ATH_MSG_VERBOSE("Cleared store " << iClearEvt->ptr()->evtStore()->name());
#endif
    }
    ++iClearEvt;
  }
  return sc;
}
