/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4TBECBeamChHit.hh

#ifndef __LArG4TBECBeamChHit_H__
#define __LArG4TBECBeamChHit_H__

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"


class LArG4TBECBeamChHit : public G4VHit
{
public:

  // Constructors and destructors.
  LArG4TBECBeamChHit();
  LArG4TBECBeamChHit(G4int c = 0, G4double energy = 0., G4double time   = 0.);
  virtual ~LArG4TBECBeamChHit();

  inline void* operator new(size_t);
  inline void operator delete(void *aHit);

  void Draw();
  void Print();

private:
  G4int m_Chamber;
  G4double m_X;
  G4double m_Y;

public:
  // Note the lack of "Set" methods.  Once you've defined the position
  // of a hit, you can't change it.
  // 01-Feb-2001 WGS: Add accessor methods for calorID, zSideID, and region.
  inline G4int getIdentifier() const { return m_Chamber; }
  inline G4double X() const { return m_X; }
  inline G4double Y() const { return m_Y; }
};


// The following is copied from the Geant4 example; I'm not entirely
// sure why we need to define "new" and "delete".

typedef G4THitsCollection<LArG4TBECBeamChHit> LArG4TBECBeamChHitsCollection;

extern G4Allocator<LArG4TBECBeamChHit> LArG4TBECBeamChHitAllocator;

inline void* LArG4TBECBeamChHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) LArG4TBECBeamChHitAllocator.MallocSingle();
  return aHit;
}

inline void LArG4TBECBeamChHit::operator delete(void *aHit)
{
  LArG4TBECBeamChHitAllocator.FreeSingle((LArG4TBECBeamChHit*) aHit);
}

#endif // _LArG4TBECBeamChHit_H_
