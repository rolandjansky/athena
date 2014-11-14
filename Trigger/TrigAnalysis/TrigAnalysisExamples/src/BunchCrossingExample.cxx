/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchCrossingExample.cxx 628392 2014-11-14 12:40:08Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// Local include(s):
#include "BunchCrossingExample.h"

namespace Trig {

   /**
    * The constructor specifies a meaningful default for the ToolHandle,
    * but in general the setup of the tool should always be done on in the
    * jobOptions, because most tool implementations can't work "out of the
    * box". They need to be configured by their special configurables.
    */
   BunchCrossingExample::BunchCrossingExample( const std::string& name,
                                               ISvcLocator* svcloc )
      : AthAlgorithm( name, svcloc ),
        m_bcTool( "Trig::TrigConfBunchCrossingTool/BunchCrossingTool" ) {

      declareProperty( "BCTool", m_bcTool );
   }

   /**
    * The tool always needs to be retrieved before the event loop starts.
    * Most tool implementations rely on receiving various incidents, or getting
    * called by some callback function. So only instantiating the tool inside
    * the event loop would have adverse effects.
    */
   StatusCode BunchCrossingExample::initialize() {

      ATH_MSG_INFO( "Initializing..." );

      // Retrieve the tool:
      CHECK( m_bcTool.retrieve() );
      ATH_MSG_DEBUG( "Retrieved the bunch crossing tool" );

      return StatusCode::SUCCESS;
   }

   /**
    * Here I show a few things that one can ask the tool...
    */
   StatusCode BunchCrossingExample::execute() {

      // Retrieve the object holding the BCID of the current event:
      const EventInfo* eventInfo = 0;
      CHECK( evtStore()->retrieve( eventInfo ) );

      // Extract the BCID of the current event:
      EventID::number_type bcid = eventInfo->event_ID()->bunch_crossing_id();
      ATH_MSG_INFO( "The BCID of the current event is: " << bcid );

      // Check if this was a filled bunch crossing. Should be in most cases...
      if( m_bcTool->isFilled( bcid ) ) {
         ATH_MSG_INFO( "This was a filled bunch crossing" );

         // Check if the BCID is found to be in a bunch train by the tool:
         if( m_bcTool->isInTrain( bcid ) ) {
            ATH_MSG_INFO( "This BCID was part of a bunch train" );

            // Check the BCID's surroundings:
            std::vector< bool > b_in_front = m_bcTool->bunchesInFront( bcid, 32 );
            ATH_MSG_INFO( "Buch filled states in front of the current BCID: "
                          << b_in_front );

            // Get the tool to calculate the BCID's distance from the front of the
            // bunch train. But remember that these numbers on their own usually
            // don't mean much...
            ATH_MSG_INFO( "This bunch was "
                          << m_bcTool->distanceFromFront( bcid,
                                                          IBunchCrossingTool::NanoSec )
                          << " nanoseconds from the front of its train" );
            ATH_MSG_INFO( "Also, it was "
                          << m_bcTool->distanceFromFront( bcid,
                                                          IBunchCrossingTool::BunchCrossings )
                          << " bunch crossings away from the front of its train" );

         } else {
            ATH_MSG_INFO( "This BCID was a single bunch" );
         }

      } else {
         ATH_MSG_INFO( "You've found an event from an empty bunch crossing" );
         ATH_MSG_INFO( "This can happen, but isn't the norm." );
      }

      //
      // Finally, let's interrogate some fixed BCIDs:
      //
      printInfo( 1 );
      printInfo( 7 );
      printInfo( 10 );

      return StatusCode::SUCCESS;
   }

   /**
    * This function prints quite detailed information about a specific bunch crossing.
    *
    * @param bcid The BCID which should be interrogated
    */
   void BunchCrossingExample::printInfo( unsigned int bcid ) {

      ATH_MSG_DEBUG( "bcType( " << bcid << " ) = " << m_bcTool->bcType( bcid ) );
      ATH_MSG_DEBUG( "isFilled( " << bcid << " ) = " << m_bcTool->isFilled( bcid ) );
      ATH_MSG_DEBUG( "isInTrain( " << bcid << " ) = " << m_bcTool->isInTrain( bcid ) );
      ATH_MSG_DEBUG( "distanceFromFront( " << bcid << ", NanoSec ) = "
                     << m_bcTool->distanceFromFront( bcid, IBunchCrossingTool::NanoSec ) );
      ATH_MSG_DEBUG( "distanceFromTail( " << bcid << ", NanoSec ) = "
                     << m_bcTool->distanceFromTail( bcid, IBunchCrossingTool::NanoSec ) );
      ATH_MSG_DEBUG( "distanceFromFront( " << bcid << ", BunchCrossings ) = "
                     << m_bcTool->distanceFromFront( bcid, IBunchCrossingTool::BunchCrossings ) );
      ATH_MSG_DEBUG( "distanceFromTail( " << bcid << ", BunchCrossings ) = "
                     << m_bcTool->distanceFromTail( bcid, IBunchCrossingTool::BunchCrossings ) );
      ATH_MSG_DEBUG( "distanceFromFront( " << bcid << ", FilledBunches ) = "
                     << m_bcTool->distanceFromFront( bcid, IBunchCrossingTool::FilledBunches ) );
      ATH_MSG_DEBUG( "distanceFromTail( " << bcid << ", FilledBunches ) = "
                     << m_bcTool->distanceFromTail( bcid, IBunchCrossingTool::FilledBunches ) );

      return;
   }

} // namespace Trig
