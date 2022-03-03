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
   /// $Revision: 631475 $
   /// $Date: 2014-11-27 12:53:05 +0100 (Thu, 27 Nov 2014) $
   ///
   class BTaggingAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      BTaggingAuxContainer_v1();

   private:

     std::vector<float> SV0_significance3D;
     //std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer > > > SV0_TrackParticles;

     std::vector<float> SV1_pb;
     std::vector<float> SV1_pu;
     std::vector<float> SV1_pc;
     //std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer > > > SV1_TrackParticles;

     std::vector<float> IP2D_pb;
     std::vector<float> IP2D_pu;
     std::vector<float> IP2D_pc;
     //std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer > > > IP2D_TrackParticles;
     
     std::vector<float> IP3D_pb;
     std::vector<float> IP3D_pu;
     std::vector<float> IP3D_pc;
     //std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer > > > IP3D_TrackParticles;

     std::vector<float> JetFitter_pb;
     std::vector<float> JetFitter_pu;
     std::vector<float> JetFitter_pc;

     std::vector<float> JetFitterCombNN_pb;
     std::vector<float> JetFitterCombNN_pu;
     std::vector<float> JetFitterCombNN_pc;

     std::vector<float> MV1_discriminant;

   }; // class BTaggingAuxContainer_v1

} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::BTaggingAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODBTAGGING_VERSIONS_BTAGGINGAUXCONTAINER_V1_H
