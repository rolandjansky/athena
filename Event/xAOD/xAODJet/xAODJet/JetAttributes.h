// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJET_JETMOMENTAATTRIBUTES_H
#define XAODJET_JETMOMENTAATTRIBUTES_H

namespace xAOD {
  
  struct JetAttribute {
    
    enum AttributeID {

      GhostMuonSegmentCount,
      GhostTrackCount,
      GhostTruthParticleCount,
      ActiveArea,
      //ActiveArea4Vec,
      ActiveArea4vec,
      AverageLArQF,
      BchCorrCell, 
      BchCorrDotx, 
      BchCorrJet, 
      BchCorrJetForCell, 
      CentroidR,
      //      DRtoReco,
      HECQuality,
      IsoKR20Par,
      IsoKR20Perp,
      JVF,
      HighestJVFVtx,
      JetLabel,
      KtDR,
      LArBadHVEnergy,
      LArBadHVRatio,
      LArQuality,
      //LowEtConstituentsFrac,
      NegativeE,
      NumTowers, // no tools available yet
      //Offset,
      //OriginIndex    , replaced by HighestJVFVtx
      SamplingMax,
      Timing,
      // TrackMF,
      // TrackMFindex,
      // TruthMF,
      // TruthMFindex,
      VoronoiArea,
      VoronoiAreaE,
      VoronoiAreaPx,
      VoronoiAreaPy,
      VoronoiAreaPz,
      Width,
      // YFlip12, replaced by split12
      // YFlip13,
      EnergyPerSampling,
      FracSamplingMax,
      FracSamplingMaxIndex,
      EMFrac,
      HECFrac,
      isBadLoose,
      isBadMedium,
      isBadTight,
      isUgly,
      N90Constituents,
      N90Cells,
      OotFracClusters10,
      OotFracClusters5,
      OotFracCells5,
      OotFracCells10,
      NumTrkPt1000,
      NumTrkPt500,
      SumPtTrkPt1000,
      SumPtTrkPt500,
      TrackWidthPt1000,
      TrackWidthPt500,
      PtTruth,
      Tau1, 
      Tau2, 
      Tau3,
      Split12,
      Split23,
      Split34,
      Dip12, 
      Dip13, 
      Dip23, 
      DipExcl12,
      ThrustMin, 
      ThrustMaj, 
      FoxWolfram0, 
      FoxWolfram1, 
      FoxWolfram2, 
      FoxWolfram3, 
      FoxWolfram4, 
      Sphericity, 
      Aplanarity,

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
