/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ScintillatorHit.h
//   Header file for class ScintillatorHit
//
// Author Thomas Kittelmann <kittel@nbi.dk>
///////////////////////////////////////////////////////////////////

#ifndef SCINTILLATORHIT_H
#define SCINTILLATORHIT_H

#include "GeneratorObjects/HepMcParticleLink.h"

class ScintillatorHit  {

public:

  ScintillatorHit( const int volNumber,
		   const int particleEncoding,
		   const float kineticEnergy,
		   const float energyDeposit,
		   const float preStepX,
		   const float preStepY,
		   const float preStepZ,
		   const float postStepX,
		   const float postStepY,
		   const float postStepZ,
		   const float globalPreStepX,
		   const float globalPreStepY,
		   const float globalPreStepZ,
		   const float globalPostStepX,
		   const float globalPostStepY,
		   const float globalPostStepZ,
		   const float globalTime,
		   const int track);

  // Needed by athenaRoot apparently
  ScintillatorHit();
  // Destructor:
  virtual ~ScintillatorHit(); //temporary make it virtual for Pool!

  int GetVolNumber() const;
  int GetTrackID() const;
  int GetParticleEncoding() const;
  float GetKineticEnergy() const;
  float GetEnergyDeposit() const;
  float GetPreStepX() const;
  float GetPreStepY() const;
  float GetPreStepZ() const;
  float GetPostStepX() const;
  float GetPostStepY() const;
  float GetPostStepZ() const;
  float GetGlobalPreStepX() const;
  float GetGlobalPreStepY() const;
  float GetGlobalPreStepZ() const;
  float GetGlobalPostStepX() const;
  float GetGlobalPostStepY() const;
  float GetGlobalPostStepZ() const;
  float GetGlobalTime() const;
  
  // link to the particle generating the hit
  const HepMcParticleLink& particleLink() const;

private:

  // Forbidden. (TK: why should the copy cons. be forbidden?
  //  ScintillatorHit(const ScintillatorHit&);
  //  ScintillatorHit &operator=(const ScintillatorHit&);

  int m_volNumber;
  int m_particleEncoding;
  float m_kineticEnergy;
  float m_energyDeposit;
  float m_preStepX;
  float m_preStepY;
  float m_preStepZ;
  float m_postStepX;
  float m_postStepY;
  float m_postStepZ;
  float m_globalPreStepX;
  float m_globalPreStepY;
  float m_globalPreStepZ;
  float m_globalPostStepX;
  float m_globalPostStepY;
  float m_globalPostStepZ;
  float m_globalTime;
  HepMcParticleLink m_partLink;

};

// Inline definitions:
#include "ScintillatorHit.icc"

#endif // SCINTILLATORHIT_H
