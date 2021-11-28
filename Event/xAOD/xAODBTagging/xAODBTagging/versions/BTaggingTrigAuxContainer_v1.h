// Dear emacs, this is -*- c++ -*- $Id: BTaggingTrigAuxContainer_v1.h

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// 585894 2014-03-03 12:47:35Z aad $
#ifndef XAODBTAGGING_VERSIONS_BTAGGINGTRIGAUXCONTAINER_V1_H
#define XAODBTAGGING_VERSIONS_BTAGGINGTRIGAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODBase/IParticleContainer.h"
//#include "xAODVertex/VertexContainer.h"
#include "xAODBTagging/BTagVertexContainer.h"


namespace xAOD {

   /// Temporary container used until we have I/O for AuxStoreInternal
   ///
   /// This class is meant to serve as a temporary way to provide an auxiliary
   /// store with Athena I/O capabilities for the BTagging EDM. Will be exchanged for
   /// a generic auxiliary container type (AuxStoreInternal) later on.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 692288 $
   /// $Date: 2015-08-31 18:22:48 +0200 (Mon, 31 Aug 2015) $
   ///
   class BTaggingTrigAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      BTaggingTrigAuxContainer_v1();

   private:

     std::vector< float > SV0_significance3D;
     //std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer > > > SV0_TrackParticles;

     std::vector< float > SV1_pb;
     std::vector< float > SV1_pu;
     std::vector< float > SV1_pc;
     //std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer > > > SV1_TrackParticles;

     std::vector< float > IP2D_pb;
     std::vector< float > IP2D_pu;
     std::vector< float > IP2D_pc;
     //std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer > > > IP2D_TrackParticles;
     
     std::vector< float > IP3D_pb;
     std::vector< float > IP3D_pu;
     std::vector< float > IP3D_pc;
     //std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer > > > IP3D_TrackParticles;

     std::vector< float > JetFitter_pb;
     std::vector< float > JetFitter_pu;
     std::vector< float > JetFitter_pc;

     std::vector< float > JetFitterCombNN_pb;
     std::vector< float > JetFitterCombNN_pu;
     std::vector< float > JetFitterCombNN_pc;

     std::vector< float > MV1_discriminant;

     // Add the dynamic variables from BTaggingAuxContainer here as static variables
     // (otherwise this information isn't available in the trigger)

     std::vector< float > MV2c00_discriminant;
     std::vector< float > MV2c10_discriminant;
     std::vector< float > MV2c20_discriminant;

     std::vector< std::vector< float > > IP2D_valD0wrtPVofTracks;
     std::vector< std::vector< float > > IP2D_sigD0wrtPVofTracks;
     std::vector< std::vector< float > > IP2D_weightBofTracks;
     std::vector< std::vector< float > > IP2D_weightUofTracks;
     std::vector< std::vector< float > > IP2D_weightCofTracks;
     std::vector< std::vector< bool  > > IP2D_flagFromV0ofTracks;
     std::vector< std::vector< int   > > IP2D_gradeOfTracks;

     std::vector< std::vector< float > > IP3D_valD0wrtPVofTracks;
     std::vector< std::vector< float > > IP3D_sigD0wrtPVofTracks;
     std::vector< std::vector< float > > IP3D_valZ0wrtPVofTracks;
     std::vector< std::vector< float > > IP3D_sigZ0wrtPVofTracks;
     std::vector< std::vector< float > > IP3D_weightBofTracks;
     std::vector< std::vector< float > > IP3D_weightUofTracks;
     std::vector< std::vector< float > > IP3D_weightCofTracks;
     std::vector< std::vector< bool  > > IP3D_flagFromV0ofTracks;
     std::vector< std::vector< int   > > IP3D_gradeOfTracks;

     std::vector< float >   SV0_normdist;
     std::vector< float >  SV0_pb;
     std::vector< float >  SV0_pu;
     std::vector< float >  SV0_pc;
     std::vector< float >   SV0_energyTrkInJet;
     std::vector< float >   SV0_masssvx;
     std::vector< float >   SV0_efracsvx;
     std::vector< int >     SV0_N2Tpair;
     std::vector< int >     SV0_NGTinSvx;
     std::vector< std::vector< ElementLink< xAOD::VertexContainer        > > > SV0_vertices;
     std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer > > > SV0_badTracksIP;

     std::vector< float >   SV1_normdist;
     std::vector< float >   SV1_masssvx;
     std::vector< float >   SV1_efracsvx;
     std::vector< int >     SV1_N2Tpair;
     std::vector< float >   SV1_significance3d;
     std::vector< float >   SV1_energyTrkInJet;
     std::vector< int >     SV1_NGTinSvx;
     std::vector< std::vector< ElementLink< xAOD::VertexContainer        > > > SV1_vertices;
     std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer > > > SV1_badTracksIP;

     std::vector< int >     JetFitter_N2Tpair;
     std::vector< float >   JetFitter_massUncorr;
     std::vector< float >   JetFitter_chi2;
     std::vector< int >     JetFitter_ndof;
     std::vector< int >     JetFitter_nVTX;
     std::vector< int >     JetFitter_nSingleTracks;
     std::vector< int >     JetFitter_nTracksAtVtx;
     std::vector< float >   JetFitter_mass;
     std::vector< float >   JetFitter_energyFraction;
     std::vector< float >   JetFitter_significance3d;
     std::vector< float >   JetFitter_deltaeta;
     std::vector< float >   JetFitter_deltaphi;
     std::vector< std::vector< float > > JetFitter_fittedPosition;
     std::vector< std::vector< float > > JetFitter_fittedCov;
     std::vector< std::vector< float > > JetFitter_tracksAtPVchi2;
     std::vector< std::vector< float > > JetFitter_tracksAtPVndf;
     std::vector< std::vector< ElementLink< BTagVertexContainer    > > > JetFitter_JFvertices;
     std::vector< std::vector< ElementLink< TrackParticleContainer > > > JetFitter_tracksAtPVlinks;

     std::vector< float >        trkSum_VEta;
     std::vector< unsigned int > trkSum_ntrk;
     std::vector< float >        trkSum_SPt;
     std::vector< float >        trkSum_VPt;

     std::vector< std::vector< ElementLink< TrackParticleContainer > > > SV0_TrackParticleLinks;
     std::vector< std::vector< ElementLink< TrackParticleContainer > > > SV1_TrackParticleLinks;
     std::vector< std::vector< ElementLink< TrackParticleContainer > > > IP2D_TrackParticleLinks;
     std::vector< std::vector< ElementLink< TrackParticleContainer > > > IP3D_TrackParticleLinks;
     std::vector< std::vector< ElementLink< TrackParticleContainer > > > BTagTrackToJetAssociator;
     //std::vector< std::vector< ElementLink< JetContainer           > > > BTagBtagToJetAssociator; 
     std::vector< std::vector< ElementLink< IParticleContainer     > > > BTagBtagToJetAssociator; 
     
   }; // class BTaggingTrigAuxContainer_v1

} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::BTaggingTrigAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODBTAGGING_VERSIONS_BTAGGINGTRIGAUXCONTAINER_V1_H
