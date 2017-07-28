/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_SimEvent/LUCID_SimHit.h"
#include "GetQuantumEfficiency.h"
#include <sstream>

LUCID_SimHit::LUCID_SimHit(): m_partLink() {

  m_tubeID        = 0;
  m_pdgCode       = 0;
  m_track         = 0;
  m_genVolume     = 0;
  m_stepStartPosX = 0;
  m_stepStartPosY = 0;
  m_stepStartPosZ = 0;
  m_stepEndPosX   = 0;
  m_stepEndPosY   = 0;
  m_stepEndPosZ   = 0;
  m_preStepTime   = 0;
  m_postStepTime  = 0;
  m_wavelength    = 0;
  m_energy        = 0;
}

LUCID_SimHit::LUCID_SimHit(const LUCID_SimHit& simhit)  { *this = simhit; }

LUCID_SimHit::LUCID_SimHit(short tubeID,
                           int   pdgCode,
                           int   track,
                           int   genVolume,
                           float stepStartPosX,
                           float stepStartPosY,
                           float stepStartPosZ,
                           float stepEndPosX,
                           float stepEndPosY,
                           float stepEndPosZ,
                           float preStepTime,
                           float postStepTime,
                           float wavelength,
                           float energy):

  m_tubeID       (tubeID),
  m_pdgCode      (pdgCode),
  m_track        (track), // Susumu Oda 2011.04.03
  m_partLink     (track),
  m_genVolume    (genVolume),
  m_stepStartPosX(stepStartPosX),
  m_stepStartPosY(stepStartPosY),
  m_stepStartPosZ(stepStartPosZ),
  m_stepEndPosX  (stepEndPosX),
  m_stepEndPosY  (stepEndPosY),
  m_stepEndPosZ  (stepEndPosZ),
  m_preStepTime  (preStepTime),
  m_postStepTime (postStepTime),
  m_wavelength   (wavelength),
  m_energy       (energy) {}

LUCID_SimHit::LUCID_SimHit(short tubeID,
                           int   pdgCode,
                           HepMcParticleLink partLink,
                           int   genVolume,
                           float stepStartPosX,
                           float stepStartPosY,
                           float stepStartPosZ,
                           float stepEndPosX,
                           float stepEndPosY,
                           float stepEndPosZ,
                           float preStepTime,
                           float postStepTime,
                           float wavelength,
                           float energy):

  m_tubeID       (tubeID),
  m_pdgCode      (pdgCode),
  m_track        (partLink.barcode()), // Susumu Oda 2011.04.03
  m_partLink     (partLink),
  m_genVolume    (genVolume),
  m_stepStartPosX(stepStartPosX),
  m_stepStartPosY(stepStartPosY),
  m_stepStartPosZ(stepStartPosZ),
  m_stepEndPosX  (stepEndPosX),
  m_stepEndPosY  (stepEndPosY),
  m_stepEndPosZ  (stepEndPosZ),
  m_preStepTime  (preStepTime),
  m_postStepTime (postStepTime),
  m_wavelength   (wavelength),
  m_energy       (energy) {}

std::string LUCID_SimHit::print() const {

  std::stringstream ss;

  ss << "LUCID_SimHit:";
  ss << " tubeID: "       << m_tubeID;
  ss << " preStepTime: "  << m_preStepTime;
  ss << " postStepTime: " << m_postStepTime;
  ss << " energy: "       << m_energy;
  ss << " wavelength : "  << m_wavelength;
  ss << " position: ("    << m_stepStartPosX
     << ","               << m_stepStartPosY
     << ","               << m_stepStartPosZ
     << ")";

  return ss.str();
}

const LUCID_SimHit& LUCID_SimHit::operator=(const LUCID_SimHit& simhit) {

  m_tubeID        = simhit.m_tubeID;
  m_pdgCode       = simhit.m_pdgCode;
  m_track         = simhit.m_track;
  m_partLink      = simhit.m_partLink;
  m_genVolume     = simhit.m_genVolume;
  m_stepStartPosX = simhit.m_stepStartPosX;
  m_stepStartPosY = simhit.m_stepStartPosY;
  m_stepStartPosZ = simhit.m_stepStartPosZ;
  m_stepEndPosX   = simhit.m_stepEndPosX;
  m_stepEndPosY   = simhit.m_stepEndPosY;
  m_stepEndPosZ   = simhit.m_stepEndPosZ;
  m_preStepTime   = simhit.m_preStepTime;
  m_postStepTime  = simhit.m_postStepTime;
  m_wavelength    = simhit.m_wavelength;
  m_energy        = simhit.m_energy;

  return *this;
}

bool LUCID_SimHit::isDetected(CLHEP::HepRandomEngine* rndEngine) const {

  double qef = GetQuantumEfficiencyR762(m_wavelength);
  double ran = CLHEP::RandFlat::shoot(rndEngine, 0., 1.);
  bool   det = (ran<qef);

  return det;
}

int LUCID_SimHit::operator == (const LUCID_SimHit& simhit) const { return (this==&simhit) ? 1 : 0; }

bool LUCID_SimHit::operator < (const LUCID_SimHit&    rhs) const { return m_tubeID < rhs.m_tubeID; }

int LUCID_SimHit::GetTrackID() const { return m_partLink.barcode(); }
