// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2CombinedMuonAuxContainer_v1.h 605591 2014-07-09 12:07:31Z krasznaa $
#ifndef XAODTRIGMUON_VERSIONS_L2COMBINEDMUONAUXCONTAINER_V1_H
#define XAODTRIGMUON_VERSIONS_L2COMBINEDMUONAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"

// xAOD include(s):
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"

namespace xAOD {

   /// Auxiliary container for CB muon containers
   ///
   /// This is a fixed implementation of the CB muon auxiliary store.
   ///
   /// @author Stefano Giagu <stefano.giagu@cern.ch>
   ///
   /// V1.0 2014-06-26 
   ///
   class L2CombinedMuonAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      L2CombinedMuonAuxContainer_v1();

   private:
      /// @name 4-momentum variables
      /// @{
      std::vector< float > pt;
      std::vector< float > phi;
      std::vector< float > eta;
      /// @}

      /// @name L2 muon specific variables
      /// @{
      std::vector< float >    charge;
      std::vector< int >      strategy;
      std::vector< int >      errorFlag;
      std::vector< int >      matchFlag;
      std::vector< float >    sigmaPt;
      std::vector<ElementLink< xAOD::TrackParticleContainer > > idTrackLink;
      std::vector<ElementLink< xAOD::L2StandAloneMuonContainer > > muSATrackLink;
      /// @}

   }; // class L2CombinedMuonAuxContainer_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::L2CombinedMuonAuxContainer_v1 , 1201627828 , 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::L2CombinedMuonAuxContainer_v1, xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE

#endif // XAODTRIGMUON_VERSIONS_L2COMBINEDMUONAUXCONTAINER_V1_H
