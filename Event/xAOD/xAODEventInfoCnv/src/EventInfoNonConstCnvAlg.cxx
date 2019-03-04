//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "EventInfoNonConstCnvAlg.h"

// EDM include(s).
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"

#include "GaudiKernel/ConcurrencyFlags.h"

// System include(s).
#include <memory>

namespace xAODMaker {

   EventInfoNonConstCnvAlg::EventInfoNonConstCnvAlg( const std::string& name,
                                                     ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ) {

      // Declare the algorithm's properties.
      declareProperty( "EventInfoKey", m_key = "EventInfo",
                       "Event store key for the xAOD::EventInfo object to "
                       "modify" );
   }

  StatusCode EventInfoNonConstCnvAlg::initialize()
  {
    if(Gaudi::Concurrency::ConcurrencyFlags::numThreads()>0) {
      ATH_MSG_FATAL("EventInfoNonConstCnvAlg can be used ONLY in serial Athena!");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

   StatusCode EventInfoNonConstCnvAlg::execute() {

      // Check if xAOD::EventInfo is available.
      if( ! evtStore()->contains< xAOD::EventInfo >( m_key ) ) {
         // If not, then let's just stop right away.
         ATH_MSG_VERBOSE( "No xAOD::EventInfo with key \"" << m_key << "\"" );
         return StatusCode::SUCCESS;
      }

      // Retrieve the const object.
      const xAOD::EventInfo* ei = nullptr;
      ATH_CHECK( evtStore()->retrieve( ei, m_key ) );

      // Check if it's already set up to be modifiable.
      if( ei->getStore() ) {
         // If so, let's assume that SG will also let the object be retrieved
         // as a non-const pointer.
         ATH_MSG_VERBOSE( "xAOD::EventInfo with key \"" << m_key
                          << "\" seems to be modifiable -> not touching it" );
         return StatusCode::SUCCESS;
      }

      // Access the aux container as well now.
      const xAOD::EventAuxInfo* aux = nullptr;
      ATH_CHECK( evtStore()->retrieve( aux, m_key + "Aux." ) );

      // Make a deep copy of the object.
      auto newei = std::make_unique< xAOD::EventInfo >();
      auto newaux = std::make_unique< xAOD::EventAuxInfo >();
      newei->setStore( newaux.get() );
      *newei = *ei;
      ATH_MSG_VERBOSE( "Made a deep copy of \"" << m_key << "\"" );

      // Overwrite the original object with its deep copy.
      static const bool ALLOW_MODS = true;
      ATH_CHECK( evtStore()->overwrite( std::move( newei ), m_key,
                                        ALLOW_MODS ) );
      ATH_CHECK( evtStore()->overwrite( std::move( newaux ), m_key + "Aux.",
                                        ALLOW_MODS ) );
      ATH_MSG_VERBOSE( "Overwrote \"" << m_key << "\" with its deep copy" );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
