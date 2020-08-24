/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2StandAloneMuonAuxContainer_v2.cxx 661304 2015-04-18 04:31:23Z mishitsu $

// Local include(s):
#include "xAODTrigMuon/versions/L2StandAloneMuonAuxContainer_v2.h"

namespace xAOD {

   L2StandAloneMuonAuxContainer_v2::L2StandAloneMuonAuxContainer_v2()
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

      AUX_VARIABLE( etaMap );
      AUX_VARIABLE( phiMap );
      AUX_VARIABLE( etaBin );
      AUX_VARIABLE( phiBin );

      AUX_VARIABLE( isTgcFailure );
      AUX_VARIABLE( isRpcFailure );

      AUX_VARIABLE( deltaPt );
      AUX_VARIABLE( deltaPtParm1 );
      AUX_VARIABLE( deltaPtParm2 );
      AUX_VARIABLE( deltaPtParm3 );
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
      AUX_VARIABLE( roiEta );
      AUX_VARIABLE( roiPhi );

      AUX_VARIABLE( tgcPt );
      AUX_VARIABLE( ptBarrelRadius );
      AUX_VARIABLE( ptBarrelSagitta );
      AUX_VARIABLE( ptEndcapAlpha );
      AUX_VARIABLE( ptEndcapBeta );
      AUX_VARIABLE( ptEndcapRadius );
      AUX_VARIABLE( ptCSC );

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

      AUX_VARIABLE( rpcFitInnPhi );
      AUX_VARIABLE( rpcFitInnSlope );
      AUX_VARIABLE( rpcFitInnOffset );

      AUX_VARIABLE( rpcFitMidPhi );
      AUX_VARIABLE( rpcFitMidSlope );
      AUX_VARIABLE( rpcFitMidOffset );

      AUX_VARIABLE( rpcFitOutPhi );
      AUX_VARIABLE( rpcFitOutSlope );
      AUX_VARIABLE( rpcFitOutOffset );

      AUX_VARIABLE( rpcHitsCapacity );
      AUX_VARIABLE( tgcHitsCapacity );
      AUX_VARIABLE( mdtHitsCapacity );
      AUX_VARIABLE( cscHitsCapacity );

      AUX_VARIABLE( rpcHitLayer );
      AUX_VARIABLE( rpcHitMeasuresPhi );
      AUX_VARIABLE( rpcHitX );
      AUX_VARIABLE( rpcHitY );
      AUX_VARIABLE( rpcHitZ );
      AUX_VARIABLE( rpcHitTime );
      AUX_VARIABLE( rpcHitDistToEtaReadout );
      AUX_VARIABLE( rpcHitDistToPhiReadout );
      AUX_VARIABLE( rpcHitStationName );

      AUX_VARIABLE( tgcHitEta );
      AUX_VARIABLE( tgcHitPhi );
      AUX_VARIABLE( tgcHitR );
      AUX_VARIABLE( tgcHitZ );
      AUX_VARIABLE( tgcHitWidth );
      AUX_VARIABLE( tgcHitStationNum );
      AUX_VARIABLE( tgcHitIsStrip );
      AUX_VARIABLE( tgcHitBCTag );
      AUX_VARIABLE( tgcHitInRoad );

      AUX_VARIABLE( mdtHitOnlineId );
      AUX_VARIABLE( mdtHitIsOutlier );
      AUX_VARIABLE( mdtHitChamberId );
      AUX_VARIABLE( mdtHitR );
      AUX_VARIABLE( mdtHitZ );
      AUX_VARIABLE( mdtHitPhi );
      AUX_VARIABLE( mdtHitResidual );
      AUX_VARIABLE( mdtHitTime );
      AUX_VARIABLE( mdtHitSpace );
      AUX_VARIABLE( mdtHitSigma );

      AUX_VARIABLE( cscHitIsOutlier );
      AUX_VARIABLE( cscHitChamberId );
      AUX_VARIABLE( cscHitStationName );
      AUX_VARIABLE( cscHitStationEta );
      AUX_VARIABLE( cscHitStationPhi );
      AUX_VARIABLE( cscHitChamberLayer );
      AUX_VARIABLE( cscHitWireLayer );
      AUX_VARIABLE( cscHitMeasuresPhi );
      AUX_VARIABLE( cscHitStrip );
      AUX_VARIABLE( cscHitEta );
      AUX_VARIABLE( cscHitPhi );
      AUX_VARIABLE( cscHitR );
      AUX_VARIABLE( cscHitZ );
      AUX_VARIABLE( cscHitCharge );
      AUX_VARIABLE( cscHitTime );
      AUX_VARIABLE( cscHitResidual );

      AUX_VARIABLE( stgcClusterLayer );
      AUX_VARIABLE( stgcClusterIsOutlier );
      AUX_VARIABLE( stgcClusterType );
      AUX_VARIABLE( stgcClusterEta );
      AUX_VARIABLE( stgcClusterPhi );
      AUX_VARIABLE( stgcClusterR );
      AUX_VARIABLE( stgcClusterZ );
      AUX_VARIABLE( stgcClusterResidualR );
      AUX_VARIABLE( stgcClusterResidualPhi );
      AUX_VARIABLE( stgcClusterStationEta );
      AUX_VARIABLE( stgcClusterStationPhi );
      AUX_VARIABLE( stgcClusterStationName );

      AUX_VARIABLE( mmClusterLayer );
      AUX_VARIABLE( mmClusterIsOutlier );
      AUX_VARIABLE( mmClusterEta );
      AUX_VARIABLE( mmClusterPhi );
      AUX_VARIABLE( mmClusterR );
      AUX_VARIABLE( mmClusterZ );
      AUX_VARIABLE( mmClusterResidualR );
      AUX_VARIABLE( mmClusterResidualPhi );
      AUX_VARIABLE( mmClusterStationEta );
      AUX_VARIABLE( mmClusterStationPhi );
      AUX_VARIABLE( mmClusterStationName );

   }

} // namespace xAOD
