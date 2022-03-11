/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


// Local include(s):
#include "xAODTracking/versions/TrackParticleAuxContainer_v5.h"

namespace xAOD {

   TrackParticleAuxContainer_v5::TrackParticleAuxContainer_v5()
      : AuxContainerBase() {

      AUX_VARIABLE( d0 );
      AUX_VARIABLE( z0 );
      AUX_VARIABLE( phi );
      AUX_VARIABLE( theta );
      AUX_VARIABLE( qOverP );

      AUX_VARIABLE( definingParametersCovMatrixDiag );
      AUX_VARIABLE( definingParametersCovMatrixOffDiag );

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
      AUX_VARIABLE( pixeldEdx                 );

      //Links
      AUX_VARIABLE( TRTTrackOccupancy );

   }

   void TrackParticleAuxContainer_v5::dump() const {
     std::cout<<" Dumping TrackParticleAuxContainer_v5"<<std::endl;
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
     std::cout<<"definingParametersCovMatrixDiag: ";
     for (unsigned int i=0; i<definingParametersCovMatrixDiag.size();++i){
     std::copy(definingParametersCovMatrixDiag[i].begin(), definingParametersCovMatrixDiag[i].end(),
       std::ostream_iterator<float>(std::cout, ", "));
     }
     std::cout<<"definingParametersCovMatrixOffDiag: ";
     for (unsigned int i=0; i<definingParametersCovMatrixOffDiag.size();++i){
     std::copy(definingParametersCovMatrixOffDiag[i].begin(), definingParametersCovMatrixOffDiag[i].end(),
       std::ostream_iterator<float>(std::cout, ", "));
        std::cout<<std::endl;
     }     
   }


} // namespace xAOD
