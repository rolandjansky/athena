
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////// -*- C++ -*- /////////////////////////////
// EventQualityFilterAlg.cxx
// Implementation file for class EventQualityFilterAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
// Description: Algorithm to filter out events with a bad Event error
///////////////////////////////////////////////////////////////////

// SelectionUtils includes
#include "EventQualityFilterAlg.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"


///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
EventQualityFilterAlg::EventQualityFilterAlg( const std::string& name,
                                              ISvcLocator* pSvcLocator ) :
  ::AthFilterAlgorithm( name, pSvcLocator ),
  m_useLArError(true),
  m_useTileError(true),
  m_useSCTError(true),
  m_useCoreError(true)
{
  //
  // Property declaration
  //
  declareProperty( "VetoLArError",  m_useLArError,  "Veto events with a LAr error" );
  declareProperty( "VetoTileError", m_useTileError, "Veto events with a Tile error" );
  declareProperty( "VetoSCTError",  m_useSCTError,  "Veto events with an SCT error" );
  declareProperty( "VetoCoreError", m_useCoreError, "Veto events with a Core error" );
  //declareProperty( "VetoTileTrips", m_useTileTripReader, "Veto events with a Tile trip error" );
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
  ATH_MSG_DEBUG( "Using: " << m_useLArError );
  ATH_MSG_DEBUG( "Using: " << m_useTileError );
  ATH_MSG_DEBUG( "Using: " << m_useSCTError );
  ATH_MSG_DEBUG( "Using: " << m_useCoreError );
  // ATH_MSG_DEBUG( "Using: " << m_useTileTripReader );
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
  const xAOD::EventInfo* eventInfo = nullptr;
  ATH_CHECK( evtStore()->retrieve(eventInfo) );


  // Only do the event vetoing on data
  const bool isSim = eventInfo->eventType(xAOD::EventInfo::EventType::IS_SIMULATION);
  if ( isSim ) {
    ATH_MSG_DEBUG ("It is an MC event... not vetoing...");
    this->setFilterPassed(true);
    return StatusCode::SUCCESS;
  }


  // Now make the event decision
  bool passEvent(true);
  if ( m_useLArError.value()  && eventInfo->errorState(xAOD::EventInfo::LAr)  == xAOD::EventInfo::Error ){ passEvent = false; }
  if ( m_useTileError.value() && eventInfo->errorState(xAOD::EventInfo::Tile) == xAOD::EventInfo::Error ){ passEvent = false; }
  if ( m_useSCTError.value()  && eventInfo->errorState(xAOD::EventInfo::SCT) == xAOD::EventInfo::Error ){ passEvent = false; }
  if ( m_useCoreError.value() && eventInfo->isEventFlagBitSet(xAOD::EventInfo::Core, 18)  ){ passEvent = false; }

  // Set the final decision
  this->setFilterPassed(passEvent);

  return StatusCode::SUCCESS;
}
