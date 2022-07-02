/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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


typedef G4THitsCollection<LArG4TBECBeamChHit> LArG4TBECBeamChHitsCollection;

#endif // _LArG4TBECBeamChHit_H_
