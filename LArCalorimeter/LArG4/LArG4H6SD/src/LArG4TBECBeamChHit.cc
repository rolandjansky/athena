/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4TBECBeamChHit

#include "LArG4TBECBeamChHit.h"

#include "G4Allocator.hh"
#include "G4ios.hh"
#include "globals.hh"

G4Allocator<LArG4TBECBeamChHit> LArG4TBECBeamChHitAllocator;

// Default constructor.  Prevent undefined values.
LArG4TBECBeamChHit::LArG4TBECBeamChHit()
  : m_Chamber(0)
  , m_X(0.)
  , m_Y(0.)
{}

LArG4TBECBeamChHit::LArG4TBECBeamChHit(G4int chamber, G4double x, G4double y)
  : m_Chamber(chamber)
  , m_X(x)
  , m_Y(y)
{;}

LArG4TBECBeamChHit::~LArG4TBECBeamChHit() {;}

void LArG4TBECBeamChHit::Draw() {;}

void LArG4TBECBeamChHit::Print()
{
  G4cout << "LArG4TBECBeamChHit: ";
	G4cout << "chamber " << m_Chamber;
  G4cout << " at ("
	 << m_X << ","
	 << m_Y << ")" << G4endl;
}
