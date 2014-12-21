/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODMuon/versions/MuonAuxContainer_v1.h"
 
namespace xAOD {
 
   MuonAuxContainer_v1::MuonAuxContainer_v1()
      : AuxContainerBase() {

        // iParticle
        AUX_VARIABLE( pt );
        AUX_VARIABLE( eta );
        AUX_VARIABLE( phi );
        AUX_VARIABLE(charge);


        // Muon summary
        AUX_VARIABLE( allAuthors );
        AUX_VARIABLE( author );
        AUX_VARIABLE( muonType );
        AUX_VARIABLE( quality ); //quality, passesIDCuts and passesHighPtCuts are packed in here.

        // Energyloss
        AUX_VARIABLE( energyLossType );

        // Muon-specific track summary (the other track summary information is retrieved from the TrackParticle)
        AUX_VARIABLE( numberOfPrecisionLayers );
        AUX_VARIABLE( numberOfPrecisionHoleLayers );
        AUX_VARIABLE( numberOfPhiLayers );
        AUX_VARIABLE( numberOfPhiHoleLayers );
        AUX_VARIABLE( numberOfTriggerEtaLayers );
        AUX_VARIABLE( numberOfTriggerEtaHoleLayers );
        AUX_VARIABLE( primarySector );
        AUX_VARIABLE( secondarySector );

        AUX_VARIABLE( innerSmallHits );
        AUX_VARIABLE( innerLargeHits );
        AUX_VARIABLE( middleSmallHits );
        AUX_VARIABLE( middleLargeHits );
        AUX_VARIABLE( outerSmallHits );
        AUX_VARIABLE( outerLargeHits );
        AUX_VARIABLE( extendedSmallHits );
        AUX_VARIABLE( extendedLargeHits );

        AUX_VARIABLE( innerSmallHoles );
        AUX_VARIABLE( innerLargeHoles );
        AUX_VARIABLE( middleSmallHoles );
        AUX_VARIABLE( middleLargeHoles );
        AUX_VARIABLE( outerSmallHoles );
        AUX_VARIABLE( outerLargeHoles );
        AUX_VARIABLE( extendedSmallHoles );
        AUX_VARIABLE( extendedLargeHoles );

        AUX_VARIABLE( phiLayer1Hits );
        AUX_VARIABLE( phiLayer2Hits );
        AUX_VARIABLE( phiLayer3Hits );
        AUX_VARIABLE( phiLayer4Hits );

        AUX_VARIABLE( etaLayer1Hits );
        AUX_VARIABLE( etaLayer2Hits );
        AUX_VARIABLE( etaLayer3Hits );
        AUX_VARIABLE( etaLayer4Hits );

        AUX_VARIABLE( phiLayer1Holes );
        AUX_VARIABLE( phiLayer2Holes );
        AUX_VARIABLE( phiLayer3Holes );
        AUX_VARIABLE( phiLayer4Holes );

        AUX_VARIABLE( etaLayer1Holes );
        AUX_VARIABLE( etaLayer2Holes );
        AUX_VARIABLE( etaLayer3Holes );
        AUX_VARIABLE( etaLayer4Holes );

        // Isolation
        AUX_VARIABLE( etcone20 );
        AUX_VARIABLE( etcone30 );
        AUX_VARIABLE( etcone40 );
        AUX_VARIABLE( ptcone20 );
        AUX_VARIABLE( ptcone30 );
        AUX_VARIABLE( ptcone40 );

        // Links
        AUX_VARIABLE( inDetTrackParticleLink );
        AUX_VARIABLE( muonSpectrometerTrackParticleLink );
        AUX_VARIABLE( combinedTrackParticleLink );
        AUX_VARIABLE( clusterLink );
        AUX_VARIABLE( muonSegmentLinks );
   }
 
} // namespace xAOD
