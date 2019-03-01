/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUON_VERSIONS_MUONAUXCONTAINER_V5_H
#define XAODMUON_VERSIONS_MUONAUXCONTAINER_V5_H
 
// System include(s):
#include <stdint.h>
#include <vector>

// Core include(s):
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"
 
// xAOD include(s):
#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODCaloEvent/CaloClusterContainer.h" 
#include "xAODMuon/MuonSegmentContainer.h" 
 
namespace xAOD {
 
   /// Temporary container used until we have I/O for AuxStoreInternal
   ///
   /// This class is meant to serve as a temporary way to provide an auxiliary
   /// store with Athena I/O capabilities for the Muon EDM. Will be exchanged for
   /// a generic auxiliary container type (AuxStoreInternal) later on.
   ///
   /// @author Edward Moyse
   ///
   class MuonAuxContainer_v5 : public AuxContainerBase {
 
   public:
      /// Default constructor
      MuonAuxContainer_v5();
 
   private:
      /// @name iParticle values
      /// @{                          
     std::vector< float >             pt;
     std::vector< float >             eta;
     std::vector< float >             phi;
     std::vector< float >             charge;
     /// @}  
			                       	
     /// @name Muon summary information
     /// @{                                      	
     std::vector< uint16_t >          allAuthors;
     std::vector< uint16_t>           author;
     std::vector< uint16_t>           muonType;
     std::vector< uint8_t>            quality; //quality, passesIDCuts and passesHighPtCuts are packed in here.

      /// @}  

      /// @name TrackSummary and MuonTrackSummary information
      /// @{
      std::vector< uint8_t >          numberOfPrecisionLayers;
      std::vector< uint8_t >          numberOfPrecisionHoleLayers;
      std::vector< uint8_t >          numberOfPhiLayers;
      std::vector< uint8_t >          numberOfPhiHoleLayers;
      std::vector< uint8_t >          numberOfTriggerEtaLayers;
      std::vector< uint8_t >          numberOfTriggerEtaHoleLayers;
      std::vector< uint8_t >          primarySector;
      std::vector< uint8_t >          secondarySector;
      
      std::vector< uint8_t >          innerSmallHits;
      std::vector< uint8_t >          innerLargeHits;
      std::vector< uint8_t >          middleSmallHits;
      std::vector< uint8_t >          middleLargeHits;
      std::vector< uint8_t >          outerSmallHits;
      std::vector< uint8_t >          outerLargeHits;
      std::vector< uint8_t >          extendedSmallHits;
      std::vector< uint8_t >          extendedLargeHits;
      
      std::vector< uint8_t >          innerSmallHoles;
      std::vector< uint8_t >          innerLargeHoles;
      std::vector< uint8_t >          middleSmallHoles;
      std::vector< uint8_t >          middleLargeHoles;
      std::vector< uint8_t >          outerSmallHoles;
      std::vector< uint8_t >          outerLargeHoles;
      std::vector< uint8_t >          extendedSmallHoles;
      std::vector< uint8_t >          extendedLargeHoles;
      
      std::vector< uint8_t >          phiLayer1Hits;
      std::vector< uint8_t >          phiLayer2Hits;
      std::vector< uint8_t >          phiLayer3Hits;
      std::vector< uint8_t >          phiLayer4Hits;
      
      std::vector< uint8_t >          etaLayer1Hits;
      std::vector< uint8_t >          etaLayer2Hits;
      std::vector< uint8_t >          etaLayer3Hits;
      std::vector< uint8_t >          etaLayer4Hits;
      
      std::vector< uint8_t >          phiLayer1Holes;
      std::vector< uint8_t >          phiLayer2Holes;
      std::vector< uint8_t >          phiLayer3Holes;
      std::vector< uint8_t >          phiLayer4Holes;
      
      std::vector< uint8_t >          etaLayer1Holes;
      std::vector< uint8_t >          etaLayer2Holes;
      std::vector< uint8_t >          etaLayer3Holes;
      std::vector< uint8_t >          etaLayer4Holes;

      std::vector< uint8_t >          phiLayer1RPCHits;
      std::vector< uint8_t >          phiLayer2RPCHits;
      std::vector< uint8_t >          phiLayer3RPCHits;
      std::vector< uint8_t >          phiLayer4RPCHits;
      
      std::vector< uint8_t >          etaLayer1RPCHits;
      std::vector< uint8_t >          etaLayer2RPCHits;
      std::vector< uint8_t >          etaLayer3RPCHits;
      std::vector< uint8_t >          etaLayer4RPCHits;
      
      std::vector< uint8_t >          phiLayer1RPCHoles;
      std::vector< uint8_t >          phiLayer2RPCHoles;
      std::vector< uint8_t >          phiLayer3RPCHoles;
      std::vector< uint8_t >          phiLayer4RPCHoles;
      
      std::vector< uint8_t >          etaLayer1RPCHoles;
      std::vector< uint8_t >          etaLayer2RPCHoles;
      std::vector< uint8_t >          etaLayer3RPCHoles;
      std::vector< uint8_t >          etaLayer4RPCHoles;
      
      std::vector< uint8_t >          phiLayer1TGCHits;
      std::vector< uint8_t >          phiLayer2TGCHits;
      std::vector< uint8_t >          phiLayer3TGCHits;
      std::vector< uint8_t >          phiLayer4TGCHits;
      
      std::vector< uint8_t >          etaLayer1TGCHits;
      std::vector< uint8_t >          etaLayer2TGCHits;
      std::vector< uint8_t >          etaLayer3TGCHits;
      std::vector< uint8_t >          etaLayer4TGCHits;
      
      std::vector< uint8_t >          phiLayer1TGCHoles;
      std::vector< uint8_t >          phiLayer2TGCHoles;
      std::vector< uint8_t >          phiLayer3TGCHoles;
      std::vector< uint8_t >          phiLayer4TGCHoles;
      
      std::vector< uint8_t >          etaLayer1TGCHoles;
      std::vector< uint8_t >          etaLayer2TGCHoles;
      std::vector< uint8_t >          etaLayer3TGCHoles;
      std::vector< uint8_t >          etaLayer4TGCHoles;

      // STGC
      std::vector< uint8_t >          phiLayer1STGCHits;
      std::vector< uint8_t >          phiLayer2STGCHits;

      std::vector< uint8_t >          etaLayer1STGCHits;
      std::vector< uint8_t >          etaLayer2STGCHits;

      std::vector< uint8_t >          phiLayer1STGCHoles;
      std::vector< uint8_t >          phiLayer2STGCHoles;

      std::vector< uint8_t >          etaLayer1STGCHoles;
      std::vector< uint8_t >          etaLayer2STGCHoles;

      // MM
      std::vector< uint8_t >          MMHits;
      std::vector< uint8_t >          MMHoles;
      // Note: there is currently no MuonStationIndex for the two MM layers
      // In the future it might be needed to add them in order to have
      // separate counters as follows
      // std::vector< uint8_t >          Layer1MMHits;
      // std::vector< uint8_t >          Layer2MMHits;
      // std::vector< uint8_t >          Layer1MMHoles;
      // std::vector< uint8_t >          Layer2MMHoles;

      std::vector< uint8_t >          cscEtaHits;
      std::vector< uint8_t >          cscUnspoiledEtaHits;

      /// @}   
      
      /// @name Links 
      /// @{    
      std::vector<ElementLink< TrackParticleContainer > >                 inDetTrackParticleLink;
      std::vector<ElementLink< TrackParticleContainer > >                 muonSpectrometerTrackParticleLink;
      std::vector<ElementLink< TrackParticleContainer > >                 extrapolatedMuonSpectrometerTrackParticleLink;
      std::vector<ElementLink< TrackParticleContainer > >                 msOnlyExtrapolatedMuonSpectrometerTrackParticleLink;
      std::vector<ElementLink< TrackParticleContainer > >                 combinedTrackParticleLink;
      std::vector<ElementLink< CaloClusterContainer   > >                 clusterLink;
      std::vector<std::vector<ElementLink< MuonSegmentContainer   > > >   muonSegmentLinks;
      /// @}
      
      /// @name Energy loss 
      /// @{
      std::vector< uint8_t>           energyLossType;
      /// @}
      
      /// @name Param defs 
      /// @{
      std::vector< float>             spectrometerFieldIntegral      ;
      std::vector< float>             scatteringCurvatureSignificance;
      std::vector< float>             scatteringNeighbourSignificance;
      std::vector< float>             momentumBalanceSignificance    ;
      std::vector< float>             segmentDeltaEta                ;
      std::vector< float>             segmentDeltaPhi                ;
      std::vector< float>             segmentChi2OverDoF             ;
      std::vector< float>             t0                             ;
      std::vector< float>             beta                           ;
      std::vector< float>             annBarrel                      ;
      std::vector< float>             annEndCap                      ;
      std::vector< float>             innAngle                       ;
      std::vector< float>             midAngle                       ;
      std::vector< float>             msInnerMatchChi2               ;
      std::vector< float>             meanDeltaADCCountsMDT          ;
      std::vector< float>             CaloLRLikelihood               ;
      std::vector< float>             EnergyLoss                     ;
      std::vector< float>             ParamEnergyLoss                ;
      std::vector< float>             MeasEnergyLoss                 ;
      std::vector< float>             EnergyLossSigma                ;
      std::vector< float>             ParamEnergyLossSigmaPlus       ;
      std::vector< float>             ParamEnergyLossSigmaMinus      ;
      std::vector< float>             MeasEnergyLossSigma            ;
      std::vector< int >              msInnerMatchDOF                ;
      std::vector< int >              msOuterMatchDOF                ;
      std::vector< int >              CaloMuonIDTag                  ;
      /// @}
 
   }; // class MuonAuxContainer_v5
 
} // namespace xAOD
 
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::MuonAuxContainer_v5, xAOD::AuxContainerBase ); 
 
#endif
