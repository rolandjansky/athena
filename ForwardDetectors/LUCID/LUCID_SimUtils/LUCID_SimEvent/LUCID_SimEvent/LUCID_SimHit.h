/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_SIMHIT_H
#define LUCID_SIMHIT_H 1

#include "GeneratorObjects/HepMcParticleLink.h"
#include "Identifier/Identifiable.h"
#include "CLHEP/Vector/ThreeVector.h"
#include <string>
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// LUCID_SimHit hold all information needed by the digitization algorithms
// to construct raw data objects.

class LUCID_SimHit  {

 public:

  LUCID_SimHit();
  LUCID_SimHit(const LUCID_SimHit&);
  LUCID_SimHit(short tubeID,
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
               float energy);
  LUCID_SimHit(short tubeID,
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
               float energy);

  std::string print() const;

  int GetTrackID() const;

  const LUCID_SimHit& operator=(const LUCID_SimHit& t);

  int  operator == (const LUCID_SimHit&) const;
  bool operator  < (const LUCID_SimHit&) const;

  const HepMcParticleLink& particleLink() const;

  inline float GetX() const { return m_stepStartPosX; }
  inline float GetY() const { return m_stepStartPosY; }
  inline float GetZ() const { return m_stepStartPosZ; }

  inline float GetEPX() const { return m_stepEndPosX; }
  inline float GetEPY() const { return m_stepEndPosY; }
  inline float GetEPZ() const { return m_stepEndPosZ; }

  inline short GetTubeID      () const { return m_tubeID; }
  inline int   GetTrack       () const { return m_track; }
  inline int   GetPdgCode     () const { return m_pdgCode; }
  inline int   GetGenVolume   () const { return m_genVolume; }
  inline float GetPreStepTime () const { return m_preStepTime; }
  inline float GetPostStepTime() const { return m_postStepTime; }
  inline float GetWavelength  () const { return m_wavelength; }
  inline float GetEnergy      () const { return m_energy; }

  bool isDetected(CLHEP::HepRandomEngine* rndEngine) const;

 private:

  short m_tubeID;
  int   m_pdgCode;
  int   m_track;
  HepMcParticleLink m_partLink; // link to the particle generating the hit
  int   m_genVolume;
  float m_stepStartPosX;
  float m_stepStartPosY;
  float m_stepStartPosZ;
  float m_stepEndPosX;
  float m_stepEndPosY;
  float m_stepEndPosZ;
  float m_preStepTime;
  float m_postStepTime;
  float m_wavelength;
  float m_energy;
};

inline const HepMcParticleLink& LUCID_SimHit::particleLink() const { return m_partLink; }

#endif
