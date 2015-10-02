// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticleAuxContainer_v2.h 576255 2013-12-19 12:54:41Z emoyse $
#ifndef XAODTRACKING_VERSIONS_TRACKPARTICLEAUXCONTAINER_V2_H
#define XAODTRACKING_VERSIONS_TRACKPARTICLEAUXCONTAINER_V2_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODTracking/VertexContainer.h"

// Want to be able to switch this off and on for the moment
//#define XAODTRACKING_SUMMARYDYNAMIC

namespace xAOD {

   /// Temporary container used until we have I/O for AuxStoreInternal
   ///
   /// This class is meant to serve as a temporary way to provide an auxiliary
   /// store with Athena I/O capabilities for the TrackParticle EDM. Will be exchanged for
   /// a generic auxiliary container type (AuxStoreInternal) later on.
   ///
   /// @author Edward Moyse
   ///
   class TrackParticleAuxContainer_v2 : public AuxContainerBase {

   public:
      /// Default constructor
      TrackParticleAuxContainer_v2();
      /// Dumps contents (for debugging)
      void dump() const;
      /// Function implementing soft schema evolution for the class
      void toTransient();

   private:

      /// @name Defining parameters (perigee)
      /// @{
      std::vector< float >                d0;
      std::vector< float >                z0;
      std::vector< float >                phi;
      std::vector< float >                theta;
      std::vector< float >                qOverP;

      std::vector< std::vector<float> >   definingParametersCovMatrix;

      std::vector< float >                vx;
      std::vector< float >                vy;
      std::vector< float >                vz;
      /// @}

      // /// @name Parameters
//       /// We store the 3-pos, 3-mom and charge, and on the transient side these will be transformed into curvilinear parameters.
//       /// Also stored are the cov matrix (still expressed in local coordinate frame) and parameter position.
//       /// @{
//       std::vector< std::vector<float> >   parameterX;
//       std::vector< std::vector<float> >   parameterY;
//       std::vector< std::vector<float> >   parameterZ;
//       std::vector< std::vector<float> >   parameterPX;
//       std::vector< std::vector<float> >   parameterPY;
//       std::vector< std::vector<float> >   parameterPZ;
//       // std::vector< char > charge; /// @todo Is this really necessary? Can get it from qOverP?
//
//       std::vector< std::vector<float> >   trackParameterCovarianceMatrices;
//       std::vector< std::vector<uint8_t> > parameterPosition;

      std::vector< float >                radiusOfFirstHit;
      std::vector< uint64_t >             identifierOfFirstHit;

     std::vector< float >  beamlineTiltX; 
     std::vector< float >  beamlineTiltY;

     std::vector< uint32_t > hitPattern;

      /// @name Fit quality functions
      /// @{
      std::vector< float >                chiSquared;
      std::vector< float >                numberDoF;
      /// @}

      /// @name TrackInfo functions
      /// @{
      std::vector< uint8_t >              trackFitter;
      std::vector< uint8_t >              particleHypothesis;
      std::vector< uint8_t >              trackProperties;
      std::vector< uint64_t >             patternRecoInfo;
      /// @}

#ifndef XAODTRACKING_SUMMARYDYNAMIC
      /// @name TrackSummary information
      /// @{
     std::vector< uint8_t >         numberOfContribPixelLayers       ;
     std::vector< uint8_t >         numberOfInnermostPixelLayerHits               ;
     std::vector< uint8_t >         numberOfInnermostPixelLayerOutliers           ;
     std::vector< uint8_t >         numberOfInnermostPixelLayerSharedHits         ;
     std::vector< uint8_t >         numberOfInnermostPixelLayerSplitHits          ;
     std::vector< uint8_t >         expectInnermostPixelLayerHit                  ; /// @todo FIXME!Should be bool.
     std::vector< uint8_t >         numberOfNextToInnermostPixelLayerHits               ;
     std::vector< uint8_t >         numberOfNextToInnermostPixelLayerOutliers           ;
     std::vector< uint8_t >         numberOfNextToInnermostPixelLayerSharedHits         ;
     std::vector< uint8_t >         numberOfNextToInnermostPixelLayerSplitHits          ;
     std::vector< uint8_t >         expectNextToInnermostPixelLayerHit                  ; /// @todo FIXME!Should be bool.
     std::vector< uint8_t >         numberOfPixelHits                ;
     std::vector< uint8_t >         numberOfPixelOutliers            ;
     std::vector< uint8_t >         numberOfPixelHoles               ;
     std::vector< uint8_t >         numberOfPixelSharedHits          ;
     std::vector< uint8_t >         numberOfPixelSplitHits           ;
     std::vector< uint8_t >         numberOfGangedPixels             ;
     std::vector< uint8_t >         numberOfGangedFlaggedFakes       ;
     std::vector< uint8_t >         numberOfPixelDeadSensors         ;
     std::vector< uint8_t >         numberOfPixelSpoiltHits          ;
     std::vector< uint8_t >         numberOfDBMHits                  ;
     std::vector< uint8_t >         numberOfSCTHits                  ;
     std::vector< uint8_t >         numberOfSCTOutliers              ;
     std::vector< uint8_t >         numberOfSCTHoles                 ;
     std::vector< uint8_t >         numberOfSCTDoubleHoles           ;
     std::vector< uint8_t >         numberOfSCTSharedHits            ;
     std::vector< uint8_t >         numberOfSCTDeadSensors           ;
     std::vector< uint8_t >         numberOfSCTSpoiltHits            ;
     std::vector< uint8_t >         numberOfTRTHits                  ;
     std::vector< uint8_t >         numberOfTRTOutliers              ;
     std::vector< uint8_t >         numberOfTRTHoles                 ;
     std::vector< uint8_t >         numberOfTRTHighThresholdHits     ;
     std::vector< uint8_t >         numberOfTRTHighThresholdHitsTotal;
     std::vector< uint8_t >         numberOfTRTHighThresholdOutliers ;
     std::vector< uint8_t >         numberOfTRTDeadStraws            ;
     std::vector< uint8_t >         numberOfTRTTubeHits              ;
     std::vector< uint8_t >         numberOfTRTXenonHits             ;
     std::vector< uint8_t >         numberOfTRTSharedHits             ;
     
     std::vector< uint8_t >         numberOfPrecisionLayers;
     std::vector< uint8_t >         numberOfPrecisionHoleLayers;
     std::vector< uint8_t >         numberOfPhiLayers;
     std::vector< uint8_t >         numberOfPhiHoleLayers;
     std::vector< uint8_t >         numberOfTriggerEtaLayers;
     std::vector< uint8_t >         numberOfTriggerEtaHoleLayers;
     
     std::vector< uint8_t >         numberOfOutliersOnTrack          ;
     std::vector< uint8_t >         standardDeviationOfChi2OS        ;
     std::vector< float >           eProbabilityComb;
     std::vector< float >           eProbabilityHT;
     //     std::vector< float >           eProbabilityToT;
     //     std::vector< float >           eProbabilityBrem;
     std::vector< float >           pixeldEdx;
     std::vector< uint8_t >         numberOfUsedHitsdEdx;
     std::vector< uint8_t >         numberOfIBLOverflowsdEdx;

     /// @}
#endif

      /// @name Links
      /// @{
      std::vector< ElementLink< VertexContainer > > vertexLink;
      /// @}

   }; // class TrackParticleAuxContainer_v2

} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrackParticleAuxContainer_v2, xAOD::AuxContainerBase );

#endif // XAODJET_VERSIONS_JETAUXCONTAINER_V1_H
