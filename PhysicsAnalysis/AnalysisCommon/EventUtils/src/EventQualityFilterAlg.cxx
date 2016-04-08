///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EventQualityFilterAlg.cxx
// Implementation file for class EventQualityFilterAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
// Description: Algorithm to filter out events with a bad Event error
///////////////////////////////////////////////////////////////////

// SelectionUtils includes
#include "EventQualityFilterAlg.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"

// EDM includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"


///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
EventQualityFilterAlg::EventQualityFilterAlg( const std::string& name,
                                              ISvcLocator* pSvcLocator ) :
  ::AthFilterAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  //
  declareProperty( "VetoLArError",  m_useLArError       = false, "Veto events with a LAr error" );
  declareProperty( "VetoTileError", m_useTileError      = false, "Veto events with a Tile error" );
  declareProperty( "VetoCoreError", m_useCoreError      = false, "Veto events with a Core error" );
  declareProperty( "VetoTileTrips", m_useTileTripReader = false, "Veto events with a Tile trip error" );
}




// Destructor
///////////////
EventQualityFilterAlg::~EventQualityFilterAlg()
{}




// Athena Algorithm's Hooks
////////////////////////////
StatusCode EventQualityFilterAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");
  return StatusCode::SUCCESS;
}



StatusCode EventQualityFilterAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}



StatusCode EventQualityFilterAlg::execute()
{
  ATH_MSG_DEBUG ("Executing " << name() << "...");


  // Get the EventInfo object
  const EventInfo* eventInfo(NULL);
  ATH_CHECK( evtStore()->retrieve(eventInfo) );


  // Only do the event vetoing on data
  bool isSim = eventInfo->event_type()->test(EventType::IS_SIMULATION);
  if ( isSim )
    {
      ATH_MSG_DEBUG ("It is an MC event... not vetoing...");
      setFilterPassed(true);
      return StatusCode::SUCCESS;
    }


  // Now make the event decision
  bool passEvent(true);
  // if ( eventInfo->errorState(EventInfo::LAr)  == EventInfo::Error ){ passEvent = false; }
  // if ( eventInfo->errorState(EventInfo::Tile) == EventInfo::Error ){ passEvent = false; }
  // if ( eventInfo->eventFlags(EventInfo::Core) & 0x40000 ){ passEvent = false; }

  // Set the final decision
  setFilterPassed(passEvent);

  return StatusCode::SUCCESS;
}
