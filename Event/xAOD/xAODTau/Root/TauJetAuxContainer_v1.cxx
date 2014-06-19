/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJetAuxContainer_v1.cxx 602664 2014-06-19 14:20:50Z janus $


// Local include(s):
#include "xAODTau/versions/TauJetAuxContainer_v1.h"

namespace xAOD {
  
  TauJetAuxContainer_v1::TauJetAuxContainer_v1()
    : AuxContainerBase() {
    
    AUX_VARIABLE( pt );
    AUX_VARIABLE( eta );
    AUX_VARIABLE( phi );
    //AUX_VARIABLE( e );
    AUX_VARIABLE( m );

    AUX_VARIABLE( ptJetSeed );
    AUX_VARIABLE( etaJetSeed );
    AUX_VARIABLE( phiJetSeed );
    //AUX_VARIABLE( eJetSeed );
    AUX_VARIABLE( mJetSeed );
    
    AUX_VARIABLE( ptDetectorAxis );
    AUX_VARIABLE( etaDetectorAxis );
    AUX_VARIABLE( phiDetectorAxis );
    //AUX_VARIABLE( eDetectorAxis );
    AUX_VARIABLE( mDetectorAxis );
    
    AUX_VARIABLE( ptIntermediateAxis );
    AUX_VARIABLE( etaIntermediateAxis );
    AUX_VARIABLE( phiIntermediateAxis );
    //AUX_VARIABLE( eIntermediateAxis );
    AUX_VARIABLE( mIntermediateAxis );
    
    AUX_VARIABLE( ptTauEnergyScale );
    AUX_VARIABLE( etaTauEnergyScale );
    AUX_VARIABLE( phiTauEnergyScale );
    //AUX_VARIABLE( eTauEnergyScale );
    AUX_VARIABLE( mTauEnergyScale );
    
    AUX_VARIABLE( ptTauEtaCalib );
    AUX_VARIABLE( etaTauEtaCalib );
    AUX_VARIABLE( phiTauEtaCalib );
    //AUX_VARIABLE( eTauEtaCalib );
    AUX_VARIABLE( mTauEtaCalib );


    AUX_VARIABLE( ptPanTauEFlowRecProto );
    AUX_VARIABLE( etaPanTauEFlowRecProto );
    AUX_VARIABLE( phiPanTauEFlowRecProto );
    //AUX_VARIABLE( ePanTauEFlowRecProto );
    AUX_VARIABLE( mPanTauEFlowRecProto );
   
    AUX_VARIABLE( ptPanTauEFlowRec );
    AUX_VARIABLE( etaPanTauEFlowRec );
    AUX_VARIABLE( phiPanTauEFlowRec );
    //AUX_VARIABLE( ePanTauEFlowRec );
    AUX_VARIABLE( mPanTauEFlowRec );
   
    AUX_VARIABLE( ptPanTauCellBasedProto );
    AUX_VARIABLE( etaPanTauCellBasedProto );
    AUX_VARIABLE( phiPanTauCellBasedProto );
    //AUX_VARIABLE( ePanTauCellBasedProto );
    AUX_VARIABLE( mPanTauCellBasedProto );
   
    AUX_VARIABLE( ptPanTauCellBased );
    AUX_VARIABLE( etaPanTauCellBased );
    AUX_VARIABLE( phiPanTauCellBased );
    //AUX_VARIABLE( ePanTauCellBased );
    AUX_VARIABLE( mPanTauCellBased );
   

    
    AUX_VARIABLE( ROIWord );

    AUX_VARIABLE( charge );


    AUX_VARIABLE( Likelihood );
    AUX_VARIABLE( SafeLikelihood );
    AUX_VARIABLE( BDTJetScore );
    AUX_VARIABLE( BDTEleScore );
    AUX_VARIABLE( PanTauScore );

    //additional discriminant outputs
    AUX_VARIABLE( BDTJetScoreSigTrans );
    AUX_VARIABLE( BDTJetScoreBkgTrans );



    AUX_VARIABLE( vetoFlags );
    AUX_VARIABLE( isTauFlags );

    AUX_VARIABLE( trackLinks );
    AUX_VARIABLE( conversionTrackLinks );
    AUX_VARIABLE( wideTrackLinks );
    AUX_VARIABLE( otherTrackLinks );

    AUX_VARIABLE( trackFilterPass );
    AUX_VARIABLE( trackFilterProngs );
    AUX_VARIABLE( trackFilterQuality );
    AUX_VARIABLE( pi0ConeDR );

    AUX_VARIABLE( trackEtaStrip );
    AUX_VARIABLE( trackPhiStrip );


    AUX_VARIABLE( jetLink );

    AUX_VARIABLE( vertexLink );
    AUX_VARIABLE( secondaryVertexLink );

    AUX_VARIABLE( charged_PFOLinks );
    AUX_VARIABLE( neutral_PFOLinks );
    AUX_VARIABLE( pi0_PFOLinks );
    AUX_VARIABLE( cellBased_Charged_PFOLinks );
    AUX_VARIABLE( cellBased_Neutral_PFOLinks );
    AUX_VARIABLE( cellBased_Pi0_PFOLinks );
    AUX_VARIABLE( eflowRec_Charged_PFOLinks );
    AUX_VARIABLE( eflowRec_Neutral_PFOLinks );
    AUX_VARIABLE( eflowRec_Pi0_PFOLinks );
    AUX_VARIABLE( shot_PFOLinks ); 



    //add the full set without seedTrk_ vars and seedCalo_ name still in place
    AUX_VARIABLE( ipZ0SinThetaSigLeadTrk );
    AUX_VARIABLE( etOverPtLeadTrk );
    AUX_VARIABLE( leadTrkPt );
    AUX_VARIABLE( ipSigLeadTrk );
    // AUX_VARIABLE( ipSigLeadLooseTrk );
    // AUX_VARIABLE( etOverPtLeadLooseTrk );
    // AUX_VARIABLE( leadLooseTrkPt );
    // AUX_VARIABLE( chrgLooseTrk );
    AUX_VARIABLE( massTrkSys );
    AUX_VARIABLE( trkWidth2 );
    AUX_VARIABLE( trFlightPathSig );
    AUX_VARIABLE( ele_E237E277 );
    AUX_VARIABLE( ele_PresamplerFraction );
    AUX_VARIABLE( ele_ECALFirstFraction );
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
    AUX_VARIABLE( etEMCalib );
    AUX_VARIABLE( etHadCalib );
    AUX_VARIABLE( seedCalo_eta );
    AUX_VARIABLE( seedCalo_phi );
    // AUX_VARIABLE( nIsolLooseTrk );
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

    AUX_VARIABLE( EM_TES_scale );
    AUX_VARIABLE( LC_TES_precalib );

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
    AUX_VARIABLE( nPi0 );
    AUX_VARIABLE( nCharged );
    AUX_VARIABLE( etEflow );
    AUX_VARIABLE( mEflow );
    AUX_VARIABLE( ptRatioEflow );

    AUX_VARIABLE( nPi0Topo );
    AUX_VARIABLE( nChargedTopo );
    AUX_VARIABLE( etEflowTopo );
    AUX_VARIABLE( mEflowTopo );
    AUX_VARIABLE( ptRatioEflowTopo );

    //trigger details
    AUX_VARIABLE( PSSFraction );
    AUX_VARIABLE( ChPiEMEOverCaloEME );
    AUX_VARIABLE( EMPOverTrkSysP );



    //panTau details
    //!PanTau variables when using CellBased pfos
    //Flag whether this seed has pantau info
    AUX_VARIABLE( pantau_CellBasedInput_isPanTauCandidate );
    //decay modes (input mode [proto] and output mode )
    AUX_VARIABLE( pantau_CellBasedInput_DecayModeProto );
    AUX_VARIABLE( pantau_CellBasedInput_DecayMode );
    //BDT output distributions
    AUX_VARIABLE( pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n );
    AUX_VARIABLE( pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn );
    AUX_VARIABLE( pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn );
    //Variables used in BDTs
    AUX_VARIABLE( pantau_CellBasedInput_BDTVar_Basic_NNeutralConsts );
    AUX_VARIABLE( pantau_CellBasedInput_BDTVar_Charged_JetMoment_EtDRxTotalEt );
    AUX_VARIABLE( pantau_CellBasedInput_BDTVar_Charged_StdDev_Et_WrtEtAllConsts );
    AUX_VARIABLE( pantau_CellBasedInput_BDTVar_Neutral_HLV_SumM );
    AUX_VARIABLE( pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_1 );
    AUX_VARIABLE( pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2 );
    AUX_VARIABLE( pantau_CellBasedInput_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts );
    AUX_VARIABLE( pantau_CellBasedInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts );
    AUX_VARIABLE( pantau_CellBasedInput_BDTVar_Neutral_Shots_NPhotonsInSeed );
    AUX_VARIABLE( pantau_CellBasedInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged );
    
    //!PanTau variables when using eflowRec pfos
    //Flag whether this seed has pantau info
    AUX_VARIABLE( pantau_eflowRecInput_isPanTauCandidate );
    //decay modes (input mode [proto] and output mode )
    AUX_VARIABLE( pantau_eflowRecInput_DecayModeProto );
    AUX_VARIABLE( pantau_eflowRecInput_DecayMode );
    //BDT output distributions
    AUX_VARIABLE( pantau_eflowRecInput_BDTValue_1p0n_vs_1p1n );
    AUX_VARIABLE( pantau_eflowRecInput_BDTValue_1p1n_vs_1pXn );
    AUX_VARIABLE( pantau_eflowRecInput_BDTValue_3p0n_vs_3pXn );
    //Variables used in BDTs
    //NOTE: They are different from the CellBased ones in general!
    AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Basic_NPi0NeutConsts );
    AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Basic_NNeutralConsts );
    AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Charged_HLV_SumPt );
    AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Charged_Ratio_EtOverEtAllConsts );
    AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Neutral_HLV_SumM );
    AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_EtSort_1 );
    AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2 );
    AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts );
    AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Neutral_Mean_DRToLeading_WrtEtAllConsts );
    AUX_VARIABLE( pantau_eflowRecInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged );
    
  }
  
} // namespace xAOD
