/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigTauPerformNtuple/TrigTauCosmicMuon.h"
ClassImp(TrigTauCosmicMuon )

TrigTauCosmicMuon::TrigTauCosmicMuon()
  :  TLorentzVector(0,0,0,0)
  , m_charge(0)
  , m_EtCone02(0)
  , m_EtCone04(0)
  , m_d0(0)
  , m_z0(0)
  , m_matchChi2(0)
  , m_fitChi2(0)
  , m_fitNumberDoF(0)
  , m_author(0)
  , m_hasCluster(0)
  , m_numberOfBLayerHits(0)
  , m_numberOfPixelHits(0)
  , m_numberOfSCTHits(0)
  , m_numberOfTRTHits(0)
  , m_numberOfTRTHighThresholdHits(0)
  , m_numberOfMDTHits(0)
  , m_numberOfCSCEtaHits(0)
  , m_numberOfCSCPhiHits(0)
  , m_numberOfRPCEtaHits(0)
  , m_numberOfRPCPhiHits(0)
  , m_numberOfTGCEtaHits(0)
  , m_numberOfTGCPhiHits(0)
  , m_bestMatch(0)
  , m_isCombinedMuon(0)
  , m_isStandAloneMuon(0)
  , m_isLowPtReconstructedMuon(0)
  , m_hasCombinedMuon(0)
  , m_hasInDetTrackParticle(0)
  , m_hasMuonExtrapolatedTrackParticle(0)
  , m_hasInnerExtrapolatedTrackParticle(0)
  , m_hasCombinedMuonTrackParticle(0)
  , m_etaAtClus(0)
  , m_phiAtClus(0)
  , m_CosmicTrackerID(0)
{}

TrigTauCosmicMuon::TrigTauCosmicMuon(const TrigTauCosmicMuon & other)
  : TLorentzVector(other)
  , m_charge   (other.m_charge )
  , m_EtCone02 (other.m_EtCone02)
  , m_EtCone04(other.m_EtCone04)
  , m_d0(other.m_d0)
  , m_z0(other.m_z0)
  , m_matchChi2(other.m_matchChi2)
  , m_fitChi2(other.m_fitChi2)
  , m_fitNumberDoF(other.m_fitNumberDoF)
  , m_author(other.m_author)
  , m_hasCluster(other.m_hasCluster)
  , m_numberOfBLayerHits(other.m_numberOfBLayerHits)
  , m_numberOfPixelHits(other.m_numberOfPixelHits)
  , m_numberOfSCTHits(other.m_numberOfSCTHits)
  , m_numberOfTRTHits(other.m_numberOfTRTHits)
  , m_numberOfTRTHighThresholdHits(other.m_numberOfTRTHighThresholdHits)
  , m_numberOfMDTHits(other.m_numberOfMDTHits)
  , m_numberOfCSCEtaHits(other.m_numberOfCSCEtaHits)
  , m_numberOfCSCPhiHits(other.m_numberOfCSCPhiHits)
  , m_numberOfRPCEtaHits(other.m_numberOfRPCEtaHits)
  , m_numberOfRPCPhiHits(other.m_numberOfRPCPhiHits)
  , m_numberOfTGCEtaHits(other.m_numberOfTGCEtaHits)
  , m_numberOfTGCPhiHits(other.m_numberOfTGCPhiHits)
  , m_bestMatch(other.m_bestMatch)
  , m_isCombinedMuon(other.m_isCombinedMuon)
  , m_isStandAloneMuon(other.m_isStandAloneMuon)
  , m_isLowPtReconstructedMuon(other.m_isLowPtReconstructedMuon)
  , m_hasCombinedMuon(other.m_hasCombinedMuon)
  , m_hasInDetTrackParticle(other.m_hasInDetTrackParticle)
  , m_hasMuonExtrapolatedTrackParticle(other.m_hasMuonExtrapolatedTrackParticle)
  , m_hasInnerExtrapolatedTrackParticle(other.m_hasInnerExtrapolatedTrackParticle)
  , m_hasCombinedMuonTrackParticle(other.m_hasCombinedMuonTrackParticle)
  , m_etaAtClus(other.m_etaAtClus)
  , m_phiAtClus(other.m_phiAtClus)
  , m_CosmicTrackerID(other.m_CosmicTrackerID)
 {}

 TrigTauCosmicMuon::~TrigTauCosmicMuon() {}
 void TrigTauCosmicMuon::reset()
{
  SetPtEtaPhiM(0,0,0,0);
  m_charge =0; 
  m_EtCone02 =0; 
  m_EtCone04 =0; 
  m_d0 = 0;
  m_z0 = 0;
  m_matchChi2 = 0;
  m_fitChi2 = 0;
  m_fitNumberDoF = 0;
  m_author = 0;
  m_hasCluster = 0;
  m_numberOfBLayerHits = 0;
  m_numberOfPixelHits = 0;
  m_numberOfSCTHits = 0;
  m_numberOfTRTHits = 0;
  m_numberOfTRTHighThresholdHits = 0;
  m_numberOfMDTHits = 0;
  m_numberOfCSCEtaHits = 0;
  m_numberOfCSCPhiHits = 0;
  m_numberOfRPCEtaHits = 0;
  m_numberOfRPCPhiHits = 0;
  m_numberOfTGCEtaHits = 0;
  m_numberOfTGCPhiHits = 0;
  m_bestMatch = 0;
  m_isCombinedMuon = 0;
  m_isStandAloneMuon = 0;
  m_isLowPtReconstructedMuon = 0;
  m_hasCombinedMuon = 0;
  m_hasInDetTrackParticle = 0;
  m_hasMuonExtrapolatedTrackParticle = 0;
  m_hasInnerExtrapolatedTrackParticle = 0;
  m_hasCombinedMuonTrackParticle = 0;
  m_etaAtClus = 0;
  m_phiAtClus = 0;
  m_CosmicTrackerID = 0;
}

