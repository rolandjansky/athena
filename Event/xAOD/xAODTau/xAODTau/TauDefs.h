// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauDefs.h 602664 2014-06-19 14:20:50Z janus $
#ifndef XAODTAU_TAUDEFS_H
#define XAODTAU_TAUDEFS_H

// Local include(s):
#include "Math/Vector4D.h"
#include <bitset>

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
  typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > PtEtaPhiMVectorF;


namespace TauJetParameters
{
    //-------------------------------------------------------------------------
    // DO NOT CHANGE THE ORDER OF THE ENUMS!
    // You can add, but changing the order may lead to disaster!
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    //! Enum for author of TauJet.
    //-------------------------------------------------------------------------
    enum Author
    {
        unknown = 0,
        tauRec  = 1,
        tau1P3P = 2,
        PanTau  = 3
    };

    //-------------------------------------------------------------------------
    //! Enum for discriminants
    //-------------------------------------------------------------------------
    enum TauID
    {
        //! discriminant for NN (used by tau1p3p)
        //DiscNN                = 1, 
        //! discriminant for LL (used by tauRec)
        Likelihood            = 2,
        //! discriminant for PDRS (used by tau1p3p)
        //DiscPDERS             = 3,
        //! discriminant for jet-tau separation LL (used by tauRec) 
        //TauJetLikelihoodOld   = 4,
        //! old discriminant for electron-tau separation LL (used by tauRec)
        //TauElLikelihoodOld    = 5,
        //! discriminant for NN (used by tauRec)
        //TauJetNeuralNetwork   = 6,
        //! discriminant for NN electron-tau separation (used by tauRec)   
        //TauENeuralNetwork     = 7,
        //! discriminant for flat acceptance NN (used by tau1p3p)
        //EfficNN               = 8,
        //! discriminant for flat acceptance PDERS (not used currently)
        //EfficPDERS            = 9,
        //! new ie july 2008 discriminant for electron-tau separation LL (used by tauRec)
        //TauElTauLikelihood    = 10,
        //! acceptance flag for cut analysis (used by tau1p3p)
        //DiscCut               = 11,
        //! acceptance flag for cuts optimized with TMVA (used by tau1p3p)
        //DiscCutTMVA           = 12,
        //! discriminant for LL (used by tau1p3p)
        //DiscLL                = 13,
        //! discriminant for flat acceptance LL (not used currently)
        //EfficLL               = 14,
        //! Boosted Decision Tree socre for Jet rejection (not transformed)
        BDTJetScore           = 15,
        //! Boosted Decision Tree score for electron rejection
        BDTEleScore           = 16,
        //! Safe LLH
        SafeLikelihood        = 17,
        //! BDT score which is signal transformed/flattened
        BDTJetScoreSigTrans   = 18, // new
        //! BDT score which is background transformed/flattened
        BDTJetScoreBkgTrans   = 19,  // new
        //! PanTau's Score
        PanTauScore           = 20
    };

    //-------------------------------------------------------------------------
    //! Enum for Veto flags
    //-------------------------------------------------------------------------
    enum VetoFlags
    {
        //! electron veto flag based on cuts (used by tau1p3p)
        ElectronFlag  = 0,
        //! electron veto flag based on egamma information (used by tau1p3p)
        EgammaFlag    = 1,
        //! muon veto flag based on ETCalo cut (used by tau1p3p)
        MuonFlag      = 2
    };

    //-------------------------------------------------------------------------
    //! Enum for IsTau flags
    //-------------------------------------------------------------------------
    enum IsTauFlag
    {
        TauCutLoose          = 0,
        TauCutMedium         = 1,
        TauCutTight          = 2,
        ElectronVeto         = 3, //Obsolete, dont use
        MuonVeto             = 4,
        TauLlhTight          = 5,
        TauLlhMedium         = 6,
        TauLlhLoose          = 7,
        //TauRecOld            = 8, //Obsolete, dont use?
        //Tau1P3POld           = 9, //Obsolete, dont use?
        //TauCutSafeLoose      = 10,
        //TauCutSafeMedium     = 11,
        //TauCutSafeTight      = 12,
        //TauCutSafeCaloLoose  = 13,
        //TauCutSafeCaloMedium = 14,
        //TauCutSafeCaloTight  = 15,
        ElectronVetoLoose    = 16,
        ElectronVetoMedium   = 17,
        ElectronVetoTight    = 18,
        JetBDTSigLoose          = 19, // rename (add sig)
        JetBDTSigMedium         = 20, // rename (add sig)
        JetBDTSigTight          = 21, // rename (add sig)
        EleBDTLoose          = 22,
        EleBDTMedium         = 23,
        EleBDTTight          = 24,
        //SafeLlhLoose         = 25,
        //SafeLlhMedium        = 26,
        //SafeLlhTight         = 27
        JetBDTBkgLoose       = 25, // new
        JetBDTBkgMedium      = 26, // new
        JetBDTBkgTight       = 27,  // new
        PanTauScoreLoose     = 28,
        PanTauScoreMedium    = 29,
        PanTauScoreTight     = 30
    };

    //-------------------------------------------------------------------------
    //! Enum for tau parameters - used mainly for backward compatibility with
    //! the analysis code
    //-------------------------------------------------------------------------
    enum Detail
    {
      ipZ0SinThetaSigLeadTrk,
      etOverPtLeadTrk,
      leadTrkPt,
      ipSigLeadTrk,
      // ipSigLeadLooseTrk,
      // etOverPtLeadLooseTrk,
      // leadLooseTrkPt,
      // chrgLooseTrk,
      massTrkSys,
      trkWidth2,
      trFlightPathSig,
      etEflow,
      mEflow,
      /// name accessors for electron LLH variables
      ele_E237E277,
      ele_PresamplerFraction,
      ele_ECALFirstFraction,
      //name accessors for topocluster ID variables
      numCells,
      /// get number of topocluster constituents of jet associated to tau candidate
      numTopoClusters,
      /// get number of effective topocluster constituents of jet associated to tau candidate !!! is a std::vector< float >
      numEffTopoClusters,
      /// get invariant mass from topoclusters of jet associated to tau candidate 
      topoInvMass,
      /// get invariant mass from effective topoclusters of jet associated to tau candidate 
      effTopoInvMass,
      /// get mean distance to leading topocluster for topoclusters of jet associated to tau candidate
      topoMeanDeltaR,
      /// get mean distance to leading topocluster for effective topoclusters of jet associated to tau candidate 
      effTopoMeanDeltaR,
      /////////////////////////////////////////////////////////////////////
      //
      // Applicable for calo seeded tau reconstruction
      //
      /////////////////////////////////////////////////////////////////////
      /// Get E_T radius
      EMRadius,
      /// Get hadron calorimeter radius
      hadRadius,
      /// Get EM energy at EM scale
      etEMAtEMScale,
      /// Get Hadronic energy at EM scale
      etHadAtEMScale,
      /// Get isolation fraction
      isolFrac,
      /// Get centrality fraction
      centFrac,
      /// Get strip width ^2
      stripWidth2,
      /// Get number of strips
      nStrip,
      /// Get calibrated EM transverse energy
      etEMCalib,
      /// Get calibrated hadronic transverse energy
      etHadCalib,
      /// Get eta of TauJet calculated from calorimeter
      seedCalo_eta,
      /// Get phi of TauJet calculated from calorimeter
      seedCalo_phi,
      /// Get number of isolated tracks
      // nIsolLooseTrk,
      /// Get the average track distance to calorimeter seed
      trkAvgDist,
      /// Get the RMS of track distance to calorimeter seed
      trkRmsDist,
      ///  Get sum of 2 leading cluster energy over all cluster energy
      lead2ClusterEOverAllClusterE,
      ///  Get sum of 3 leading cluster energy over all cluster energy
      lead3ClusterEOverAllClusterE,
      ///  Get sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  
      caloIso,
      ///  Get sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  (pile up corrected)
      caloIsoCorrected,
      ///  Get maximal dR of tracks associated to calo-seeded tau
      dRmax,
    

      ///migrate only seedTrk_ variables which are used in reco and ID and without prefix
      secMaxStripEt,
      sumEMCellEtOverLeadTrkPt,
      hadLeakEt,

      /// EM+TES final scale
      EM_TES_scale,
      /// LC+TES precalibration
      LC_TES_precalib,

 
      //////////////////////////////////////////////////////////////////////
      //
      // Cell-based energies in rings getters/setters
      //
      //////////////////////////////////////////////////////////////////////
      /// Ring 1: 0.00  < R < 0.05
      cellBasedEnergyRing1,
      /// Ring 2: 0.05  < R < 0.075
      cellBasedEnergyRing2,
      /// Ring 3: 0.075 < R < 0.10
      cellBasedEnergyRing3,
      /// Ring 4: 0.10  < R < 0.125
      cellBasedEnergyRing4,
      /// Ring 5: 0.125 < R < 0.15
      cellBasedEnergyRing5,
      /// Ring 6: 0.15  < R < 0.2
      cellBasedEnergyRing6,
      /// Ring 7: 0.2   < R < 0.4
      cellBasedEnergyRing7,
      /// TRT hits high threshold over low threshold
      TRT_NHT_OVER_NLT,
      /// @Tau Jet Vertex Fraction
      TauJetVtxFraction,

      //substructure details
      nPi0,
      nCharged,
      ptRatioEflow,
      
      nPi0Topo,
      nChargedTopo,
      etEflowTopo,
      mEflowTopo,
      ptRatioEflowTopo,
      

      //trigger details
      PSSFraction,
      ChPiEMEOverCaloEME,
      EMPOverTrkSysP,
      
      //for TES parameters
      TESOffset,
      TESCalibConstant

    };

    //-------------------------------------------------------------------------
    //! Enum for tau seed type
    //-------------------------------------------------------------------------
    enum SeedTypes
    {
        CaloObject = 0,
        Track      = 1
    };

  //-------------------------------------------------------------------------
    //! Enum for tau seed type
    //-------------------------------------------------------------------------
    enum TauCalibType
    {
        JetSeed=0,
        DetectorAxis=1,
        IntermediateAxis=2,
        TauEnergyScale=3,
        TauEtaCalib=4,
	PanTauEFlowRecProto=5,  //4-vector by using eflowRec pfos in seed
	PanTauEFlowRec=6,   //4-vector by adjusting eflowRec pfos using decay mode
	PanTauCellBasedProto=7, //4-vector by using CellBased pfos in seed
	PanTauCellBased=8  //4-vector by adjusting CellBased pfos using decay mode
    };


 

  //! ===== PanTau related code ====
  
  //-------------------------------------------------------------------------
  //! Enum for tau 4-vector type
  //-------------------------------------------------------------------------
  enum TauHLVType
    {
      Pantau_JetSeed=0,
      //other hlv variants I don't know about
      PanTau_eflowRec_Proto=1,  //4-vector by using eflowRec pfos in seed
      PanTau_eflowRec=2,   //4-vector by adjusting eflowRec pfos using decay mode
      PanTau_CellBased_Proto=3, //4-vector by using CellBased pfos in seed
      PanTau_CellBased=4  //4-vector by adjusting CellBased pfos using decay mode
    };
  
  
  //-------------------------------------------------------------------------
  //! Enum for tau details (scalar) calculated by pantau
  //-------------------------------------------------------------------------
  enum PanTauDetails
    {
      //!PanTau variables when using CellBased pfos
      //Flag whether this seed has pantau info
      pantau_CellBasedInput_isPanTauCandidate,
      //decay modes (input mode [proto] and output mode )
      pantau_CellBasedInput_DecayModeProto,
      pantau_CellBasedInput_DecayMode,
      //BDT output distributions
      pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n,
      pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn,
      pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn,
      //Variables used in BDTs
      pantau_CellBasedInput_BDTVar_Basic_NNeutralConsts,
      pantau_CellBasedInput_BDTVar_Charged_JetMoment_EtDRxTotalEt,
      pantau_CellBasedInput_BDTVar_Charged_StdDev_Et_WrtEtAllConsts,
      pantau_CellBasedInput_BDTVar_Neutral_HLV_SumM,
      pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_1,
      pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2,
      pantau_CellBasedInput_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts,
      pantau_CellBasedInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts,
      pantau_CellBasedInput_BDTVar_Neutral_Shots_NPhotonsInSeed,
      pantau_CellBasedInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged,
      
      //!PanTau variables when using eflowRec pfos
      //Flag whether this seed has pantau info
      pantau_eflowRecInput_isPanTauCandidate,
      //decay modes (input mode [proto] and output mode )
      pantau_eflowRecInput_DecayModeProto,
      pantau_eflowRecInput_DecayMode,
      //BDT output distributions
      pantau_eflowRecInput_BDTValue_1p0n_vs_1p1n,
      pantau_eflowRecInput_BDTValue_1p1n_vs_1pXn,
      pantau_eflowRecInput_BDTValue_3p0n_vs_3pXn,
      //Variables used in BDTs
      //NOTE: They are different from the CellBased ones in general!
      pantau_eflowRecInput_BDTVar_Basic_NPi0NeutConsts,
      pantau_eflowRecInput_BDTVar_Basic_NNeutralConsts,
      pantau_eflowRecInput_BDTVar_Charged_HLV_SumPt,
      pantau_eflowRecInput_BDTVar_Charged_Ratio_EtOverEtAllConsts,
      pantau_eflowRecInput_BDTVar_Neutral_HLV_SumM,
      pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_EtSort_1,
      pantau_eflowRecInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2,
      pantau_eflowRecInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts,
      pantau_eflowRecInput_BDTVar_Neutral_Mean_DRToLeading_WrtEtAllConsts,
      pantau_eflowRecInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged
      
    };
  
  
  //-------------------------------------------------------------------------
  //! Enum for tau constituents as used in PanTau 
  //-------------------------------------------------------------------------
  //NOTE: Not sure if we need those. They return the pfos used in PanTau, but this should be the same as the pfos linked to the tau
  enum PanTau_Constituents
    {
      //!Tau constituents based on cell based
      //Flag whether this seed has pantau info
      pantau_CellBasedInput_ChargedConstituents,
      pantau_CellBasedInput_Neutral
    };
    
}//end namespace TauJetParameters

}

#endif // XAODTAU_TAUDEFS_H
