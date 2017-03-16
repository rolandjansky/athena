/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EnergySpot
// Prepared 30-Apr-2003 Bill Seligman

// This is a very primitive "hit" class for recording energy deposits
// created by fast shower models.

#ifndef __EnergySpot_h__
#define __EnergySpot_h__

#include "G4ThreeVector.hh"
#include "globals.hh"

class EnergySpot
{
public:
  // Constructors and destructors
  EnergySpot() 
    : m_point(G4ThreeVector())
    , m_energy(0)
    , m_time(0)
    {;}

  EnergySpot(const G4ThreeVector& P, G4double E, G4double t)
    : m_point(P)
    , m_energy(E) 
    , m_time(t)
  {;}

  virtual ~EnergySpot() {;}

  // Accessor methods
  inline void SetEnergy(const G4double& E) { m_energy = E; }
  inline G4double GetEnergy() const { return m_energy; }

  inline void SetPosition(const G4ThreeVector& P) { m_point = P; }
  inline G4ThreeVector GetPosition() const { return m_point; }

  inline void SetTime(const G4double t) { m_time = t; }
  inline G4double GetTime() const { return m_time; }

  G4bool operator==(const EnergySpot& eSpot) const
  {
    return ( m_energy == eSpot.m_energy  &&  m_point == eSpot.m_point );
  }

private:
  G4ThreeVector m_point;
  G4double m_energy;
  G4double m_time;
};

#endif
