/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4Hit.hh
// Definition of a hit in the ATLAS Liquid-Argon Barrel Accordion Calorimeter.
// 09-Nov-2000 Bill Seligman

// Based on exampleN04 in Geant 4.2.0ref2.

#ifndef __LArG4Hit_H__
#define __LArG4Hit_H__

#include "LArG4Code/LArG4Identifier.h"

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"


class LArG4Hit : public G4VHit
{
public:

  // Constructors and destructors.
  LArG4Hit();
  LArG4Hit(LArG4Identifier ident, 
		    G4double energy = 0.,
		    G4double time   = 0.);
  virtual ~LArG4Hit();

  inline void* operator new(size_t);
  inline void operator delete(void *aHit);

  void Draw();
  void Print();

private:
  LArG4Identifier m_identifier;
  G4double m_sumEnergy;
  G4double m_sumEnergyTime;

public:
  // Note the lack of "Set" methods.  Once you've defined the position
  // of a hit, you can't change it.  
  // 01-Feb-2001 WGS: Add accessor methods for calorID, zSideID, and region.
  inline LArG4Identifier getIdentifier() const { return m_identifier; }
  inline G4double energy() const { return m_sumEnergy; }

  inline G4double time() const
  {
    if (m_sumEnergy == 0.)
      {
	return 0.;
      }
    else
      {
	return m_sumEnergyTime / m_sumEnergy;
      }
  }

  // The following methods make life much easier for the class
  // LArG4SensitiveDetector.  They define comparisons between LArG4Hit
  // pointers that reference the underlying objects.  For example
  // given that p and q are LArG4Hit*, then p<q if the identifier of
  // (*p) is less than the identifier of (*q).

  bool Less(LArG4Hit* const& h) const { return m_identifier < h->m_identifier; } 
  bool Equals(LArG4Hit* const& h) const { return m_identifier == h->m_identifier; };
  void Add(LArG4Hit* const& h)
  {
    m_sumEnergy += h->m_sumEnergy;
    m_sumEnergyTime += h->m_sumEnergyTime;
  }

};


// The following is copied from the Geant4 example; I'm not entirely
// sure why we need to define "new" and "delete". 

typedef G4THitsCollection<LArG4Hit> LArG4HitsCollection;

extern G4Allocator<LArG4Hit> LArG4HitAllocator;

inline void* LArG4Hit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) LArG4HitAllocator.MallocSingle();
  return aHit;
}

inline void LArG4Hit::operator delete(void *aHit)
{
  LArG4HitAllocator.FreeSingle((LArG4Hit*) aHit);
}

#endif // _LArG4Hit_H_
