/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformNtuple/TrigTauTrack.h"
ClassImp(TrigTauTrack) 

TrigTauTrack::TrigTauTrack::TrigTauTrack()
  : TLorentzVector(0,0,0,0)
  , m_chi2(0)
  , m_charge(0)
  , m_etaAtClus(0)
  , m_phiAtClus(0)
  , m_a0(0)
  , m_a0error(0)
  , m_nPixelHits(0)
  , m_nSCTHits(0)
  , m_nStrawHits(0)
  , m_nTRTHits(0)
  , m_nBLHits(0)
  , m_z0(0)
{}

TrigTauTrack::TrigTauTrack( const TrigTauTrack & other)
  : TLorentzVector(other)
  , m_chi2(other.m_chi2)
  , m_charge(other.m_charge)
  , m_etaAtClus(other.m_etaAtClus)
  , m_phiAtClus(other.m_phiAtClus)
  , m_a0(other.m_a0)
  , m_a0error(other.m_a0error)
  , m_nPixelHits(other.m_nPixelHits)
  , m_nSCTHits(other.m_nSCTHits)
  , m_nStrawHits(other.m_nTRTHits)
  , m_nTRTHits(other.m_nTRTHits)
  , m_nBLHits(other.m_nBLHits)
  , m_z0(other.m_z0)
{}

TrigTauTrack::~TrigTauTrack() {}
  
void TrigTauTrack::reset()
{
  SetPxPyPzE(0,0,0,0);
  m_chi2 = 0;  
  m_charge = 0;
  m_phiAtClus = 0;
  m_etaAtClus = 0;
  m_a0 = 0;
  m_a0error = 0;
  m_nPixelHits = 0;
  m_nSCTHits = 0;
  m_nStrawHits = 0;
  m_nTRTHits = 0;
  m_nBLHits = 0;
  m_z0 = 0;
}
