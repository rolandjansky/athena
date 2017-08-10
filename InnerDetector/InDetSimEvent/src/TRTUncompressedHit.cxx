/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimEvent/TRTUncompressedHit.h"


  // Called by TRTSensitiveDetector::ProcessHits

TRTUncompressedHit::TRTUncompressedHit(int hit,
                                       int track,
                                       int particle,
                                       float kinEnergy,
                                       float eneDeposit,
                                       float preX, float preY, float preZ,
                                       float postX, float postY, float postZ,
                                       float time):
  m_hitID(hit),
  m_partLink(track),
  m_particleEncoding(particle),
  m_kineticEnergy(kinEnergy),
  m_energyDeposit(eneDeposit),
  m_preStepX(preX), m_preStepY(preY), m_preStepZ(preZ),
  m_postStepX(postX), m_postStepY(postY), m_postStepZ(postZ),
  m_globalTime(time) {}

TRTUncompressedHit::TRTUncompressedHit(int hit,
                                       const HepMcParticleLink& partlink,
                                       int particle,
                                       float kinEnergy,
                                       float eneDeposit,
                                       float preX, float preY, float preZ,
                                       float postX, float postY, float postZ,
                                       float time):
  m_hitID(hit),
  m_partLink(partlink),
  m_particleEncoding(particle),
  m_kineticEnergy(kinEnergy),
  m_energyDeposit(eneDeposit),
  m_preStepX(preX), m_preStepY(preY), m_preStepZ(preZ),
  m_postStepX(postX), m_postStepY(postY), m_postStepZ(postZ),
  m_globalTime(time) {}

// Default constructor needed by athenaroot/athenapool
//
TRTUncompressedHit::TRTUncompressedHit( ) :
  m_hitID(0xffff),
  m_particleEncoding(0),
  m_kineticEnergy(0.0),
  m_energyDeposit(0.0),
  m_preStepX(0.0), m_preStepY(0.0), m_preStepZ(0.0),
  m_postStepX(0.0), m_postStepY(0.0), m_postStepZ(0.0),
  m_globalTime(0.0)
{
}

// barcode
int TRTUncompressedHit::GetTrackID() const
{
  return m_partLink.barcode();
}
