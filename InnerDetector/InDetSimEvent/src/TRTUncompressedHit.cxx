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
  hitID(hit),
  m_partLink(track),
  particleEncoding(particle),
  kineticEnergy(kinEnergy),
  energyDeposit(eneDeposit),
  preStepX(preX), preStepY(preY), preStepZ(preZ),
  postStepX(postX), postStepY(postY), postStepZ(postZ),
  globalTime(time) {}

TRTUncompressedHit::TRTUncompressedHit(int hit,
                                       const HepMcParticleLink& partlink,
                                       int particle,
                                       float kinEnergy,
                                       float eneDeposit,
                                       float preX, float preY, float preZ,
                                       float postX, float postY, float postZ,
                                       float time):
  hitID(hit),
  m_partLink(partlink),
  particleEncoding(particle),
  kineticEnergy(kinEnergy),
  energyDeposit(eneDeposit),
  preStepX(preX), preStepY(preY), preStepZ(preZ),
  postStepX(postX), postStepY(postY), postStepZ(postZ),
  globalTime(time) {}

// Default constructor needed by athenaroot/athenapool
//
TRTUncompressedHit::TRTUncompressedHit( ) :
  hitID(0xffff),
  particleEncoding(0),
  kineticEnergy(0.0),
  energyDeposit(0.0),
  preStepX(0.0), preStepY(0.0), preStepZ(0.0),
  postStepX(0.0), postStepY(0.0), postStepZ(0.0),
  globalTime(0.0)
{
}

// barcode
int TRTUncompressedHit::GetTrackID() const
{
  return m_partLink.barcode();
}
