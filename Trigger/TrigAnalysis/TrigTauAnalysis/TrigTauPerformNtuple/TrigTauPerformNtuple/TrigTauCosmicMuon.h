/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGCOSMICMUON_H_
#define  _TRIGCOSMICMUON_H_
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauCosmicMuon
/// Package : TauTrigPerformance
/// Author  : Soshi Tsuno (KEK)
/// Created : April 2009
///
/// DESCRIPTION:
///         cosmic muon based on the dedicated tracking algorithm for cosmic events
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "TLorentzVector.h"

class TrigTauCosmicMuon: public TLorentzVector
{
 public:
  TrigTauCosmicMuon();
  TrigTauCosmicMuon(const TrigTauCosmicMuon & other);
  virtual ~TrigTauCosmicMuon();
  virtual void reset();

  int   m_charge;
  float m_EtCone02;
  float m_EtCone04;
  float m_d0;
  float m_z0;
  float m_matchChi2;
  float m_fitChi2;
  int m_fitNumberDoF;
  int m_author;
  int m_hasCluster;
  int m_numberOfBLayerHits;
  int m_numberOfPixelHits;
  int m_numberOfSCTHits;
  int m_numberOfTRTHits;
  int m_numberOfTRTHighThresholdHits;
  int m_numberOfMDTHits;
  int m_numberOfCSCEtaHits;
  int m_numberOfCSCPhiHits;
  int m_numberOfRPCEtaHits;
  int m_numberOfRPCPhiHits;
  int m_numberOfTGCEtaHits;
  int m_numberOfTGCPhiHits;
  int m_bestMatch;
  int m_isCombinedMuon;
  int m_isStandAloneMuon;
  int m_isLowPtReconstructedMuon;
  int m_hasCombinedMuon;
  int m_hasInDetTrackParticle;
  int m_hasMuonExtrapolatedTrackParticle;
  int m_hasInnerExtrapolatedTrackParticle;
  int m_hasCombinedMuonTrackParticle;
  float m_etaAtClus;
  float m_phiAtClus;

  int m_CosmicTrackerID;

  enum CosmicTrackFitAlgo { 
    unknown                              = 0, 
    MuidMuonCollection                   = 1,  // in MuonContainer
    StacoMuonCollection                  = 2,  // in MuonContainer
    HLT_CosmicMuons                      = 10, // in CosmicMuonCollection (not implemented)
    MuTagTrackParticles                  = 20, // in Rec::TrackParticleContainer (not implemented)
    StacoTrackParticles                  = 21, // in Rec::TrackParticleContainer (not implemented)
    Combined_TrackParticles              = 22, // in Rec::TrackParticleContainer (not implemented)
    MuTagIMOTrackParticles               = 23, // in Rec::TrackParticleContainer (not implemented)
    MuonboyMuonSpectroOnlyTrackParticles = 24, // in Rec::TrackParticleContainer (not implemented)
    MuonboyTrackParticles                = 25, // in Rec::TrackParticleContainer (not implemented)
    MooreTrackParticles                  = 26, // in Rec::TrackParticleContainer (not implemented)
    TRTStandaloneTRTTracks               = 30, // in Trk::TrackCollection
    CombinedInDetTracks                  = 31, // in Trk::TrackCollection
    Combined_Tracks                      = 32, // in Trk::TrackCollection
    ConvertedMBoyMuonSpectroOnlyTracks   = 33, // in Trk::TrackCollection
    ConvertedMBoyTracks                  = 34, // in Trk::TrackCollection
    MuidExtrapolatedTracks               = 35, // in Trk::TrackCollection
    MooreTracks                          = 36  // in Trk::TrackCollection
  };

 private:
  ClassDef(TrigTauCosmicMuon, 1)  ;
};

#endif //TRIGCOSMICMUON_H
