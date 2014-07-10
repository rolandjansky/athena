// Dear emacs, this is -*- c++ -*- $Id: BTaggingAuxContainer_v1.h

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// 585894 2014-03-03 12:47:35Z aad $
#ifndef XAODBTAGGING_VERSIONS_BTAGGINGAUXCONTAINER_V1_H
#define XAODBTAGGING_VERSIONS_BTAGGINGAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace xAOD {

   /// Temporary container used until we have I/O for AuxStoreInternal
   ///
   /// This class is meant to serve as a temporary way to provide an auxiliary
   /// store with Athena I/O capabilities for the BTagging EDM. Will be exchanged for
   /// a generic auxiliary container type (AuxStoreInternal) later on.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 602796 $
   /// $Date: 2014-06-19 23:49:07 +0200 (Thu, 19 Jun 2014) $
   ///
   class BTaggingAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      BTaggingAuxContainer_v1();

   private:

     std::vector<double> SV0_significance3D;
     //std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer > > > SV0_TrackParticles;

     std::vector<double> SV1_pb;
     std::vector<double> SV1_pu;
     std::vector<double> SV1_pc;
     //std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer > > > SV1_TrackParticles;

     std::vector<double> IP2D_pb;
     std::vector<double> IP2D_pu;
     std::vector<double> IP2D_pc;
     //std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer > > > IP2D_TrackParticles;
     
     std::vector<double> IP3D_pb;
     std::vector<double> IP3D_pu;
     std::vector<double> IP3D_pc;
     //std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer > > > IP3D_TrackParticles;

     std::vector<double> JetFitter_pb;
     std::vector<double> JetFitter_pu;
     std::vector<double> JetFitter_pc;

     std::vector<double> JetFitterCombNN_pb;
     std::vector<double> JetFitterCombNN_pu;
     std::vector<double> JetFitterCombNN_pc;

     std::vector<double> MV1_discriminant;

   }; // class BTaggingAuxContainer_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::BTaggingAuxContainer_v1, 1243532533, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::BTaggingAuxContainer_v1, xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE

#endif // XAODBTAGGING_VERSIONS_BTAGGINGAUXCONTAINER_V1_H
