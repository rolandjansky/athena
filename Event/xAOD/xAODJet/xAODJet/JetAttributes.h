// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////
///
/// \file JetAttributes.h
///  Defines enum to access jet attribute and associated particles/objects
///
///  The enum list is expected to follow 
///   https://twiki.cern.ch/twiki/bin/view/AtlasProtected/Run2JetMoments
///  and correspond to attributes build by official tools.
///
////////////////////////////////////////////////////
#ifndef XAODJET_JETMOMENTAATTRIBUTES_H
#define XAODJET_JETMOMENTAATTRIBUTES_H

namespace xAOD {
  
  struct JetAttribute {
    
    enum AttributeID {
      // Some attributes have dedicated access methods. 
      // They appear below so we follow Run2JetMoments but are commented and the access method is indicated.


      // AlgorithmType  use Jet::getAlgorithmType()
      // InputType      use Jet::getInputType()
      // SizeParameter  use Jet::getSizeParameter()

      // constituentLinks  use through Jet::getConstituents() or rawConstituent
      // constituentWeights    same as above. Practically unused yet.

      // ConstituentScale  use getConstituentsSignalState()
      
      // JetConstitScaleMomentum, JetEMScaleMomentum, JetLCScaleMomentum
      //    --> use Jet::jetP4(JetScale) 

      JetGhostArea,
      ActiveArea,
      ActiveArea4vec,

      VoronoiArea,
      VoronoiAreaE,
      VoronoiAreaPx,
      VoronoiAreaPy,
      VoronoiAreaPz,


      GhostMuonSegmentCount,
      GhostTrackCount,
      GhostTruthParticleCount,


      // grooming attributes
      TransformType,
      MuMax,
      YMin, 
      RClus,
      BDRS,
      DRFilt, 
      YFilt, 
      MuFilt,
      NSubjet,
      PtFrac,
      NTrimSubjets,
      RCut, 
      ZCut,


      // Attributes from moment builders
      JetOriginConstitScaleMomentum,

      
      GhostTruthAssociationFraction,
      GhostTrackAssociationFraction,

      HECQuality,
      LArBadHVEnergy,
      LArBadHVRatio,
      LArQuality,
      NegativeE,
      AverageLArQF,
      OotFracClusters10,
      OotFracClusters5,
      OotFracCells5,
      OotFracCells10,
      Timing,
      N90Constituents,
      N90Cells,

      BchCorrCell, 

      IsoDeltaR2SumPt,
      IsoDeltaR3SumPt,

      JVF,
      JVFLoose,
      Jvt, 
      JvtRpt, 
      JvtJvfcorr,

      NumTrkPt1000,
      NumTrkPt500,
      SumPtTrkPt1000,
      SumPtTrkPt500,
      TrackWidthPt1000,
      TrackWidthPt500,

      EMFrac,
      HECFrac,
      PSFrac,
      EnergyPerSampling,
      FracSamplingMax,
      FracSamplingMaxIndex,
      ECPSFraction,
      SamplingMax,


      Width,
      WidthPhi,

      LeadingClusterPt, 
      LeadingClusterSecondLambda, 
      LeadingClusterCenterLambda, 
      LeadingClusterSecondR ,

      
      CentroidR,

      // Substructure attributes
      KtDR,
      Tau1, 
      Tau2, 
      Tau3,
      Tau1_wta, 
      Tau2_wta, 
      Tau3_wta,
      Tau21, 
      Tau32,
      Tau21_wta, 
      Tau32_wta,
      Split12,
      Split23,
      Split34,
      ZCut12, 
      ZCut23, 
      ZCut34, 
      Dip12, 
      Dip13, 
      Dip23, 
      DipExcl12,
      ECF1, 
      ECF2, 
      ECF3, 
      ECF1_Beta2, 
      ECF2_Beta2, 
      ECF3_Beta2,
      C1, 
      C2, 
      D2, 
      C1_Beta2, 
      C2_Beta2, 
      D2_Beta2,
      ThrustMin, 
      ThrustMaj, 
      FoxWolfram0, 
      FoxWolfram1, 
      FoxWolfram2, 
      FoxWolfram3, 
      FoxWolfram4, 
      Sphericity, 
      Aplanarity,
      PullMag, 
      PullPhi, 
      Pull_C00, 
      Pull_C01, 
      Pull_C10, 
      Pull_C11,
      Charge,
      ShowerDeconstructionW, 
      ShowerDeconstructionTop,
      Volatility,



      // older or kept for compaitibility
      isBadLoose,
      isBadMedium,
      isBadTight,
      isUgly,
      PtTruth,

      IsoKR20Par,
      IsoKR20Perp,
      HighestJVFVtx,
      JetLabel,

      NumTowers, // no tools available yet

      BchCorrDotx, 
      BchCorrJet, 
      BchCorrJetForCell, 

      UnkownAttribute,


      // older run1 moments
      //AMAREA,
      // CALOAREA,
      // CORE_RBB,
      // CORE_RFILT,
      // CORR_JET,
      // CORR_JET_FORCELL,
      // subjetX_E,
      // subjetX_eta,
      // subjetX_m,
      // subjetX_phi,
      // subjetX_pt,
      // ELLIPTICAREA,
      // EMJES,
      // EMJES_EtaCorr,
      // EM_FRACTION_CLUSTER,
      // EM_FRACTION_MCTRUTH,
      // EM_FRACTION_RECOTRACKS,
      // EM_OVER_ESUM,
      // NOISEEXT,
      // N_BAD_CELLS,
      // N_BAD_CELLS_CORR,
      // TRACKS_DELTA_ETA,
      // TRACKS_DELTA_PHI,
      // TRACKS_P,
      // TRACKS_RMS_ETA,
      // TRACKS_RMS_PHI,
      // HULL_AREA,
      // HULL_LENGHT,
      // PtNearest,
      // SL_B_JES,
      //OotFracCells,
      //EtaOrigin,
      //PhiOrigin,
      //MOrigin,

    };

    enum AssoParticlesID {
      GhostTrack,
      GhostTruthParticle,
      GhostMuonSegment,
      OriginVertex,
    };
    
  };


}
#endif
