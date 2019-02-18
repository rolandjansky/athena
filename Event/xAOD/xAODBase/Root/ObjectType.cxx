/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <iostream>

// Local include(s):
#include "xAODBase/ObjectType.h"

/// Helper macro for printing the object type as a string
#define PRINT_TYPE( TYPE )                      \
   case TYPE:                                   \
      out << #TYPE;                             \
      break

/// This function can be used in (debug) printouts to easily show the type
/// name returned by an object.
///
/// @param out The STL stream to print to
/// @param type The type whose name to print in the stream
/// @returns The same stream that it received
///
std::ostream& operator<< ( std::ostream& out, xAOD::Type::ObjectType type ) {

   switch( type ) {

      PRINT_TYPE( xAOD::Type::Other );

      PRINT_TYPE( xAOD::Type::CaloCluster );
      PRINT_TYPE( xAOD::Type::Jet );
      PRINT_TYPE( xAOD::Type::ParticleFlow );
      PRINT_TYPE( xAOD::Type::TrackParticle );
      PRINT_TYPE( xAOD::Type::NeutralParticle );
      PRINT_TYPE( xAOD::Type::Electron );
      PRINT_TYPE( xAOD::Type::Photon );
      PRINT_TYPE( xAOD::Type::Muon );
      PRINT_TYPE( xAOD::Type::Tau );

      PRINT_TYPE( xAOD::Type::Vertex );
      PRINT_TYPE( xAOD::Type::BTag );

      PRINT_TYPE( xAOD::Type::TruthParticle );
      PRINT_TYPE( xAOD::Type::TruthVertex );
      PRINT_TYPE( xAOD::Type::TruthEvent );
      PRINT_TYPE( xAOD::Type::TruthPileupEvent );

      PRINT_TYPE( xAOD::Type::L2StandAloneMuon );
      PRINT_TYPE( xAOD::Type::L2IsoMuon );
      PRINT_TYPE( xAOD::Type::L2CombinedMuon );

      PRINT_TYPE( xAOD::Type::TrigElectron );
      PRINT_TYPE( xAOD::Type::TrigPhoton );
      PRINT_TYPE( xAOD::Type::TrigCaloCluster );
      PRINT_TYPE( xAOD::Type::TrigEMCluster );

      PRINT_TYPE( xAOD::Type::EventInfo );
      PRINT_TYPE( xAOD::Type::EventFormat );

      PRINT_TYPE( xAOD::Type::Particle );
      PRINT_TYPE( xAOD::Type::CompositeParticle );

   default:
      out << "UNKNOWN";
      break;
   }

   // Return the stream object:
   return out;
}
