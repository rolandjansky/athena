/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJetAuxContainer_v3.cxx 725228 2016-02-19 22:59:42Z griffith $


// Local include(s):
#include "xAODTau/versions/TauJetAuxContainer_v3.h"

namespace xAOD {
  
  TauJetAuxContainer_v3::TauJetAuxContainer_v3()
    : AuxContainerBase() {
    
    AUX_VARIABLE( pt );
    AUX_VARIABLE( eta );
    AUX_VARIABLE( phi );
    AUX_VARIABLE( m );

    AUX_VARIABLE( ptJetSeed );
    AUX_VARIABLE( etaJetSeed );
    AUX_VARIABLE( phiJetSeed );
    AUX_VARIABLE( mJetSeed );
    
    AUX_VARIABLE( ptDetectorAxis );
    AUX_VARIABLE( etaDetectorAxis );
    AUX_VARIABLE( phiDetectorAxis );
    AUX_VARIABLE( mDetectorAxis );
    
    AUX_VARIABLE( ptIntermediateAxis );
    AUX_VARIABLE( etaIntermediateAxis );
    AUX_VARIABLE( phiIntermediateAxis );
    AUX_VARIABLE( mIntermediateAxis );
    
    AUX_VARIABLE( ptTauEnergyScale );
    AUX_VARIABLE( etaTauEnergyScale );
    AUX_VARIABLE( phiTauEnergyScale );
    AUX_VARIABLE( mTauEnergyScale );
    
    AUX_VARIABLE( ptTauEtaCalib );
    AUX_VARIABLE( etaTauEtaCalib );
    AUX_VARIABLE( phiTauEtaCalib );
    AUX_VARIABLE( mTauEtaCalib );

    AUX_VARIABLE( ptPanTauCellBasedProto );
    AUX_VARIABLE( etaPanTauCellBasedProto );
    AUX_VARIABLE( phiPanTauCellBasedProto );
    AUX_VARIABLE( mPanTauCellBasedProto );
   
    AUX_VARIABLE( ptPanTauCellBased );
    AUX_VARIABLE( etaPanTauCellBased );
    AUX_VARIABLE( phiPanTauCellBased );
    AUX_VARIABLE( mPanTauCellBased );

    AUX_VARIABLE( ptTrigCaloOnly );
    AUX_VARIABLE( etaTrigCaloOnly );
    AUX_VARIABLE( phiTrigCaloOnly );
    AUX_VARIABLE( mTrigCaloOnly );
   
    AUX_VARIABLE( ptFinalCalib );
    AUX_VARIABLE( etaFinalCalib );
    AUX_VARIABLE( phiFinalCalib );
    AUX_VARIABLE( mFinalCalib );

    
    AUX_VARIABLE( ROIWord );

    AUX_VARIABLE( charge );


    // AUX_VARIABLE( Likelihood );
    // AUX_VARIABLE( SafeLikelihood );
    AUX_VARIABLE( BDTJetScore );
    AUX_VARIABLE( BDTEleScore );
    AUX_VARIABLE( EleMatchLikelihoodScore );

    //additional discriminant outputs
    AUX_VARIABLE( BDTJetScoreSigTrans );
    // AUX_VARIABLE( BDTJetScoreBkgTrans );



    //    AUX_VARIABLE( vetoFlags );//r21 cleanup
    AUX_VARIABLE( isTauFlags );

    AUX_VARIABLE( tauTrackLinks );
    AUX_VARIABLE( nChargedTracks );
    AUX_VARIABLE( nIsolatedTracks );
    AUX_VARIABLE( clusterLinks );
    AUX_VARIABLE( pi0Links );

    AUX_VARIABLE( trackFilterProngs );
    AUX_VARIABLE( trackFilterQuality );
    AUX_VARIABLE( pi0ConeDR );

    AUX_VARIABLE( jetLink );

    AUX_VARIABLE( vertexLink );
    AUX_VARIABLE( secondaryVertexLink );

    AUX_VARIABLE( chargedPFOLinks );
    AUX_VARIABLE( neutralPFOLinks );
    AUX_VARIABLE( pi0PFOLinks );
    AUX_VARIABLE( protoChargedPFOLinks );
    AUX_VARIABLE( protoNeutralPFOLinks );
    AUX_VARIABLE( protoPi0PFOLinks );
    AUX_VARIABLE( shotPFOLinks ); 
    AUX_VARIABLE( hadronicPFOLinks ); 



    //set of details variables
    AUX_VARIABLE( ipZ0SinThetaSigLeadTrk );
    AUX_VARIABLE( etOverPtLeadTrk );
    AUX_VARIABLE( leadTrkPt );
    AUX_VARIABLE( ipSigLeadTrk );
    AUX_VARIABLE( massTrkSys );
    AUX_VARIABLE( trkWidth2 );
    AUX_VARIABLE( trFlightPathSig );
    // AUX_VARIABLE( ele_E237E277 ); //r21 cleanup
    // AUX_VARIABLE( ele_PresamplerFraction ); //r21 cleanup
    // AUX_VARIABLE( ele_ECALFirstFraction ); //r21 cleanup
    AUX_VARIABLE( numCells );
    AUX_VARIABLE( numTopoClusters );
    AUX_VARIABLE( numEffTopoClusters );
    AUX_VARIABLE( topoInvMass );
    AUX_VARIABLE( effTopoInvMass );
    AUX_VARIABLE( topoMeanDeltaR );
    AUX_VARIABLE( effTopoMeanDeltaR );
    AUX_VARIABLE( EMRadius );
    AUX_VARIABLE( hadRadius );
    AUX_VARIABLE( etEMAtEMScale );
    AUX_VARIABLE( etHadAtEMScale );
    AUX_VARIABLE( isolFrac );
    AUX_VARIABLE( centFrac );
    AUX_VARIABLE( stripWidth2 );
    AUX_VARIABLE( nStrip );
    
    //need to discuss whether these can be dropped
    //    AUX_VARIABLE( etEMCalib );//r21 cleanup
    //    AUX_VARIABLE( etHadCalib );//r21 cleanup
    // AUX_VARIABLE( seedCalo_eta );//r21 cleanup same as DetectorAxis eta
    // AUX_VARIABLE( seedCalo_phi );//r21 cleanup same as DetectorAxis phi
    

    AUX_VARIABLE( trkAvgDist );
    AUX_VARIABLE( trkRmsDist );
    AUX_VARIABLE( lead2ClusterEOverAllClusterE );
    AUX_VARIABLE( lead3ClusterEOverAllClusterE );
    AUX_VARIABLE( caloIso );
    AUX_VARIABLE( caloIsoCorrected );
    AUX_VARIABLE( dRmax );

    AUX_VARIABLE( secMaxStripEt );
    AUX_VARIABLE( sumEMCellEtOverLeadTrkPt );
    AUX_VARIABLE( hadLeakEt );

    //AUX_VARIABLE( EM_TES_scale );//r21 cleanup
    //AUX_VARIABLE( LC_TES_precalib );//r21 cleanup

    AUX_VARIABLE( TESOffset );
    AUX_VARIABLE( TESCalibConstant );

    AUX_VARIABLE( cellBasedEnergyRing1 );
    AUX_VARIABLE( cellBasedEnergyRing2 );
    AUX_VARIABLE( cellBasedEnergyRing3 );
    AUX_VARIABLE( cellBasedEnergyRing4 );
    AUX_VARIABLE( cellBasedEnergyRing5 );
    AUX_VARIABLE( cellBasedEnergyRing6 );
    AUX_VARIABLE( cellBasedEnergyRing7 );
    AUX_VARIABLE( TRT_NHT_OVER_NLT );
    AUX_VARIABLE( TauJetVtxFraction );
    
    //generic substructure details
    //    AUX_VARIABLE( nPi0 );//r21 cleanup
    AUX_VARIABLE( nCharged ); //used in PanTau/PanTauAlgs/src/Tool_DetailsArranger.cxx
    //    AUX_VARIABLE( etEflow );//r21 cleanup
    //    AUX_VARIABLE( mEflow );//r21 cleanup
    //    AUX_VARIABLE( ptRatioEflow );//r21 cleanup

    // AUX_VARIABLE( nPi0Topo );//r21 cleanup
    //    AUX_VARIABLE( nChargedTopo );//r21 cleanup
    //    AUX_VARIABLE( etEflowTopo );//r21 cleanup
    //    AUX_VARIABLE( mEflowTopo );//r21 cleanup
    //    AUX_VARIABLE( ptRatioEflowTopo );//r21 cleanup

    AUX_VARIABLE( mEflowApprox );
    AUX_VARIABLE( ptRatioEflowApprox );

    AUX_VARIABLE( innerTrkAvgDist );

    AUX_VARIABLE( SumPtTrkFrac );


    AUX_VARIABLE( etOverPtLeadTrkCorrected );
    AUX_VARIABLE( ipSigLeadTrkCorrected );
    AUX_VARIABLE( trFlightPathSigCorrected );
    AUX_VARIABLE( massTrkSysCorrected );
    AUX_VARIABLE( dRmaxCorrected );
    AUX_VARIABLE( ChPiEMEOverCaloEMECorrected );
    AUX_VARIABLE( EMPOverTrkSysPCorrected );
    AUX_VARIABLE( ptRatioEflowApproxCorrected );
    AUX_VARIABLE( mEflowApproxCorrected );
    AUX_VARIABLE( centFracCorrected );
    AUX_VARIABLE( innerTrkAvgDistCorrected );
    AUX_VARIABLE( SumPtTrkFracCorrected );



    //trigger details
    AUX_VARIABLE( PSSFraction );
    AUX_VARIABLE( ChPiEMEOverCaloEME );
    AUX_VARIABLE( EMPOverTrkSysP );



    //panTau details
    //!PanTau variables when using CellBased pfos
    //Flag whether this seed has pantau info
    AUX_VARIABLE( PanTau_isPanTauCandidate );
    //decay modes (input mode [proto] and output mode )
    AUX_VARIABLE( PanTau_DecayModeProto );
    AUX_VARIABLE( PanTau_DecayMode );
    //BDT output distributions
    AUX_VARIABLE( PanTau_BDTValue_1p0n_vs_1p1n );
    AUX_VARIABLE( PanTau_BDTValue_1p1n_vs_1pXn );
    AUX_VARIABLE( PanTau_BDTValue_3p0n_vs_3pXn );

    //Variables used in BDTs
    AUX_VARIABLE( PanTau_BDTVar_Basic_NNeutralConsts );
    AUX_VARIABLE( PanTau_BDTVar_Charged_JetMoment_EtDRxTotalEt );
    AUX_VARIABLE( PanTau_BDTVar_Charged_StdDev_Et_WrtEtAllConsts );
    AUX_VARIABLE( PanTau_BDTVar_Neutral_HLV_SumM );
    AUX_VARIABLE( PanTau_BDTVar_Neutral_PID_BDTValues_BDTSort_1 );
    AUX_VARIABLE( PanTau_BDTVar_Neutral_PID_BDTValues_BDTSort_2 );
    AUX_VARIABLE( PanTau_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts );
    AUX_VARIABLE( PanTau_BDTVar_Neutral_Ratio_EtOverEtAllConsts );
    AUX_VARIABLE( PanTau_BDTVar_Neutral_Shots_NPhotonsInSeed );
    AUX_VARIABLE( PanTau_BDTVar_Combined_DeltaR1stNeutralTo1stCharged );
    AUX_VARIABLE( PanTau_BDTVar_Charged_HLV_SumM );
    
    ////!PanTau variables when using eflowRec pfos
    ////Flag whether this seed has pantau info
    //AUX_VARIABLE( pantau_eflowRecInput_isPanTauCandidate );
    ////decay modes (input mode [proto] and output mode )
    //AUX_VARIABLE( pantau_eflowRecInput_DecayModeProto );
    //AUX_VARIABLE( pantau_eflowRecInput_DecayMode );
    ////BDT output distributions
    //AUX_VARIABLE( pantau_eflowRecInput_BDTValue_1p0n_vs_1p1n );
    //AUX_VARIABLE( pantau_eflowRecInput_BDTValue_1p1n_vs_1pXn );
    //AUX_VARIABLE( pantau_eflowRecInput_BDTValue_3p0n_vs_3pXn );
    ////Variables used in BDTs
    ////NOTE: They are different from the CellBased ones in general!
    //AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Basic_NPi0NeutConsts );
    //AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Basic_NNeutralConsts );
    //AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Charged_HLV_SumPt );
    //AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Charged_Ratio_EtOverEtAllConsts );
    //AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Neutral_HLV_SumM );
    //AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_EtSort_1 );
    //AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2 );
    //AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts );
    //AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Neutral_Mean_DRToLeading_WrtEtAllConsts );
    //AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged );

    // RNN tau ID
    AUX_VARIABLE( RNNJetScore );
    AUX_VARIABLE( RNNJetScoreSigTrans ); 

    //RNN tau eVeto
    AUX_VARIABLE( RNNEleScore );
    AUX_VARIABLE( RNNEleScoreSigTrans ); 
    
  }
  
} // namespace xAOD
