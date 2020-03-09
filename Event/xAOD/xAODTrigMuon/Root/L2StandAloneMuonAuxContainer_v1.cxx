/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODTrigMuon/versions/L2StandAloneMuonAuxContainer_v1.h"

namespace xAOD {

   L2StandAloneMuonAuxContainer_v1::L2StandAloneMuonAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( pt );
      AUX_VARIABLE( phi );
      AUX_VARIABLE( eta );

      AUX_VARIABLE( roiWord );
      AUX_VARIABLE( sAddress );
      AUX_VARIABLE( rMS );
      AUX_VARIABLE( etaMS );
      AUX_VARIABLE( phiMS );
      AUX_VARIABLE( dirPhiMS );
      AUX_VARIABLE( zMS );
      AUX_VARIABLE( dirZMS );
      AUX_VARIABLE( beta );

      AUX_VARIABLE( barrelRadius );
      AUX_VARIABLE( barrelSagitta );

      AUX_VARIABLE( endcapAlpha );
      AUX_VARIABLE( endcapBeta );
      AUX_VARIABLE( endcapRadius );

      AUX_VARIABLE( slopeInner );
      AUX_VARIABLE( interceptInner );
      AUX_VARIABLE( deltaR );

      AUX_VARIABLE( etaMap );
      AUX_VARIABLE( phiMap );
      AUX_VARIABLE( etaBin );
      AUX_VARIABLE( phiBin );

      AUX_VARIABLE( isTgcFailure );
      AUX_VARIABLE( isRpcFailure );

      AUX_VARIABLE( deltaPt );
      AUX_VARIABLE( deltaEta );
      AUX_VARIABLE( deltaPhi );

      AUX_VARIABLE( superPointR );
      AUX_VARIABLE( superPointZ );
      AUX_VARIABLE( superPointSlope );
      AUX_VARIABLE( superPointIntercept );
      AUX_VARIABLE( superPointChi2 );

      AUX_VARIABLE( trackPositionR );
      AUX_VARIABLE( trackPositionZ );
      AUX_VARIABLE( trackPositionEta );
      AUX_VARIABLE( trackPositionPhi );

      AUX_VARIABLE( algoId );
      AUX_VARIABLE( teId );
      AUX_VARIABLE( lvl1Id );
      AUX_VARIABLE( lumiBlock );
      AUX_VARIABLE( muonDetMask );
      AUX_VARIABLE( roiId );
      AUX_VARIABLE( roiSystem );
      AUX_VARIABLE( roiSubsystem );
      AUX_VARIABLE( roiSector );
      AUX_VARIABLE( roiNumber );
      AUX_VARIABLE( roiThreshold );
      //AUX_VARIABLE( roiEta );
      //AUX_VARIABLE( roiPhi );
      static const auxid_t etaId = getAuxID( "roiEtaUint", roiEta );
      regAuxVar( etaId, "roiEtaUint", roiEta );
      static const auxid_t phiId = getAuxID( "roiPhiUint", roiPhi );
      regAuxVar( phiId, "roiPhiUint", roiPhi );

      AUX_VARIABLE( ptBarrelRadius );
      AUX_VARIABLE( ptBarrelSagitta );
      AUX_VARIABLE( ptEndcapAlpha );
      AUX_VARIABLE( ptEndcapBeta );
      AUX_VARIABLE( ptEndcapRadius );

      AUX_VARIABLE( chamberType1Normal );
      AUX_VARIABLE( chamberType1Overlap );
      AUX_VARIABLE( chamberType2Normal );
      AUX_VARIABLE( chamberType2Overlap );

      AUX_VARIABLE( roadAwNormal );
      AUX_VARIABLE( roadAwOverlap );

      AUX_VARIABLE( roadBwNormal );
      AUX_VARIABLE( roadBwOverlap );

      AUX_VARIABLE( zMinNormal );
      AUX_VARIABLE( zMinOverlap );
      AUX_VARIABLE( zMaxNormal );
      AUX_VARIABLE( zMaxOverlap );

      AUX_VARIABLE( rMinNormal );
      AUX_VARIABLE( rMinOverlap );
      AUX_VARIABLE( rMaxNormal );
      AUX_VARIABLE( rMaxOverlap );

      AUX_VARIABLE( etaMinNormal );
      AUX_VARIABLE( etaMinOverlap );
      AUX_VARIABLE( etaMaxNormal );
      AUX_VARIABLE( etaMaxOverlap );

      AUX_VARIABLE( tgcPt );

      AUX_VARIABLE( tgcInnEta );
      AUX_VARIABLE( tgcInnPhi );
      AUX_VARIABLE( tgcInnR );
      AUX_VARIABLE( tgcInnZ );

      AUX_VARIABLE( tgcInnRhoStd );
      AUX_VARIABLE( tgcInnRhoN );
      AUX_VARIABLE( tgcInnPhiStd );
      AUX_VARIABLE( tgcInnPhiN );

      AUX_VARIABLE( tgcMid1Eta );
      AUX_VARIABLE( tgcMid1Phi );
      AUX_VARIABLE( tgcMid1R );
      AUX_VARIABLE( tgcMid1Z );

      AUX_VARIABLE( tgcMid2Eta );
      AUX_VARIABLE( tgcMid2Phi );
      AUX_VARIABLE( tgcMid2R );
      AUX_VARIABLE( tgcMid2Z );

      AUX_VARIABLE( tgcMidRhoChi2 );
      AUX_VARIABLE( tgcMidRhoN );
      AUX_VARIABLE( tgcMidPhiChi2 );
      AUX_VARIABLE( tgcMidPhiN );

      AUX_VARIABLE( rpc1x );
      AUX_VARIABLE( rpc1y );
      AUX_VARIABLE( rpc1z );

      AUX_VARIABLE( rpc2x );
      AUX_VARIABLE( rpc2y );
      AUX_VARIABLE( rpc2z );

      AUX_VARIABLE( rpc3x );
      AUX_VARIABLE( rpc3y );
      AUX_VARIABLE( rpc3z );

      AUX_VARIABLE( extensionCapacity );
      AUX_VARIABLE( robCapacity );
      AUX_VARIABLE( csmCapacity );
      AUX_VARIABLE( lvl1EmuCapacity );
      AUX_VARIABLE( rpcHitsCapacity );
      AUX_VARIABLE( tgcHitsCapacity );
      AUX_VARIABLE( mdtHitsCapacity );

      AUX_VARIABLE( extension0 );
      AUX_VARIABLE( extension1 );
      AUX_VARIABLE( extension2 );
      AUX_VARIABLE( extension3 );
      AUX_VARIABLE( extension4 );
      AUX_VARIABLE( extension5 );
      AUX_VARIABLE( extension6 );
      AUX_VARIABLE( extension7 );
      AUX_VARIABLE( extension8 );
      AUX_VARIABLE( extension9 );

      AUX_VARIABLE( lvl1Emulation );    

      AUX_VARIABLE( robId );
      AUX_VARIABLE( removedRobId );

      AUX_VARIABLE( csmId );
      AUX_VARIABLE( csmSize );
      AUX_VARIABLE( csmError );
      AUX_VARIABLE( removedCsmId );

      AUX_VARIABLE( padHitOnlineId );
      AUX_VARIABLE( padHitCode );
      AUX_VARIABLE( padHitX );
      AUX_VARIABLE( padHitY );
      AUX_VARIABLE( padHitZ );
      AUX_VARIABLE( padHitR );
      AUX_VARIABLE( padHitP );

      AUX_VARIABLE( tgcInnRhoHitPhi );
      AUX_VARIABLE( tgcInnRhoHitR );
      AUX_VARIABLE( tgcInnRhoHitZ );
      AUX_VARIABLE( tgcInnRhoHitWidth );
      AUX_VARIABLE( tgcInnRhoHitInSeg );
      AUX_VARIABLE( tgcInnPhiHitPhi );
      AUX_VARIABLE( tgcInnPhiHitR );
      AUX_VARIABLE( tgcInnPhiHitZ );
      AUX_VARIABLE( tgcInnPhiHitWidth );
      AUX_VARIABLE( tgcInnPhiHitInSeg );
      AUX_VARIABLE( tgcMidRhoHitPhi );
      AUX_VARIABLE( tgcMidRhoHitR );
      AUX_VARIABLE( tgcMidRhoHitZ );
      AUX_VARIABLE( tgcMidRhoHitWidth );
      AUX_VARIABLE( tgcMidRhoHitInSeg );
      AUX_VARIABLE( tgcMidPhiHitPhi );
      AUX_VARIABLE( tgcMidPhiHitR );
      AUX_VARIABLE( tgcMidPhiHitZ );
      AUX_VARIABLE( tgcMidPhiHitWidth );
      AUX_VARIABLE( tgcMidPhiHitInSeg );

      AUX_VARIABLE( mdtHitOnlineId );
      AUX_VARIABLE( mdtHitOfflineId );
      //AUX_VARIABLE( mdtHitChamber );
      AUX_VARIABLE( mdtHitR );
      AUX_VARIABLE( mdtHitZ );
      AUX_VARIABLE( mdtHitResidual );
      AUX_VARIABLE( mdtHitTime );
      AUX_VARIABLE( mdtHitSpace );
      AUX_VARIABLE( mdtHitSigma );
   }

} // namespace xAOD
