/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoCnvAlg.cxx 751296 2016-06-01 08:00:25Z krasznaa $

// System include(s):
#include <memory>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ConcurrencyFlags.h"

// EDM include(s):
#include "EventInfo/EventInfo.h"
#include "EventInfo/PileUpEventInfo.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include "xAODEventInfo/EventInfoContainer.h"
#include "xAODEventInfo/EventInfoAuxContainer.h"

// Local include(s):
#include "EventInfoCnvAlg.h"

namespace xAODMaker {

   EventInfoCnvAlg::EventInfoCnvAlg( const std::string& name,
                                     ISvcLocator* svcLoc )
      : AthReentrantAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::EventInfoCnvTool/EventInfoCnvTool", this ) {

      declareProperty( "AODKey", m_aodKey = "" );
      declareProperty( "xAODKey", m_xaodKey = "EventInfo" );
      declareProperty( "PileupKey", m_pileupKey = "" );
      declareProperty( "CnvTool", m_cnvTool );
      declareProperty( "DoBeginRun", m_doBeginRun = true );
   }

   StatusCode EventInfoCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " AOD Key: " << m_aodKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey.key() );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      if (m_pileupKey.key().empty()) {
        m_pileupKey = "Pileup" + m_xaodKey.key();
      }

      CHECK( m_aodKey.initialize(SG::AllowEmpty) );
      CHECK( m_xaodKey.initialize() );

      /// FIXME: Should do this only if we have a PileUpEventInfo.
      CHECK( m_pileupKey.initialize() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode EventInfoCnvAlg::execute (const EventContext& ctx) const {

      // Check if anything needs to be done:
      // FIXME: Job configuration should be fixed so we don't need this test.
      if( evtStore()->contains< xAOD::EventInfo >( m_xaodKey.key() ) ) {
        ATH_MSG_WARNING( "xAOD::EventInfo with key \"" << m_xaodKey.key()
                          << "\" is already in StoreGate; "
                          << "EventInfoCnvAlg should not be scheduled.");
         return StatusCode::SUCCESS;
      }

      // Retrieve the AOD object:
      // FIXME: Use a ReadHandle.
      const EventInfo* aod = 0;
      if( m_aodKey.empty() ) {
         // If key has not been set, do a keyless retrieve instead.
         // This is not standard behavior, but is for compatibility
         // with existing configurations.
         CHECK( evtStore()->retrieve( aod ) );
      } else {
         SG::ReadHandle<EventInfo> ei (m_aodKey, ctx);
         aod = ei.cptr();
      }

      // Create/record the xAOD object(s):
      SG::WriteHandle<xAOD::EventInfo> ei (m_xaodKey, ctx);
      CHECK( ei.record (std::make_unique<xAOD::EventInfo>(),
                        std::make_unique<xAOD::EventAuxInfo>()) );

      // Do the translation:
      CHECK( m_cnvTool->convert( aod, ei.ptr() ) );

      // Check if this is a PileUpEventInfo object:
      const PileUpEventInfo* paod =
         dynamic_cast< const PileUpEventInfo* >( aod );
      // If not, then we're done already:
      if( ! paod ) {
         return StatusCode::SUCCESS;
      }

      // Create an EventInfoContainer for the pileup events:
      SG::WriteHandle<xAOD::EventInfoContainer> puei (m_pileupKey, ctx);
      CHECK( puei.record (std::make_unique<xAOD::EventInfoContainer>(),
                          std::make_unique<xAOD::EventInfoAuxContainer>()) );

      // Sub-events for the main EventInfo object:
      std::vector< xAOD::EventInfo::SubEvent > subEvents;

      // A convenience type declaration:
      typedef ElementLink< xAOD::EventInfoContainer > EiLink;

      // Create xAOD::EventInfo objects for each pileup EventInfo object:
      PileUpEventInfo::SubEvent::const_iterator pu_itr = paod->beginSubEvt();
      PileUpEventInfo::SubEvent::const_iterator pu_end = paod->endSubEvt();
      for( ; pu_itr != pu_end; ++pu_itr ) {
         // Create a new xAOD object:
         xAOD::EventInfo* ei = new xAOD::EventInfo();
         puei->push_back( ei );
         // Fill it with information:
         CHECK( m_cnvTool->convert( pu_itr->pSubEvt, ei, true, false ) );
         // And now add a sub-event to the temporary list:
         xAOD::EventInfo::PileUpType type = xAOD::EventInfo::Unknown;
         switch (pu_itr->type()) {
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
         case PileUpTimeEventIndex::ZeroBias:
           type = xAOD::EventInfo::ZeroBias;
           break;
         default:
           break;
         }
         subEvents.push_back( xAOD::EventInfo::SubEvent( pu_itr->time(),
                                                         pu_itr->index(),
                                                         type,
                                                         EiLink( m_pileupKey.key(),
                                                                 puei->size() -
                                                                 1 ) ) );
      }

      // And now update the main EventInfo object with the sub-events:
      ei->setSubEvents( subEvents );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode EventInfoCnvAlg::beginRun() {

     if(m_doBeginRun && !Gaudi::Concurrency::ConcurrencyFlags::concurrent()) {
       // Let the user know what's happening:
       ATH_MSG_DEBUG( "Preparing xAOD::EventInfo object in beginRun()" );
       
       // Run the conversion using the execute function:
       CHECK( execute (Gaudi::Hive::currentContext()) );
     }
     else {
       // Supress warning about use of beginRun().
#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
       CHECK( Algorithm::beginRun() );
#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif
     }

     // Return gracefully:
     return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
