/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Class header
#include "PileUpToolsAlg.h"
#include <algorithm>

/////////////////////////////////////////////////////////////////////////////

PileUpToolsAlg::PileUpToolsAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PileUpToolsAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << " - package version " << PACKAGE_VERSION);
  //locate the pu tools and initialize them
  ATH_CHECK(m_puTools.retrieve());
  // initialise read handle keys
  ATH_CHECK(m_eventInfoKey.initialize());
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool differentBunchXing (xAOD::EventInfo::SubEvent i, xAOD::EventInfo::SubEvent j)
{
  return (i.time()!=j.time());
}


StatusCode PileUpToolsAlg::execute()
{
  ATH_MSG_DEBUG ("in execute()");

  /////////////////////////////////////////////////////////////////////
  // Get the overlaid event header, print out event and run number

  SG::ReadHandle<xAOD::EventInfo> evt(m_eventInfoKey);
  if (!evt.isValid()) {
    ATH_MSG_ERROR("Could not get xAOD::EventInfo " << evt.name() << " from store " << evt.store());
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO ("Hard-scatter xAOD::EventInfo : " << " event: " << evt->eventNumber() << " run: " << evt->runNumber());

  // access the sub events...
  SubEventIterator iEvt(evt->subEvents().begin());
  const SubEventIterator endEvt(evt->subEvents().end());

  ATH_MSG_DEBUG( "There are " << std::distance(iEvt,endEvt) << " subevents in this Event." );

  // Group subevents by bunch-crossing and work out the number of
  // subevents that each PileUpTools will need to process.
  std::map<const IPileUpTool*, unsigned int> eventsToProcessByTool;
  std::map<SubEventIterator, SubEventIterator> bunchCrossingRanges;
  while (iEvt != endEvt)
    {
      const SubEventIterator fEvt(iEvt);
      // find the next SubEvent where the SubEvent after it has a
      // different bunch crossing time.
      iEvt = std::adjacent_find(fEvt, endEvt, differentBunchXing);
      const SubEventIterator lEvt = (iEvt==endEvt) ? iEvt : ++iEvt;
      const unsigned int nEventsInXing = std::distance(fEvt,lEvt);
      ATH_MSG_VERBOSE( "Found " << nEventsInXing << " consecutive subevents with a bunch crossing time of " << fEvt->time() << " ns." );
      bunchCrossingRanges[lEvt]=fEvt; //FIXME storing range "backwards" as the first part of the pair is const when accessed later
      // loop over PileUpTools to check if they will need to process
      // the current bunch crossing
      for(auto& puToolHandle : m_puTools)
        {
          if (puToolHandle->toProcess(fEvt->time()))
            {
              eventsToProcessByTool[&(*puToolHandle)] += nEventsInXing;
            }
        }
    }

  // call prepareEvent for all PileUpTools to set nInputEvents
  for(auto& puToolHandle : m_puTools)
    {
      // Reset the filters
      puToolHandle->resetFilter();
      ATH_MSG_VERBOSE ( puToolHandle->name() << " will get " << eventsToProcessByTool[&(*puToolHandle)] << " subevents to process." );
      ATH_CHECK(puToolHandle->prepareEvent(Gaudi::Hive::currentContext(), eventsToProcessByTool[&(*puToolHandle)]));
    }

  // Loop over bunch-crossings and call processBunchXing for each
  // PileUpTool
  for(auto& bunch : bunchCrossingRanges)
    {
      SubEventIterator fEvt(bunch.second);
      const SubEventIterator lEvt(bunch.first);
      const int bunchXing(fEvt->time());
      ATH_MSG_DEBUG("Processing bunch-crossing at " << bunchXing << " ns.");
      for(auto& puToolHandle : m_puTools)
        {
          if(puToolHandle->toProcess(bunchXing))
            {
              ATH_CHECK(puToolHandle->processBunchXing(bunchXing, fEvt, lEvt));
            }
          else
            {
              ATH_MSG_VERBOSE( "Skipping " << puToolHandle->name() << " for bunch crossing at " << bunchXing << " ns.");
            }
        }
      // clear the bkg events from the bunch xing we have just processed.
      ATH_CHECK(this->clearXing(fEvt, lEvt));
    }

  // call mergeEvent for all PileUpTools
  for(auto& puToolHandle : m_puTools)
    {
      ATH_CHECK(puToolHandle->mergeEvent(Gaudi::Hive::currentContext()));
      // Check if the event was filtered out by the current PileUpTool.
      if (!puToolHandle->filterPassed())
        {
          ATH_MSG_VERBOSE( "Filter " << puToolHandle->name() << " failed - will stop the event" );
          this->setFilterPassed(false);
        }
    }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PileUpToolsAlg::finalize()
{
  ATH_MSG_DEBUG ("in finalize()");
  return StatusCode::SUCCESS;
}


StatusCode PileUpToolsAlg::clearXing(SubEventIterator& fEvt, const SubEventIterator& lEvt)
{
  SubEventIterator iClearEvt=fEvt;
  while (iClearEvt != lEvt)
    {
      ATH_CHECK(iClearEvt->ptr()->evtStore()->clearStore());
#ifndef NDEBUG
      ATH_MSG_VERBOSE("Cleared store " << iClearEvt->ptr()->evtStore()->name());
#endif
      ++iClearEvt;
    }
  return StatusCode::SUCCESS;
}
