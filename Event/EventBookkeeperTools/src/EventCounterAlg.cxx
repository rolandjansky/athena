///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// EventCounterAlg.cxx
// Implementation file for class EventCounterAlg
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////

// EventBookkeeperTools includes
#include "EventCounterAlg.h"

// STL includes
//#include <stdio.h>
//#include <stdlib.h>
//#include <printf.h>
#include <iostream>

// EDM includes
#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandle.h"



///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
EventCounterAlg::EventCounterAlg( const std::string& name,
                                  ISvcLocator* pSvcLocator ) :
  ::AthFilterAlgorithm( name, pSvcLocator ),
  m_trackOtherMCWeights(false),
  m_eventsProcessed(0),
  m_mcCutIDs()
{
  //
  // Property declaration
  //
  declareProperty( "BookkeepOtherMCEventWeights", m_trackOtherMCWeights=false,
                   "If true, the non-nominal MC event weights will be bookkept as well" );
}



// Destructor
///////////////
EventCounterAlg::~EventCounterAlg()
{}



// Athena Algorithm's Hooks
////////////////////////////
StatusCode EventCounterAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << this->name() << "...");

  cutFlowSvc()->registerTopFilter( this->name(),
                                   "Number of processed events before any cut",
                                   xAOD::CutBookkeeper::CutLogic::ALLEVENTSPROCESSED,
                                   "AllStreams");

  return StatusCode::SUCCESS;
}



StatusCode EventCounterAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << this->name() << "...");
  return StatusCode::SUCCESS;
}



StatusCode EventCounterAlg::execute()
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  ATH_MSG_VERBOSE ("Executing " << this->name() << "...");

  setFilterPassed(true);

  // Update also the other counters for the non-nominal MC weights
  if (m_trackOtherMCWeights) {
    // Get the EventInfo object
    SG::ReadHandle<xAOD::EventInfo> evtInfo (eventInfoKey(), ctx);
    // Only try to access the mcEventWeight is we are running on Monte Carlo, duhhh!
    if ( !(evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) ) {
      ATH_MSG_DEBUG("We are not running on simulation and thus, nothing to be done here");
      m_trackOtherMCWeights = false;
      return StatusCode::SUCCESS;
    }

    // Get all MC event weights
    const std::vector<float>& mcWeights = evtInfo->mcEventWeights();

    // Set up everything during the first event
    if ( m_eventsProcessed == 0 ){
      const std::size_t nNonNominalMCWeight = mcWeights.size() - 1;
      m_mcCutIDs.reserve(nNonNominalMCWeight);
      for ( std::size_t i=1; i<mcWeights.size(); ++i ) {
        std::stringstream sstm;
        sstm << this->name() << "_NonNominalMCWeight_" << i;
        const std::string cutName = sstm.str();
        std::stringstream sstm2;
        sstm2 << "non-nominal MC event weight number " << i;
        const std::string& cutDescription = sstm2.str();
        CutIdentifier cutID = cutFlowSvc()->registerTopFilter( cutName,
                                                               cutDescription,
                                                               xAOD::CutBookkeeper::CutLogic::ALLEVENTSPROCESSED,
                                                               "AllStreams" );
        m_mcCutIDs.push_back(cutID);
      }
    }

    // Increment the non-nominal MC event weight counters
    for ( std::size_t i=0; i<m_mcCutIDs.size(); ++i) {
      const double weight = static_cast<double>(mcWeights[i+1]);
      const CutIdentifier cutID = m_mcCutIDs[i];
      cutFlowSvc()->addEvent( cutID, weight );
    }

    // Increment the event counter
    m_eventsProcessed += 1;
  }

  return StatusCode::SUCCESS;
}
