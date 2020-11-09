//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "xAODRootAccess/LoadDictionaries.h"
#include "xAODRootAccess/tools/Message.h"

// ROOT include(s).
#include "TClass.h"
#include "TError.h"

/// Helper macro for loading xAOD dictionaries.
#define LOAD_TYPE(NAME)                                                        \
   do {                                                                        \
      TClass* cl = TClass::GetClass( NAME );                                   \
      if( cl == nullptr ) {                                                    \
         Error( "xAOD::LoadDictionaries",                                      \
                XAOD_MESSAGE( "Failed to load the dictionary for: %s" ),       \
                NAME );                                                        \
         return xAOD::TReturnCode::kFailure;                                   \
      }                                                                        \
   } while( false )

namespace xAOD {

   TReturnCode LoadDictionaries() {

      // Load the libraries in a carefully selected order.
      LOAD_TYPE( "xAOD::TruthParticle_v1" );
      LOAD_TYPE( "xAOD::MuonRoI_v1" );
      LOAD_TYPE( "xAOD::CaloCluster_v1" );
      LOAD_TYPE( "xAOD::TrackParticle_v1" );
      LOAD_TYPE( "xAOD::Electron_v1" );
      LOAD_TYPE( "xAOD::Muon_v1" );
      LOAD_TYPE( "xAOD::Jet_v1" );
      LOAD_TYPE( "xAOD::TauJet_v1" );
      LOAD_TYPE( "xAOD::PFO_v1" );
      LOAD_TYPE( "xAOD::TrigElectron_v1" );
      LOAD_TYPE( "xAOD::L2CombinedMuon_v1" );

      // Tell the user what happened.
      Info( "xAOD::LoadDictionaries", "xAOD EDM dictionaries loaded" );

      // Return gracefully.
      return TReturnCode::kSuccess;
   }

} // namespace xAOD
