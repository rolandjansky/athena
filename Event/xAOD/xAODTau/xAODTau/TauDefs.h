// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauDefs.h 796092 2017-02-08 00:14:16Z griffith $
#ifndef XAODTAU_TAUDEFS_H
#define XAODTAU_TAUDEFS_H

// Local include(s):
#include "Math/Vector4D.h"
#include <bitset>

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
  typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > PtEtaPhiMVectorF;

	/* define type traits for xAOD Tau.
	 * needs to be outside of the TauJet namespace
	 */
	// empty return type for all types T.
	// Leads to a compilation failure if one tries to use it with other types then a specialization is given for.
	template <class T>
	struct xAODTAU_return_type;

	// specialization for ints
	template <>
	struct xAODTAU_return_type<int> { typedef int type; };

	// specialization for floats
	template <>
	struct xAODTAU_return_type<float> { typedef float type; };


namespace TauJetParameters
{
    //-------------------------------------------------------------------------
    // DO NOT CHANGE THE ORDER OF THE ENUMS!
    // You can add, but changing the order may lead to disaster!
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    //! Enum for discriminants
    //-------------------------------------------------------------------------
    enum TauID
      {
	//!
	// = 0,
        //! Likelihood of matched electron (PassEleOLR)
	EleMatchLikelihoodScore = 1, 
        //! needed by TauJet_v1/2
        Likelihood = 2, //deprecated
        //!
        // = 3,
        //!
        // = 4,
        //!
        // = 5,
        //! 
        // = 6,
        //!
        // = 7,
        //!
        // = 8,
        //!
        // = 9,
        //!
        // = 10,
        //!
        // = 11,
        //!
        // = 12,
        //!
        // = 13,
        //!
        // = 14,
        //! Boosted Decision Tree socre for Jet rejection (not transformed)
        BDTJetScore           = 15,
        //! Boosted Decision Tree score for electron rejection
        BDTEleScore           = 16,
        //! needed by TauJet_v1/2
        SafeLikelihood = 17, //deprecated
        //! BDT score which is signal transformed/flattened
        BDTJetScoreSigTrans   = 18, // new
        //!
        // = 19,  // new
        //! needed by TauJet_v1/2
        PanTauScore= 20, //deprecated
        //! RNN score for Jet rejection (not transformed)
        RNNJetScore           = 21,
        //! RNN score which is signal transformed/flattened
        RNNJetScoreSigTrans   = 22, // new
        //! RNN score for Ele rejection (not transformed)
        RNNEleScore           = 23,
        //! RNN score which is signal transformed/flattened
        RNNEleScoreSigTrans   = 24 // new
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
	PassEleOLR = 0, 
	// = 1, 
	// = 2, 
	// = 3, 
        MuonVeto   = 4,
	// = 5, 
	// = 6, 
	// = 7, 
        // = 8, 
        // = 9, 
        // = 10,
        // = 11,
        // = 12,
        // = 13,
        // = 14,
        // = 15,
        // = 16,
        // = 17,
        // = 18,
	JetBDTSigVeryLoose      = 18, // new in r21
        JetBDTSigLoose          = 19, // rename (add sig)
        JetBDTSigMedium         = 20, // rename (add sig)
        JetBDTSigTight          = 21, // rename (add sig)
        EleBDTLoose          = 22,
        EleBDTMedium         = 23,
        EleBDTTight          = 24,
        JetBDTBkgLoose       = 25, // new
        JetBDTBkgMedium      = 26, // new
        JetBDTBkgTight       = 27, // new
	JetRNNSigVeryLoose      = 28,
        JetRNNSigLoose          = 29,
        JetRNNSigMedium         = 30,
        JetRNNSigTight          = 31,
        EleRNNLoose          = 32,
        EleRNNMedium         = 33,
        EleRNNTight          = 34,	
      };

    //-------------------------------------------------------------------------
    //! Enum for tau parameters - used mainly for backward compatibility with
    //! the analysis code
    //-------------------------------------------------------------------------
    enum Detail
    {
      ipZ0SinThetaSigLeadTrk=0,
      etOverPtLeadTrk=1,
      leadTrkPt=2,
      ipSigLeadTrk=3,
      massTrkSys=4,
      trkWidth2=5,
      trFlightPathSig=6,
      // etEflow=7,//r21 cleanup
      // mEflow=8,//r21 cleanup
      // /// name accessors for electron LLH variables
      // ele_E237E277=9,//r21 cleanup
      // ele_PresamplerFraction=10,//r21 cleanup
      // ele_ECALFirstFraction=11,//r21 cleanup
      //name accessors for topocluster ID variables
      numCells=12,
      /// get number of topocluster constituents of jet associated to tau candidate
      numTopoClusters=13,
      /// get number of effective topocluster constituents of jet associated to tau candidate !!! is a std::vector< float >
      numEffTopoClusters=14,
      /// get invariant mass from topoclusters of jet associated to tau candidate 
      topoInvMass=15,
      /// get invariant mass from effective topoclusters of jet associated to tau candidate 
      effTopoInvMass=16,
      /// get mean distance to leading topocluster for topoclusters of jet associated to tau candidate
      topoMeanDeltaR=17,
      /// get mean distance to leading topocluster for effective topoclusters of jet associated to tau candidate 
      effTopoMeanDeltaR=18,
      /////////////////////////////////////////////////////////////////////
      //
      // Applicable for calo seeded tau reconstruction
      //
      /////////////////////////////////////////////////////////////////////
      /// Get E_T radius
      EMRadius=19,
      /// Get hadron calorimeter radius
      hadRadius=20,
      /// Get EM energy at EM scale
      etEMAtEMScale=21,
      /// Get Hadronic energy at EM scale
      etHadAtEMScale=22,
      /// Get isolation fraction
      isolFrac=23,
      /// Get centrality fraction
      centFrac=24,
      /// Get strip width ^2
      stripWidth2=25,
      /// Get number of strips
      nStrip=26,
      /// Get calibrated EM transverse energy (DEPRECATED since r19)
      // etEMCalib=27,//r21 cleanup
      /// Get calibrated hadronic transverse energy (DEPRECATED since r19)
      // etHadCalib=28,//r21 cleanup
      /// Get eta of TauJet calculated from calorimeter
      //      seedCalo_eta=29,//r21 cleanup same as detectorAxis Eta
      /// Get phi of TauJet calculated from calorimeter
      //      seedCalo_phi=30,//r21 cleanup same as detectorAxis Phi
      /// Get the average track distance to calorimeter seed
      trkAvgDist=31,
      /// Get the RMS of track distance to calorimeter seed
      trkRmsDist=32,
      ///  Get sum of 2 leading cluster energy over all cluster energy
      lead2ClusterEOverAllClusterE=33,
      ///  Get sum of 3 leading cluster energy over all cluster energy
      lead3ClusterEOverAllClusterE=34,
      ///  Get sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  
      caloIso=35,
      ///  Get sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  (pile up corrected)
      caloIsoCorrected=36,
      ///  Get maximal dR of tracks associated to calo-seeded tau
      dRmax=37,
    

      ///migrate only seedTrk_ variables which are used in reco and ID and without prefix
      secMaxStripEt=38,
      sumEMCellEtOverLeadTrkPt=39,
      hadLeakEt=40,

      /// EM+TES final scale
      //EM_TES_scale=41,//r21 cleanup
      /// LC+TES precalibration
      //LC_TES_precalib=42,//r21 cleanup

 
      //////////////////////////////////////////////////////////////////////
      //
      // Cell-based energies in rings getters/setters
      //
      //////////////////////////////////////////////////////////////////////
      /// Ring 1: 0.00  < R < 0.05
      cellBasedEnergyRing1=43,
      /// Ring 2: 0.05  < R < 0.075
      cellBasedEnergyRing2=44,
      /// Ring 3: 0.075 < R < 0.10
      cellBasedEnergyRing3=45,
      /// Ring 4: 0.10  < R < 0.125
      cellBasedEnergyRing4=46,
      /// Ring 5: 0.125 < R < 0.15
      cellBasedEnergyRing5=47,
      /// Ring 6: 0.15  < R < 0.2
      cellBasedEnergyRing6=48,
      /// Ring 7: 0.2   < R < 0.4
      cellBasedEnergyRing7=49,
      /// TRT hits high threshold over low threshold
      TRT_NHT_OVER_NLT=50,
      /// @Tau Jet Vertex Fraction
      TauJetVtxFraction=51,

      //substructure details
      //nPi0=52,//r21 cleanup
      nCharged=53,
      //ptRatioEflow=54,//r21 cleanup
      
      //nPi0Topo=55, //deprecated//r21 cleanup
      //nChargedTopo=56,//r21 cleanup
      //etEflowTopo=57,//r21 cleanup
      //mEflowTopo=58,//r21 cleanup
      //ptRatioEflowTopo=59,//r21 cleanup
      

      //trigger details
      PSSFraction=60,
      ChPiEMEOverCaloEME=61,
      EMPOverTrkSysP=62,
      
      //for TES parameters
      TESOffset=63,
      TESCalibConstant=64,

      centFracCorrected=65,
      etOverPtLeadTrkCorrected=66,
      innerTrkAvgDist=67,
      innerTrkAvgDistCorrected=68,
      SumPtTrkFrac=69,
      SumPtTrkFracCorrected=70,
      
      mEflowApprox=71,
      ptRatioEflowApprox=72,

      /// pileup-corrected ID variables
      ipSigLeadTrkCorrected=73,
      trFlightPathSigCorrected=74,
      massTrkSysCorrected=75,
      dRmaxCorrected=76,
      ChPiEMEOverCaloEMECorrected=77,
      EMPOverTrkSysPCorrected=78,
      ptRatioEflowApproxCorrected=79,
      mEflowApproxCorrected=80,



      //currently dynamic enums
      // CaloSamplingEtaEM
      // CaloSamplingEtaHad
      // CaloSamplingPhiEM
      // CaloSamplingPhiHad
      ClustersMeanCenterLambda=100,
      ClustersMeanEMProbability=101,
      ClustersMeanFirstEngDens=102,
      ClustersMeanSecondLambda=103,
      ClustersMeanPresamplerFrac=104,
      GhostMuonSegmentCount=105,//this is just a copy of the jet-seed detail
      PFOEngRelDiff=106,
      LC_pantau_interpolPt=107,
      electronLink=108,
      
      nChargedTracks=109,
      nIsolatedTracks=110,
      nModifiedIsolationTracks=111,//no static variable for now

      //Tau/Ele BDTVars

    };


  //-------------------------------------------------------------------------
    //! Enum for tau calib type
    //-------------------------------------------------------------------------
    enum TauCalibType
    {
        JetSeed=0,
        DetectorAxis=1,
        IntermediateAxis=2,
        TauEnergyScale=3,
        TauEtaCalib=4,
        // =5,  
        // =6,   
        PanTauCellBasedProto=7, //4-vector by using CellBased pfos in seed
        PanTauCellBased=8,  //4-vector by adjusting CellBased pfos using decay mode
        TrigCaloOnly=9,
        FinalCalib=10,
    };


   
  //-------------------------------------------------------------------------
  //! Enum for tau details (scalar) calculated by pantau
  //-------------------------------------------------------------------------
  enum PanTauDetails
    {
      //Flag whether this seed has pantau info
      PanTau_isPanTauCandidate=0,
      //decay modes (input mode [proto] and output mode )
      PanTau_DecayModeProto=1,
      PanTau_DecayMode=2,
      //BDT output distributions
      PanTau_BDTValue_1p0n_vs_1p1n=3,
      PanTau_BDTValue_1p1n_vs_1pXn=4,
      PanTau_BDTValue_3p0n_vs_3pXn=5,
      //Variables used in BDTs
      PanTau_BDTVar_Basic_NNeutralConsts=6,
      PanTau_BDTVar_Charged_JetMoment_EtDRxTotalEt=7,
      PanTau_BDTVar_Charged_StdDev_Et_WrtEtAllConsts=8,
      PanTau_BDTVar_Neutral_HLV_SumM=9,
      PanTau_BDTVar_Neutral_PID_BDTValues_BDTSort_1=10,
      PanTau_BDTVar_Neutral_PID_BDTValues_BDTSort_2=11,
      PanTau_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts=12,
      PanTau_BDTVar_Neutral_Ratio_EtOverEtAllConsts=13,
      PanTau_BDTVar_Neutral_Shots_NPhotonsInSeed=14,
      PanTau_BDTVar_Combined_DeltaR1stNeutralTo1stCharged=15,
      PanTau_DecayModeExtended=16,
      
      // new variables for pantau BDT
      PanTau_BDTVar_Charged_HLV_SumM=32,

    };
  
  
  enum DecayMode 
    {
      Mode_1p0n=0,
      Mode_1p1n=1,
      Mode_1pXn=2,
      Mode_3p0n=3,
      Mode_3pXn=4,
      Mode_Other=5,   // for 2p, 4p, 5p taus
      Mode_NotSet=6,  // for 0p, >= 6p
      Mode_Error=7,    // use this as initialisation
    };

  //-------------------------------------------------------------------------
  //! Enum for tau track flags
  //-------------------------------------------------------------------------
  enum TauTrackFlag
    {
      isConversionOld    = 0,
      failTrackFilter = 1,
      coreTrack = 2, //all tracks dR<0.2
      wideTrack = 3, //all tracks 0.2<dR<0.4
      passTrkSelector = 4, // pass the old track selector tool;  old tau tracks are coreTrack&passTrkSelector
      classifiedCharged    = 5, // classified as charged track from tau decay
      classifiedIsolation  = 6, // track from the same interaction as the tau candidate: to be used for isolation
      classifiedConversion = 7, // classified as conversion by the global track classifier
      classifiedFake       = 8, // track classified as pileup or fake tracks (should not be used for isolation computation)
      unclassified         = 9,  // helper enum for not yet classified tracks
      passTrkSelectionTight = 10, //pass the xAOD Tight track selection
      modifiedIsolationTrack = 11, //passTrkSelector && !classifiedCharged
      //if you add another flag, you'll need to check the track sorting alg, it assumes unclassified is the last flag *This should be fixed in TauTrackClassifier now*
      //Track sorting just requires that the classifiedXXX bits are consecutive, charged is first (of classifiedXXX), and fake is last (of classifiedXXX)
    };

  enum TrackDetail
    {
      CaloSamplingEtaEM = 0,
      CaloSamplingEtaHad = 1,
      CaloSamplingPhiEM = 2,
      CaloSamplingPhiHad = 3,
    };

  struct ROOT6_NamespaceAutoloadHook{};
}//end namespace TauJetParameters

}

#endif // XAODTAU_TAUDEFS_H
