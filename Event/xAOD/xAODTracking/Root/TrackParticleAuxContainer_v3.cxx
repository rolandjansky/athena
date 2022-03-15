/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


// Local include(s):
#include "xAODTracking/versions/TrackParticleAuxContainer_v3.h"

namespace xAOD {

   TrackParticleAuxContainer_v3::TrackParticleAuxContainer_v3()
      : AuxContainerBase() {

      AUX_VARIABLE( d0 );
      AUX_VARIABLE( z0 );
      AUX_VARIABLE( phi );
      AUX_VARIABLE( theta );
      AUX_VARIABLE( qOverP );

      AUX_VARIABLE( definingParametersCovMatrix );

      AUX_VARIABLE( vx );
      AUX_VARIABLE( vy );
      AUX_VARIABLE( vz );

      AUX_VARIABLE( radiusOfFirstHit );
      AUX_VARIABLE( identifierOfFirstHit );
      
      AUX_VARIABLE( beamlineTiltX );
      AUX_VARIABLE( beamlineTiltY );

      AUX_VARIABLE( hitPattern );

      AUX_VARIABLE(  numberOfUsedHitsdEdx );
      
      AUX_VARIABLE(  numberOfIBLOverflowsdEdx );

      AUX_VARIABLE( chiSquared          );
      AUX_VARIABLE( numberDoF );

      AUX_VARIABLE( trackFitter          );
      AUX_VARIABLE( particleHypothesis );
      AUX_VARIABLE( trackProperties );
      AUX_VARIABLE( patternRecoInfo );

      // TrackSummary information
#ifndef XAODTRACKPARTICLE_SUMMARYDYNAMIC
      // uint8_ts
      AUX_VARIABLE( numberOfContribPixelLayers        );
      AUX_VARIABLE( numberOfInnermostPixelLayerHits               );
      AUX_VARIABLE( numberOfInnermostPixelLayerOutliers           );
      AUX_VARIABLE( numberOfInnermostPixelLayerSharedHits         );
      AUX_VARIABLE( numberOfInnermostPixelLayerSplitHits          );
      AUX_VARIABLE( expectInnermostPixelLayerHit                  ); 
      AUX_VARIABLE( numberOfNextToInnermostPixelLayerHits               );
      AUX_VARIABLE( numberOfNextToInnermostPixelLayerOutliers           );
      AUX_VARIABLE( numberOfNextToInnermostPixelLayerSharedHits         );
      AUX_VARIABLE( numberOfNextToInnermostPixelLayerSplitHits          );
      AUX_VARIABLE( expectNextToInnermostPixelLayerHit                  );
      AUX_VARIABLE( numberOfPixelHits                 );
      AUX_VARIABLE( numberOfPixelOutliers             );
      AUX_VARIABLE( numberOfPixelHoles                );
      AUX_VARIABLE( numberOfPixelSharedHits           );
      AUX_VARIABLE( numberOfPixelSplitHits            );
      AUX_VARIABLE( numberOfGangedPixels              );
      AUX_VARIABLE( numberOfGangedFlaggedFakes        );
      AUX_VARIABLE( numberOfPixelDeadSensors          );
      AUX_VARIABLE( numberOfPixelSpoiltHits           );
      AUX_VARIABLE( numberOfDBMHits                   );
      AUX_VARIABLE( numberOfSCTHits                   );
      AUX_VARIABLE( numberOfSCTOutliers               );
      AUX_VARIABLE( numberOfSCTHoles                  );
      AUX_VARIABLE( numberOfSCTDoubleHoles            );
      AUX_VARIABLE( numberOfSCTSharedHits             );
      AUX_VARIABLE( numberOfSCTDeadSensors            );
      AUX_VARIABLE( numberOfSCTSpoiltHits             );
      AUX_VARIABLE( numberOfTRTHits                   );
      AUX_VARIABLE( numberOfTRTOutliers               );
      AUX_VARIABLE( numberOfTRTHoles                  );
      AUX_VARIABLE( numberOfTRTHighThresholdHits      );
      AUX_VARIABLE( numberOfTRTHighThresholdHitsTotal );
      AUX_VARIABLE( numberOfTRTHighThresholdOutliers  );
      AUX_VARIABLE( numberOfTRTDeadStraws             );
      AUX_VARIABLE( numberOfTRTTubeHits               );
      AUX_VARIABLE( numberOfTRTXenonHits              );
      AUX_VARIABLE( numberOfTRTSharedHits              );
      AUX_VARIABLE( numberOfPrecisionLayers );
      AUX_VARIABLE( numberOfPrecisionHoleLayers );
      AUX_VARIABLE( numberOfPhiLayers );
      AUX_VARIABLE( numberOfPhiHoleLayers );
      AUX_VARIABLE( numberOfTriggerEtaLayers );
      AUX_VARIABLE( numberOfTriggerEtaHoleLayers );

      AUX_VARIABLE( numberOfOutliersOnTrack           );
      AUX_VARIABLE( standardDeviationOfChi2OS         );
      // Floats
      AUX_VARIABLE( eProbabilityComb                  );
      AUX_VARIABLE( eProbabilityHT                    ); 
      // AUX_VARIABLE( eProbabilityToT                   ); 
      // AUX_VARIABLE( eProbabilityBrem                  );
      AUX_VARIABLE( pixeldEdx                 );
#endif

      //Links
      AUX_VARIABLE( vertexLink );
      AUX_VARIABLE( TRTTrackOccupancy );

   }

   void TrackParticleAuxContainer_v3::dump() const {
     std::cout<<" Dumping TrackParticleAuxContainer_v3"<<std::endl;
     std::cout<<"d0:";
     std::copy(d0.begin(), d0.end(),
       std::ostream_iterator<float>(std::cout, ", "));
     std::cout<<"z0:";
     std::copy(z0.begin(), z0.end(),
       std::ostream_iterator<float>(std::cout, ", "));
     std::cout<<"phi:";
     std::copy(phi.begin(), phi.end(),
       std::ostream_iterator<float>(std::cout, ", "));
     std::cout<<"theta:";
     std::copy(theta.begin(), theta.end(),
       std::ostream_iterator<float>(std::cout, ", "));
     std::cout<<"qOverP:";
     std::copy(qOverP.begin(), qOverP.end(),
       std::ostream_iterator<float>(std::cout, ", "));
     std::cout<<"definingParametersCovMatrix: ";
     for (unsigned int i=0; i<definingParametersCovMatrix.size();++i){
     std::copy(definingParametersCovMatrix[i].begin(), definingParametersCovMatrix[i].end(),
       std::ostream_iterator<float>(std::cout, ", "));
        std::cout<<std::endl;
     }
   }

   /// This function is used to implement simple schema evolution rules that
   /// don't warrant creating a _v3 class yet.
   void TrackParticleAuxContainer_v3::toTransient() {

      // Make sure that the numberOfDBMHits variable is of the right size:
      if( numberOfDBMHits.size() != phi.size() ) {
         numberOfDBMHits.resize( phi.size(), 0 );
      }
      if( numberOfTRTHighThresholdHitsTotal.size() != phi.size() ) {
         numberOfTRTHighThresholdHitsTotal.resize( phi.size(), 0 );
      }

      return;
   }

} // namespace xAOD
