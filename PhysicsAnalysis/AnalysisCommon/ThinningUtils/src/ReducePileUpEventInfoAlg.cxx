/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************************************
Name:        ReducePileUpEventInfoAlg

Author:      Karsten Koeneke (CERN)
Created:     February 2013

Description: Algorithm to create a new EventInfo object (and record it into StoreGate) 
             from an existing PileUpEventInfo object and in the process stripping away all
             EventInfo information from pileup events (keeping only the hard scatter).
******************************************************************************/

// This class' header
#include "ReducePileUpEventInfoAlg.h"

// EventInfo includes
#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/EventInfo.h"

// STL includes
#include <string>



//=============================================================================
// Constructor
//=============================================================================
ReducePileUpEventInfoAlg::ReducePileUpEventInfoAlg( const std::string& name,
                                                    ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_nEventsProcessed(0)
{
  // Properties go here
  declareProperty("ReducedEventInfoName",  m_eiKey = "", "Name of the new EventInfo object" );
}



//=============================================================================
// Destructor
//=============================================================================
ReducePileUpEventInfoAlg::~ReducePileUpEventInfoAlg()
{
}



StatusCode ReducePileUpEventInfoAlg::initialize()
{
  // Print the used configuration
  ATH_MSG_DEBUG ( "==> initialize " << name() << "..." );
  // Print out the used configuration
  ATH_MSG_DEBUG ( " using ReducedEventInfoName = " << m_eiKey );
  
  // Sanity checks
  if ( m_eiKey.empty() )
    {
      ATH_MSG_FATAL( "You have to set the property 'ReducedEventInfoName' of this algorithm to some new name for the new EventInfo object!" );
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
}





//=============================================================================
// Athena execute event method
//=============================================================================
StatusCode ReducePileUpEventInfoAlg::execute() 
{
  ++m_nEventsProcessed;
  // Simple status message at the beginning of each event execute
  ATH_MSG_DEBUG ( "==> execute " << name() << " on " << m_nEventsProcessed << ". event..." );
  
  // Check if we find the input EventInfo object
  if ( evtStore()->contains< PileUpEventInfo >( "" ) )
    {
      // Get the EventInfo object
      const PileUpEventInfo* oldEventInfo(nullptr);
      ATH_CHECK( evtStore()->retrieve( oldEventInfo ) );

      // Create a new EventInfo object and register it into StoreGate
      EventInfo* newEventInfo = new EventInfo(*oldEventInfo);
      ATH_CHECK( evtStore()->record ( newEventInfo, m_eiKey , false, false) );
      ATH_MSG_DEBUG ( "Done creating a new EventInfo object and recording it in StoreGate with the name '" << m_eiKey << "'." );
    }
  else
    {
      ATH_MSG_DEBUG ( "Didn't find the input EventInfo object" );
    }

  return StatusCode::SUCCESS;
}



//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode ReducePileUpEventInfoAlg::finalize() 
{
  // Print messages
  ATH_MSG_DEBUG ( "==> finalize " << name() << "..." );
  ATH_MSG_DEBUG ( "***************************************************************" );
  ATH_MSG_DEBUG ( "Results of " << name() << " thinning algorithm:" );
  ATH_MSG_DEBUG ( "-------------" );
  ATH_MSG_DEBUG ( " Number of processed events:  " << m_nEventsProcessed );
    
  return StatusCode::SUCCESS;
}



