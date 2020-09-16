/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimpleScintillatorHit.h
//   Header file for class ScintillatorHit
//
// Author Andrea Di Simone. Based on ScintillatorHit by Thomas Kittelmann <kittel@nbi.dk>
///////////////////////////////////////////////////////////////////

#ifndef SIMPLESCINTILLATORHIT_H
#define SIMPLESCINTILLATORHIT_H

#include "GeneratorObjects/HepMcParticleLink.h"
#include "CLHEP/Vector/ThreeVector.h"

class SimpleScintillatorHit  {

public:

  SimpleScintillatorHit( const int particleEncoding,
		   const float kineticEnergy,
		   const float energyDeposit,
		   const CLHEP::Hep3Vector& position,
		   const float globalTime,
	           const int copyNo,
		   const int track);

  // Needed by athenaRoot apparently
  SimpleScintillatorHit();
  // Destructor:
  virtual ~SimpleScintillatorHit(); //temporary make it virtual for Pool!

  int GetParticleEncoding() const;
  float GetKineticEnergy() const;
  float GetEnergyDeposit() const;
  CLHEP::Hep3Vector GetPosition() const;
  float GetGlobalTime() const;
  int GetCopyNo() const;
  int GetTrackID() const;
  
  // link to the particle generating the hit
  const HepMcParticleLink& particleLink() const;

private:

  // Forbidden. (TK: why should the copy cons. be forbidden?
  // SimpleScintillatorHit(const SimpleScintillatorHit&);
  // SimpleScintillatorHit &operator=(const SimpleScintillatorHit&);

  int m_particleEncoding;
  float m_kineticEnergy;
  float m_energyDeposit;
  CLHEP::Hep3Vector m_position;
  float m_globalTime;
  int m_copyNo;
  HepMcParticleLink m_partLink;

};

// Inline definitions:
#include "SimpleScintillatorHit.icc"

#endif // SCINTILLATORHIT_H
