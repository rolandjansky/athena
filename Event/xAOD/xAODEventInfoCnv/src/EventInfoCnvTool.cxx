/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoCnvTool.cxx 793565 2017-01-23 22:00:14Z leggett $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/PileUpEventInfo.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventInfoContainer.h"

// Local include(s):
#include "EventInfoCnvTool.h"

namespace xAODMaker {

  // maps to copy the flags
  static const std::map< xAOD::EventInfo::EventFlagSubDet,
                         EventInfo::EventFlagSubDet > subDetMap = 
    { {xAOD::EventInfo::Pixel,      EventInfo::Pixel},
      {xAOD::EventInfo::SCT,        EventInfo::SCT},
      {xAOD::EventInfo::TRT,        EventInfo::TRT},
      {xAOD::EventInfo::LAr,        EventInfo::LAr},
      {xAOD::EventInfo::Tile,       EventInfo::Tile},
      {xAOD::EventInfo::Muon,       EventInfo::Muon},
      {xAOD::EventInfo::ForwardDet, EventInfo::ForwardDet},
      {xAOD::EventInfo::Core,       EventInfo::Core},
      {xAOD::EventInfo::Background, EventInfo::Background},
      {xAOD::EventInfo::Lumi,       EventInfo::Lumi} };

  static const std::map< EventInfo::EventFlagErrorState,
                         xAOD::EventInfo::EventFlagErrorState > errorStateMap =
    { { EventInfo::NotSet,  xAOD::EventInfo::NotSet },
      { EventInfo::Warning, xAOD::EventInfo::Warning },
      { EventInfo::Error,   xAOD::EventInfo::Error}
    };

   /// Hard-coded location of the beam position information
   static const std::string INDET_BEAMPOS = "/Indet/Beampos";

   /// Hard-coded location of the luminosity information
   static const std::string LUMI_FOLDER_RUN11 = "/TRIGGER/OFLLUMI/LBLESTOFL";
   static const std::string LUMI_FOLDER_RUN12 = "/TRIGGER/LUMI/LBLESTONL";
   static const std::string LUMI_FOLDER_RUN21 = "/TRIGGER/OFLLUMI/OflPrefLumi";
   static const std::string LUMI_FOLDER_RUN22 = "/TRIGGER/LUMI/OnlPrefLumi";

   EventInfoCnvTool::EventInfoCnvTool( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
      : AthAlgTool( type, name, parent ),
#ifndef XAOD_ANALYSIS
        m_beamCondSvc( "BeamCondSvc", name ),
        m_lumiTool( "LuminosityTool" ),
#endif
        m_beamCondSvcAvailable( false ),
        m_lumiToolAvailable( false ),
        m_disableBeamSpot( false ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< IEventInfoCnvTool >( this );
#ifndef XAOD_ANALYSIS
      // Declare the tool's properties:
      declareProperty( "BeamCondSvc", m_beamCondSvc );
      declareProperty( "LuminosityTool", m_lumiTool );
      declareProperty( "DisableBeamSpot", m_disableBeamSpot );
#endif
   }

   StatusCode EventInfoCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

#ifndef XAOD_ANALYSIS
      // Check if the beam position will be available or not:
      if( detStore()->contains< AthenaAttributeList >( INDET_BEAMPOS ) ) {
         m_beamCondSvcAvailable = true;
      } else {
         ATH_MSG_WARNING( "Beam conditions service not available" );
         ATH_MSG_WARNING( "Will not fill beam spot information into "
                          "xAOD::EventInfo" );
         m_beamCondSvcAvailable = false;
      }
      if(m_disableBeamSpot){
         ATH_MSG_WARNING( "Beam conditions service manually disabled on EventInfo object" );
         m_beamCondSvcAvailable = false;
      }
      // Try to access the beam conditions service:
      if( m_beamCondSvcAvailable ) {
         CHECK( m_beamCondSvc.retrieve() );
      }

      // Check if the luminosity tool will be available or not:
      if( detStore()->contains< CondAttrListCollection >( LUMI_FOLDER_RUN11 ) ||
          detStore()->contains< CondAttrListCollection >( LUMI_FOLDER_RUN12 ) ||
          detStore()->contains< CondAttrListCollection >( LUMI_FOLDER_RUN21 ) ||
          detStore()->contains< CondAttrListCollection >( LUMI_FOLDER_RUN22 ) ) {
         ATH_MSG_INFO( "Taking luminosity information from: " << m_lumiTool );
         m_lumiToolAvailable = true;
      } else {
         ATH_MSG_INFO( "Luminosity information not available" );
         ATH_MSG_INFO( "Will take information from the EventInfo object" );
         m_lumiToolAvailable = false;
      }

      // Try to access the luminosity tool:
      if( m_lumiToolAvailable ) {
         CHECK( m_lumiTool.retrieve() );
      } else {
        m_lumiTool.disable();
      }
#else
      //do nothing, lumi and beam conditions not available

#endif

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the EventInfo object
    * available in Athena, and fills an xAOD::EventInfo object with its contents.
    *
    * @param aod The AOD's EventInfo object
    * @param xaod The xAOD::EventInfo object to fill
    * @param pileUpInfo <code>true</code> for pile-up EventInfo objects
    * @param copyPileUpLinks Allows to turn the ElementLink creation on or off
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode EventInfoCnvTool::convert( const EventInfo* aod,
                                         xAOD::EventInfo* xaod,
                                         bool pileUpInfo,
                                         bool copyPileUpLinks ) const {

      if( ! aod ) {
         ATH_MSG_WARNING( "Null pointer received for input!" );
         return StatusCode::SUCCESS;
      }

      // Copy the basic properties into the xAOD object:
      if( aod->event_ID() ) {
         xaod->setRunNumber( aod->event_ID()->run_number() );
         xaod->setEventNumber( aod->event_ID()->event_number() );
         xaod->setLumiBlock( aod->event_ID()->lumi_block() );
         xaod->setTimeStamp( aod->event_ID()->time_stamp() );
         xaod->setTimeStampNSOffset( aod->event_ID()->time_stamp_ns_offset() );
         xaod->setBCID( aod->event_ID()->bunch_crossing_id() );
         xaod->setDetectorMask( aod->event_ID()->detector_mask0(),
                                aod->event_ID()->detector_mask1() );
         xaod->setDetectorMaskExt( aod->event_ID()->detector_mask2(),
                                 aod->event_ID()->detector_mask3() );
      }

      // Copy the event type properties:
      if( aod->event_type() ) {
         EventType::NameTagPairVec detDescrTags;
         aod->event_type()->get_detdescr_tags( detDescrTags );
         xaod->setDetDescrTags( detDescrTags );
         uint32_t eventTypeBitmask = 0;
         if( aod->event_type()->test( EventType::IS_SIMULATION ) ) {
            eventTypeBitmask |= xAOD::EventInfo::IS_SIMULATION;
         }
         if( aod->event_type()->test( EventType::IS_TESTBEAM ) ) {
            eventTypeBitmask |= xAOD::EventInfo::IS_TESTBEAM;
         }
         if( aod->event_type()->test( EventType::IS_CALIBRATION ) ) {
            eventTypeBitmask |= xAOD::EventInfo::IS_CALIBRATION;
         }
         xaod->setEventTypeBitmask( eventTypeBitmask );
         // Only add MC information for simulation files:
         if( xaod->eventType( xAOD::EventInfo::IS_SIMULATION ) ) {
            xaod->setMCChannelNumber( aod->event_type()->mc_channel_number() );
            xaod->setMCEventNumber( aod->event_type()->mc_event_number() );
            std::vector< float >
               eventWeights( aod->event_type()->n_mc_event_weights(), 0.0 );
            for( unsigned int i = 0; i < aod->event_type()->n_mc_event_weights();
                 ++i ) {
               eventWeights[ i ] = aod->event_type()->mc_event_weight( i );
            }
            xaod->setMCEventWeights( eventWeights );
         }
      }

      // Copy the trigger properties into the xAOD object:
      if( aod->trigger_info() && ( ! pileUpInfo ) ) {
         xaod->setStatusElement( aod->trigger_info()->statusElement() );
         xaod->setExtendedLevel1ID( aod->trigger_info()->extendedLevel1ID() );
         xaod->setLevel1TriggerType( aod->trigger_info()->level1TriggerType() );
         std::vector< xAOD::EventInfo::StreamTag > streamTags;
         std::vector< TriggerInfo::StreamTag >::const_iterator st_itr =
            aod->trigger_info()->streamTags().begin();
         std::vector< TriggerInfo::StreamTag >::const_iterator st_end =
            aod->trigger_info()->streamTags().end();
         for( ; st_itr != st_end; ++st_itr ) {
            streamTags.push_back(
                xAOD::EventInfo::StreamTag( st_itr->name(), st_itr->type(),
                                            st_itr->obeysLumiblock(),
                                            st_itr->robs(), st_itr->dets() ) );
         }
         xaod->setStreamTags( streamTags );
      }
 
      // Copy/calculate the pileup information:
      if( ! pileUpInfo ) {
         if( m_lumiToolAvailable ) {
#ifndef XAOD_ANALYSIS
            float actualMu = 0.0;
            const float muToLumi = m_lumiTool->muToLumi();
            if( std::abs( muToLumi ) > 0.00001 ) {
               actualMu = m_lumiTool->lbLuminosityPerBCID() / muToLumi;
            }
            xaod->setActualInteractionsPerCrossing( actualMu );
            xaod->setAverageInteractionsPerCrossing(
               m_lumiTool->lbAverageInteractionsPerCrossing() );
#endif
         } else {
            xaod->setActualInteractionsPerCrossing(
               aod->actualInteractionsPerCrossing() );
            xaod->setAverageInteractionsPerCrossing(
               aod->averageInteractionsPerCrossing() );
         }
      }

      // Copy the sub-detector flags:
      std::map< xAOD::EventInfo::EventFlagSubDet,
                EventInfo::EventFlagSubDet >::const_iterator sd_itr =
         subDetMap.begin();
      std::map< xAOD::EventInfo::EventFlagSubDet,
                EventInfo::EventFlagSubDet >::const_iterator sd_end =
         subDetMap.end();
      for( ; sd_itr != sd_end; ++sd_itr ) {

         // Lumi does not store event flags, or an error state:
        if( (int)sd_itr->first == (int)EventInfo::Lumi ) {
            continue;
         }

         // Set the event flags for this sub-detector:
         xaod->setEventFlags( sd_itr->first, aod->eventFlags( sd_itr->second ) );

         // Look up the error state of this sub-detector:
         std::map< EventInfo::EventFlagErrorState,
                   xAOD::EventInfo::EventFlagErrorState >::const_iterator state =
            errorStateMap.find( aod->errorState( sd_itr->second ) );
         if( state == errorStateMap.end() ) {
            REPORT_MESSAGE( MSG::FATAL )
               << "Unknown error state found for sub-detector "
               << sd_itr->second << ": " << aod->errorState( sd_itr->second );
            return StatusCode::FAILURE;
         }

         // Set the error state for the sub-detector:
         xaod->setErrorState( sd_itr->first, state->second );
      }

      // Check if it is a PileUpEventInfo object:
      const PileUpEventInfo* puei =
         dynamic_cast< const PileUpEventInfo* >( aod );
      if( puei && copyPileUpLinks ) {
         // Create the sub-event objects to fill into the output object:
         std::vector< xAOD::EventInfo::SubEvent > subEvents;
         PileUpEventInfo::SubEvent::const_iterator itr = puei->beginSubEvt();
         PileUpEventInfo::SubEvent::const_iterator end = puei->endSubEvt();
         for( ; itr != end; ++itr ) {
            // Look up the sub-event type:
           
            xAOD::EventInfo::PileUpType type = xAOD::EventInfo::Unknown;
            switch (itr->type()) {
            case PileUpTimeEventIndex::Signal:
              type = xAOD::EventInfo::Signal;
              break;
            case PileUpTimeEventIndex::MinimumBias:
              type = xAOD::EventInfo::MinimumBias;
              break;
            case PileUpTimeEventIndex::Cavern:
              type = xAOD::EventInfo::Cavern;
              break;
            case PileUpTimeEventIndex::HaloGas:
              type = xAOD::EventInfo::HaloGas;
              break;
            case PileUpTimeEventIndex::HighPtMinimumBias:
              type = xAOD::EventInfo::HighPtMinimumBias;
              break;
            case PileUpTimeEventIndex::ZeroBias:
              type = xAOD::EventInfo::ZeroBias;
              break;
            case PileUpTimeEventIndex::Unknown:
              break;
            default:
              ATH_MSG_WARNING( "Unknown sub-event type ("
                               << itr->type() << ") encountered" );
              break;
            }

            // Construct the link to the pile-up EventInfo object:
            ElementLink< xAOD::EventInfoContainer > link;
            link.resetWithKeyAndIndex( "PileUpEventInfo", itr->index() );

            // Add the new object
            subEvents.push_back( xAOD::EventInfo::SubEvent( itr->time(),
                                                            itr->index(),
                                                            type, link ) );
         }

         // Add the vector to the EventInfo:
         xaod->setSubEvents( subEvents );
      }

#ifndef XAOD_ANALYSIS
      // Fill the beam spot variables if the necessary service is available:
      if( m_beamCondSvcAvailable && ( ! pileUpInfo ) ) {
         xaod->setBeamPos( m_beamCondSvc->beamPos()[ Amg::x ],
                           m_beamCondSvc->beamPos()[ Amg::y ],
                           m_beamCondSvc->beamPos()[ Amg::z ] );
         xaod->setBeamPosSigma( m_beamCondSvc->beamSigma( 0 ),
                                m_beamCondSvc->beamSigma( 1 ),
                                m_beamCondSvc->beamSigma( 2 ) );
         xaod->setBeamPosSigmaXY( m_beamCondSvc->beamSigmaXY() );
         xaod->setBeamTiltXZ( m_beamCondSvc->beamTilt( 0 ) );
         xaod->setBeamTiltYZ( m_beamCondSvc->beamTilt( 1 ) );
         xaod->setBeamStatus( m_beamCondSvc->beamStatus() );
      }
#endif

      // Finish with some printout:
      ATH_MSG_VERBOSE( "Finished conversion" << *xaod );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
